/* Copyright (c) 2016 Nicholas Esterer. All rights reserved. */

#include "synth_control.h"
#include "midi_setup.h" 
#include "poly_management.h" 
#include "wavetables.h" 
#include "mm_time.h" 
#include <math.h> 
#include "audio_setup.h" 
#include "scheduling.h" 
#include "quantization_tables.h" 
#include "env_map.h" 
#include "mm_mark_zerox.h" 
#include <string.h> 
#include "mm_common_calcs.h" 
#include "leds.h" 
#include "_gend_tempo_map_table_header.h"

#ifdef DEBUG
 #include <assert.h>
#endif  

typedef struct __uni_stuff_t uni_stuff_t;

struct __uni_stuff_t {
    uni_stuff_t *next;
    void (*reset_cb)(void*); /* Function to call when uni_state reset */
    void *data;           /* Data to pass to that function */
};

uni_stuff_t *uni_reset_items_head = NULL;
static int num_fbk_presses = 0;

/* Keeps track of how many times a note has been played. If the number of times
 * is divisible by the note-parameter set's intermittency value, this count gets
 * reset */
int noteOnEventCount[NUM_NOTE_PARAM_SETS];

/* Stuff that could be saved */
NoteParamSet                noteParamSets[NUM_NOTE_PARAM_SETS];
/* The tempo before tempo scaling has been applied */
static float                tempoBPM_prescale; 
/* The tempo representing how often notes are scheduled, etc. */
static float                tempoBPM; 
/* value in [0,1) representing the point in the coarse tempo range */
static float                tempo_coarse_norm;
/* value in [0,1) representing the point in the fine tempo range */
static float                tempo_fine_norm;
static float                tempoBPM_scale;
static const float          tempoBPM_scale_table[] = 
                                SYNTH_CONTROL_TEMPOBPM_SCALE_TABLE;
static const float          eventDelta_quant_table[] = 
                                SYNTH_CONTROL_EVENTDELTA_QUANT_TABLE;
static const int            intermittency_table[] =
                                SYNTH_CONTROL_INTERMITTENCY_TABLE;
SynthControlDeltaButtonMode deltaButtonMode;
SynthControlGainMode        gainMode;
SynthControlRecMode         recMode;
SynthControlPitchMode       pitchMode;
static int                  uni_stuff_changed = 0;

/* Stuff that shouldn't really be saved */
int                         noteDeltaFromBuffer;
SynthControlEditingWhichParamsIndex editingWhichParams;
int                         currentPreset;
int                         feedbackState;
int                         scheduleRecording;
int                         firstScheduledRecording;
SynthControlPitchIndex      editing_which_pitch;
/* Is the scheduler on or off ? */
int                         schedulerState;
/* What preset would be recalled/stored. First preset is numbered 0. */
SynthControlPresetNumber    presetNumber;
/* If non-zero it means a recording has been made, otherwise it means no
recording has yet been made */
static int recording_exists = 0;

static void schedulerState_off_helper(void *data);
static void schedulerState_on_helper(void);
static void synth_control_fbk_tog_setup(void);
static void synth_control_reset_aux_note_all_params(void);

static int expr_ctl_chosen = 0;

/* This is actually now just used to indicate what we are controlling: STRIDE, ABS or UNI */
SynthControlPosMode posMode = SYNTH_CONTROL_DEFAULT_POSMODE; 

void
synth_control_reset_noteStrideAcc_note(int note)
{
    noteParamSets[note].noteStrideAcc = 0;
}

void
synth_control_reset_noteStrideAcc(void)
{
    int n;
    for (n = 0; n < NUM_NOTE_PARAM_SETS; n++) {
        synth_control_reset_noteStrideAcc_note(n);
    }
}

static int noteSched_scheduling_helper(NoteSchedEvent *nse)
{
    if (recording_exists == 0) {
        /* We don't schedule, free the event */
        free(nse);
        return 0;
    }
    /* Reset note stride accumulator. */
    synth_control_reset_noteStrideAcc();
    /* schedule the noteSchedEvent */
    schedule_noteSched_event(0,nse);
    return 1;
}

void autorelease_on_done(MMEnvedSamplePlayer * esp)
{
    MMWavTab_dec_n_players(esp->spsp.samples);
    pm_yield_params_to_allocator((void*)&voiceAllocator,
            (void *)&(MMEnvedSamplePlayer_getSamplePlayerSigProc(esp).note));
    /* Remove callback from sample player */
    esp->onDone = NULL;
    /* Set samples to NULL and therefore cease it from playing */
    esp->spsp.samples = NULL;
}

void synth_control_set_envelopeTime(float envelopeTime_param,
                                    int note_param_idx)
{
    /* The attack and release are specified as fractions of the length of the
    audio, so the maxmimum attack and release are 0.5 and minimum are 0 (so that
    their sum never excede 1) */
    env_map_attack_release_f(
            &noteParamSets[note_param_idx].attackTime,
            &noteParamSets[note_param_idx].releaseTime,
            envelopeTime_param,
            0,
            0.5,
            0,
            0.5);
}

void synth_control_set_envelopeTime_curParams(float envelopeTime_param)
{
    synth_control_set_envelopeTime(envelopeTime_param,
            synth_control_get_editingWhichParams());
}

void synth_control_envelopeTime_control(void *data_, float envelopeTime_param)
{
    synth_control_set_envelopeTime_curParams(envelopeTime_param);
}

void synth_control_set_sustainTime(float sustainTime_param,
                                    int note_param_idx)
{
    /* Sustain time is relative to length of recording, so here just 0-1.
     * It is scaled this way so that the length selection is more precise for
     * short lengths and less precise for longer ones */
    noteParamSets[note_param_idx].sustainTime
        = powf(2.,-7.*(1 - sustainTime_param));
}

void synth_control_set_sustainTime_curParams(float sustainTime_param)
{
    synth_control_set_sustainTime(sustainTime_param,
            synth_control_get_editingWhichParams());
}

void synth_control_sustainTime_control(void *data_, float sustainTime_param)
{
    synth_control_set_sustainTime_curParams(sustainTime_param);
}

static void set_tempoBPM_scale(float tempo_scale)
{
    tempoBPM_scale = tempo_scale;
}

static void apply_tempo_scale(void)
{
    tempoBPM = tempoBPM_prescale / synth_control_get_tempoBPM_scale();
}

static void set_tempoBPM_prescale(float tempoBPM_param)
{
    tempoBPM_prescale = tempoBPM_param;
}

void synth_control_tempoNudge(float tempoNudge_param)
{
    /* the tempo is calculated so that 1 buffer * K is played per 1 beat where K
     * is some scalar. The value is negated so if K < 1, the tempo is slower and
     * K > 1 the tempo is faster */
    MMSample K =  1.05 - tempoNudge_param * 0.1;
    float _tempoBPM = 60. * (MMSample)audio_hw_get_sample_rate(NULL) 
            / ((MMSample)((MMArray*)theSound->wavtab)->length * K);
    set_tempoBPM_prescale(_tempoBPM);
    apply_tempo_scale();
}

static void set_tempo_scale(float tempo_scale)
{
    tempoBPM_scale=tempo_scale;
}

/* sets tempo and resets fine and scaling controls.
 * This does NOT check if the argument is a valid tempo. A valid tempo is one
 * that is between the limits defined in this file's header and is a number. */
void synth_control_set_tempoBPM_absolute(float _tempoBPM)
{
    set_tempo_scale(SYNTH_CONTROL_DEFAULT_TEMPOBPM_SCALE);
    set_tempoBPM_prescale(_tempoBPM);
    apply_tempo_scale();
}

/*
We would really like to have 2 different resolutions available for selecting the
tempo: a lower resolution with the knob because it is hard to be more accurate,
and a higher resolution with MIDI. But this is very difficult to manage (imagine
you set the coarse tempo with MIDI and the fine tempo with the knob, you might
have a tempo that is out of bounds). So instead we opt for a coarse tempo
control using table lookup and simply scale the tempo value by +- 10% depending
on the fine control. We keep the whole table though for the future (it is saved
to flash).
*/

static void update_lookup_tempo(void)
{
    float fine_tempo_scale = tempo_fine_norm * .2 + 0.9,
          tempo_bpm = tempo_map_table_lookup(tempo_coarse_norm,0.5) * fine_tempo_scale;
    set_tempoBPM_prescale(tempo_bpm);
    apply_tempo_scale();
}

/* Param should be in [0,1] */
void synth_control_set_tempo_coarse_norm(float param)
{
    tempo_coarse_norm = param;
    update_lookup_tempo();
}

/* Param should be in [0,1] */
void synth_control_set_tempo_fine_norm(float param)
{
    tempo_fine_norm = param;
    update_lookup_tempo();
}

/* Param should be in [0,1) */
void synth_control_set_tempo_scale_norm(float param)
{
    int _tmp = (int)round(SYNTH_CONTROL_TEMPOBPM_SCALE_TABLE_LENGTH
                            * param);
    if (_tmp >= SYNTH_CONTROL_TEMPOBPM_SCALE_TABLE_LENGTH) {
        _tmp = SYNTH_CONTROL_TEMPOBPM_SCALE_TABLE_LENGTH - 1;
    }
    if (_tmp < 0) {
        _tmp = 0;
    }
    set_tempo_scale(tempoBPM_scale_table[_tmp]);
    apply_tempo_scale();
}

static void update_fade_rates(int note_param_idx);

void synth_control_set_numRepeats(int numRepeats_param, int note_params_idx)
{
    noteParamSets[note_params_idx].numRepeats = numRepeats_param;
    update_fade_rates(note_params_idx);
}

void synth_control_set_numRepeats_curParams(int numRepeats_param)
{
    synth_control_set_numRepeats(numRepeats_param,
            synth_control_get_editingWhichParams());
}

void synth_control_set_repeats(float repeats_param)
{
    synth_control_set_numRepeats_curParams((int)(
                ((float)SYNTH_CONTROL_MAX_NUM_REPEATS) * repeats_param));        
}

/* Set absolute pitch according to MIDI notes (60 == no transposition) */
void synth_control_set_pitch(float pitch_param,
                             int which_pitch,
                             int note_params_idx)
{
    MMSample _tmp = pitch_param - 60 + SYNTH_CONTROL_PITCH_OFFSET;
    noteParamSets[note_params_idx].pitches[which_pitch] = _tmp;
    _tmp = MMCC_et12_rate(_tmp + noteParamSets[note_params_idx].fine_pitches[which_pitch]);
    noteParamSets[note_params_idx].rate_busses[which_pitch] = mm_q8_24_t_from_MMSample(_tmp);
}

void synth_control_set_pitch_curParams(float pitch_param)
{
    synth_control_set_pitch(pitch_param,
                            synth_control_get_editing_which_pitch(),
                            synth_control_get_editingWhichParams());
}

/* Set the pitch to any pitch between midi note 48 and 72, limited by the
 * precision of pitch_param. Input can be [0,1]*/
void synth_control_set_pitch_chrom(float pitch_param,
                                   int which_pitch,
                                   int note_params_idx)
{
    noteParamSets[note_params_idx].pitches[which_pitch]
        = -12 + 24 * pitch_param;
}

void synth_control_set_pitch_chrom_curParams(float pitch_param)
{
    synth_control_set_pitch_chrom(pitch_param,
            synth_control_get_editing_which_pitch(),
            synth_control_get_editingWhichParams());
}

/* Set pitch to a midi note. Rounds according to quantization parameters. */
void synth_control_set_pitch_chrom_quant(float param,
                                         int which_pitch,
                                         int note_params_idx)
{
    float _tmp;
    _tmp = round(((SYNTH_CONTROL_PITCH_CHROM_MAX 
                    - SYNTH_CONTROL_PITCH_CHROM_MIN)
                / SYNTH_CONTROL_PITCH_CHROM_QUANT + 1) 
            * param)
            * SYNTH_CONTROL_PITCH_CHROM_QUANT 
            + SYNTH_CONTROL_PITCH_CHROM_MIN;
    if (_tmp > SYNTH_CONTROL_PITCH_CHROM_MAX) {
        _tmp = SYNTH_CONTROL_PITCH_CHROM_MAX;
    }
    if (_tmp < SYNTH_CONTROL_PITCH_CHROM_MIN) {
        _tmp = SYNTH_CONTROL_PITCH_CHROM_MIN;
    }
    _tmp += SYNTH_CONTROL_PITCH_OFFSET;
    noteParamSets[note_params_idx].pitches[which_pitch]
        = _tmp;
    /* Compute Q8_24 value */
    _tmp = MMCC_et12_rate(_tmp + noteParamSets[note_params_idx].fine_pitches[which_pitch]);
    noteParamSets[note_params_idx].rate_busses[which_pitch] = mm_q8_24_t_from_MMSample(_tmp);
}

void synth_control_set_pitch_chrom_quant_curParams(float param)
{
    synth_control_set_pitch_chrom_quant(param,
            synth_control_get_editing_which_pitch(),
            synth_control_get_editingWhichParams());
}

/* expects input to be in [0,1] */
void synth_control_set_pitch_fine_quant(float param, 
                                        int which_pitch,
                                        int note_param_idx)
{
    float _tmp;
    _tmp = round(((SYNTH_CONTROL_PITCH_FINE_MAX 
                    - SYNTH_CONTROL_PITCH_FINE_MIN)
                / SYNTH_CONTROL_PITCH_FINE_QUANT) 
            * param)
            * SYNTH_CONTROL_PITCH_FINE_QUANT 
            + SYNTH_CONTROL_PITCH_FINE_MIN;
    if (_tmp > SYNTH_CONTROL_PITCH_FINE_MAX) {
        _tmp = SYNTH_CONTROL_PITCH_FINE_MAX;
    }
    if (_tmp < SYNTH_CONTROL_PITCH_FINE_MIN) {
        _tmp = SYNTH_CONTROL_PITCH_FINE_MIN;
    }
    noteParamSets[note_param_idx].fine_pitches[which_pitch]
        = _tmp;
    /* Compute Q8_24 value */
    _tmp = MMCC_et12_rate(_tmp + noteParamSets[note_param_idx].pitches[which_pitch]);
    noteParamSets[note_param_idx].rate_busses[which_pitch] = mm_q8_24_t_from_MMSample(_tmp);
}

void synth_control_set_pitch_fine_curParams(float param)
{
    synth_control_set_pitch_fine_quant(param,
            synth_control_get_editing_which_pitch(),
            synth_control_get_editingWhichParams());
}

void synth_control_pitch_control(void *data_, float pitch_param)
{
    synth_control_set_pitch_chrom_curParams(pitch_param);
}

void synth_control_set_startPoint(float startPoint_param, int note_params_idx)
{
    noteParamSets[note_params_idx].startPoint
        = startPoint_param;
}

void synth_control_set_startPoint_curParams(float startPoint_param)
{
    synth_control_set_startPoint(startPoint_param,
            synth_control_get_editingWhichParams());
}

void synth_control_set_positionStride(float positionStride_param, int note_params_idx)
{
    noteParamSets[note_params_idx].positionStride
        = positionStride_param * SYNTH_CONTROL_POS_STRIDE_SCALE 
            - SYNTH_CONTROL_POS_STRIDE_OFFSET;
}

void synth_control_set_positionStride_curParams(float positionStride_param)
{
    synth_control_set_positionStride(positionStride_param,
            synth_control_get_editingWhichParams());
}

void synth_control_set_noteStride(float noteStride_param, int note_params_idx)
{
    noteParamSets[note_params_idx].noteStride
        = noteStride_param * SYNTH_CONTROL_POS_STRIDE_SCALE 
            - SYNTH_CONTROL_POS_STRIDE_OFFSET;
}

void synth_control_set_noteStride_curParams(float noteStride_param)
{
    synth_control_set_noteStride(noteStride_param,
            synth_control_get_editingWhichParams());
}

void synth_control_set_eventDelta_quant(float eventDeltaBeats_param, int note_params_idx)
{
    int _tmp;
    _tmp = (int)round(SYNTH_CONTROL_EVENTDELTA_QUANT_TABLE_LENGTH 
            * eventDeltaBeats_param);
    if (_tmp >= SYNTH_CONTROL_EVENTDELTA_QUANT_TABLE_LENGTH) {
        _tmp = SYNTH_CONTROL_EVENTDELTA_QUANT_TABLE_LENGTH - 1;
    }
    if (_tmp < 0) {
        _tmp = 0;
    }
    noteParamSets[note_params_idx].eventDeltaBeats
        = eventDelta_quant_table[_tmp];
}

void synth_control_set_eventDelta_quant_curParams(float eventDeltaBeats_param)
{
    synth_control_set_eventDelta_quant(eventDeltaBeats_param,
            synth_control_get_editingWhichParams());
}

void synth_control_set_eventDelta_free(float eventDeltaBeats_param, int note_params_idx)
{
    size_t idx = (size_t)(eventDeltaBeats_param * (float)TABLES_DELTA_TIME_FREE_LEN);
    if (idx >= TABLES_DELTA_TIME_FREE_LEN) {
        idx = TABLES_DELTA_TIME_FREE_LEN - 1;
    }
    noteParamSets[note_params_idx].eventDeltaBeats = tables_delta_time_free[idx];
}

void synth_control_set_eventDelta_free_curParams(float eventDeltaBeats_param)
{
    synth_control_set_eventDelta_free(eventDeltaBeats_param,
            synth_control_get_editingWhichParams());
}

void synth_control_reset_noteOnEventCounts(void)
{
    uint32_t _idx;
    for (_idx = 0; _idx < NUM_NOTE_PARAM_SETS; _idx++) {
        noteOnEventCount[_idx] = 0;
    }
}

void synth_control_set_intermittency_idx(unsigned int idx, int note_params_idx)
{
    uint32_t _idx = idx;
    if (_idx >= SYNTH_CONTROL_INTERMITTENCY_TABLE_LENGTH) {
        _idx = SYNTH_CONTROL_INTERMITTENCY_TABLE_LENGTH - 1;
    }
    if (_idx < 0) {
        _idx = 0;
    }
    noteParamSets[note_params_idx].intermittency = intermittency_table[_idx];
    /* Reset all events' event count so that all sequences has same phase, no
     * matter when intermittency was set */
    synth_control_reset_noteOnEventCounts();
}

void synth_control_set_intermittency(float intermittency_param, int note_params_idx)
{
    uint32_t _idx = (uint32_t)((float)SYNTH_CONTROL_INTERMITTENCY_TABLE_LENGTH 
            * intermittency_param);
    synth_control_set_intermittency_idx(_idx,note_params_idx);
}

void synth_control_set_intermittency_curParams(float intermittency_param)
{
    synth_control_set_intermittency(intermittency_param,
            synth_control_get_editingWhichParams());
}

void synth_control_eventDeltaBeats_control(void *data_, float eventDeltaBeats_param)
{
    switch (deltaButtonMode) {
        case SynthControlDeltaButtonMode_EVENT_DELTA_QUANT:
            synth_control_set_eventDelta_quant_curParams(
                    eventDeltaBeats_param);
        case SynthControlDeltaButtonMode_EVENT_DELTA_FREE:
            synth_control_set_eventDelta_free_curParams(
                    eventDeltaBeats_param);
            break;
        case SynthControlDeltaButtonMode_INTERMITTENCY:
            synth_control_set_intermittency_curParams(
                    eventDeltaBeats_param);
            break;
    }
}

void synth_control_set_offset(float offset_param, int note_params_idx)
{
    noteParamSets[note_params_idx].offsetBeats
        = offset_param;
}

void synth_control_set_offset_curParams(float offset_param)
{
    synth_control_set_offset(offset_param,
            synth_control_get_editingWhichParams());
}

void synth_control_noteDeltaFromBuffer_control(void *data_,
        uint32_t noteDeltaFromBuffer_param)
{
    noteDeltaFromBuffer = (int)noteDeltaFromBuffer_param;
}

/* Called when you want to turn recording off, but not switch buffers, do
 * windowing, etc. This is done when auto record is turned off so that the last
 * completed recording can still be used. */
void synth_control_autoRecord_stop_helper(void)
{
    /* No more record scheduling */
    scheduleRecording = 0;
    /* Just stop the recorder, that's it. */
    wtr.state = MMWavTabRecorderState_STOPPED;
}

/* If the origin is USER, the tempo is adjusted in the REC_LEN_1_BEAT and
 * REC_LEN_1_BEAT_REC_SCHED modes, otherwise it is not. */
void synth_control_record_stop_helper(scrsh_source_t origin)
{
    /* Only do something if it was recording */
    if (wtr.state == MMWavTabRecorderState_STOPPED) {
        return;
    }
    /* Set the length to the index the recorder got to */
    ((MMArray*)wtr.buffer)->length =
        wtr.currentIndex;
    wtr.state = MMWavTabRecorderState_STOPPED;
    /* Don't window, we want to see the buffer filled only with ones. */
#if !defined(SIG_CHAIN_FILL_BUF_ONES)
    /* If the index the recorder got to is greater than the window length of a
     * Hann window, window the beginning and ends of the file using this window
     * */
    if (wtr.currentIndex >= hannWindowTableLength) {
        int n;
        for (n = 0; n < hannWindowTableLength/2; n++) {
            MMWavTab_get(wtr.buffer,n) *= hannWindowTable[n];
            MMWavTab_get(wtr.buffer,
                    ((MMArray*)wtr.buffer)->length - n - 1)
                *= hannWindowTable[hannWindowTableLength - n - 1];
        }
    }
#endif
    /* If the noteDeltaFromBuffer flag is set, compute the tempo from
     * the buffer length, set the playback rate to 1 and set the eventDelta
     * to 1 beat so that the recording plays once per beat and the tempo is
     * one beat per length of recording */
    SynthControlRecMode _recMode = synth_control_get_recMode();
    if ((_recMode == SynthControlRecMode_REC_LEN_1_BEAT)
            || (_recMode == SynthControlRecMode_REC_LEN_1_BEAT_REC_SCHED)) {
        /* When REC_LEN_1_BEAT* flag set
         *  - the tempo is adjusted so the recording plays in the time of one
         *    beat.
         *
         * If REC_LEN_1_BEAT* flag is set and the feedback is enabled,
         * then:
         *  - the sustain time is made to be 1
         *  - the the amplitudes of notes that aren't the 0th are set to 0
         *  - the pitch of note 0 is set to unison (60 so that rate is 1)
         *  - the delta time of note 0 is set to 1 beat
         *  - the intermittence of note 0 is set to 0
         *  - the offset of note 0 is set to 0
         *
         */
        if (origin == scrsh_source_USER) {
            /* Only adjust the tempo if record stop was called by user */
            float _tmp;
            if ((MMSample)((MMArray*)wtr.buffer)->length <= 0) {
                _tmp = SYNTH_CONTROL_ABS_MAX_TEMPO_BPM;
            } else if ((MMSample)((MMArray*)wtr.buffer)->length >
                        (SAMPLE_TABLE_LENGTH_SEC*((float)audio_hw_get_sample_rate(NULL))))
                _tmp = SYNTH_CONTROL_ABS_MIN_TEMPO_BPM;
            else {
                _tmp = (60. * (MMSample)audio_hw_get_sample_rate(NULL) 
                    / (MMSample)((MMArray*)wtr.buffer)->length);
            }
            synth_control_set_tempoBPM_absolute(_tmp);
        }
        if (feedbackState == 1) {
            int n;
            noteParamSets[0].pitches[0] = SYNTH_CONTROL_DEFAULT_PITCH;
            noteParamSets[0].fine_pitches[0] = SYNTH_CONTROL_DEFAULT_FINEPITCH;
            noteParamSets[0].rate_busses[0] = SYNTH_CONTROL_DEFAULT_RATEBUSRATE;
            noteParamSets[0].amplitude = 1.;
            for (n = 1; n < NUM_NOTE_PARAM_SETS; n++) {
                noteParamSets[n].amplitude = 0;
            }
            noteParamSets[0].sustainTime = 1.;
            noteParamSets[0].intermittency = 0;
            noteParamSets[0].offsetBeats = 0;
            noteParamSets[0].startPoint = 0;
            noteParamSets[0].attackTime = 0;
            noteParamSets[0].releaseTime = 0;
        }
    }
    /* Swap the playing and the recording sounds */
    /* Advance the playing and recording sounds along the ring */
    theSound = theSound->next;
    recordingSound = recordingSound->next;
}

void synth_control_record_start_helper(void)
{
    /* Only allow recording if wavtable is not being played */
    if (recordingSound->wavtab->n_players != 0) {
        return;
    }
    /* Set to max length so it would be possible to record all the way to
     * the end of allocated space */
    wtr.maxLength = soundSampleMaxLength;
    /* Set the area it is pointing to to the beginning of the allocated space */
    ((MMArray*)recordingSound->wavtab)->data = recordingSound->area;
    wtr.buffer = recordingSound->wavtab;
    wtr.currentIndex = 0;
    wtr.state = MMWavTabRecorderState_RECORDING;
}

void synth_control_record_stop(void)
{
    /* Indicate a recording was made */
    recording_exists = 1;
    if (scheduleRecording > 0) {
        /* If recordings are being scheduled, when the user requests the
         * recording to stop, we just stop recording but do not switch buffers
         * or adjust the tempo. */
        scheduleRecording = 0;
        synth_control_autoRecord_stop_helper();
    } else {
        SynthControlRecMode _recMode = synth_control_get_recMode();
        /* Stop playback if R=B or AREC */
        if ((_recMode == SynthControlRecMode_REC_LEN_1_BEAT) ||
            (_recMode == SynthControlRecMode_REC_LEN_1_BEAT_REC_SCHED)) {
            if (schedulerState == 1) {
                schedulerState_off_helper((void*)noteOnEventListHead);
            }
        }
        if (_recMode == SynthControlRecMode_REC_LEN_1_BEAT_REC_SCHED) {
            /* Recording will be stopped by event in scheduler */
            scheduleRecording = 2;
        } else {
            /* Stop recording if not in AREC mode */
            synth_control_record_stop_helper(scrsh_source_USER);
        }
        /* Turn back on scheduling if in R=B or AREC mode, otherwise scheduling
         * was never stopped. */
        if ((_recMode == SynthControlRecMode_REC_LEN_1_BEAT)
                || (_recMode == SynthControlRecMode_REC_LEN_1_BEAT_REC_SCHED)) {
            schedulerState_on_helper();
        }
    }
}

void synth_control_record_start(void)
{
#ifdef DEBUG
       assert(scheduleRecording == 0);
#endif  
       SynthControlRecMode _recMode =
           synth_control_get_recMode();
       if ((_recMode == SynthControlRecMode_REC_LEN_1_BEAT_REC_SCHED)
              && (schedulerState == 1)) {
           /* Turn back on record scheduling. Note that this will make record
            * scheduling happen when the scheduler is turned back on, if it is
            * not already on. */
           if (scheduleRecording == 0) {
               /* Set to 1 (and not 2) because we do not want tempo to be
                * adjusted */
               scheduleRecording = 1;
           }
       } else {
           synth_control_record_start_helper();
       }
}

/* If record switch pressed, recording on-going and record scheduling off, turn
 * off the current recording and use the "record stop helper".
 * If record switch pressed, recording on-going but record scheduling on,
 * re-start the recording by calling record start using the "record start
 * helper".
 * If record switch pressed, recording not on-going and recording in any mode,
 * start recording. If record scheduling is on in this case, there is an error.
 * */
void synth_control_record_tog(void)
{
    if (wtr.state == MMWavTabRecorderState_RECORDING) {
        synth_control_record_stop();
    } else if (wtr.state == MMWavTabRecorderState_STOPPED) {
        synth_control_record_start();
    }
}

void synth_control_feedback_control(uint32_t feedback_param)
{
    if (feedback_param > 0) {
        if ((feedbackState == 0)) {
            /* Move fbBusSplitter to onNode */
            MMSigProc_insertAfter(fbOnNode,&fbBusSplitter);
            feedbackState = 1;
        }
    } else {
        if (feedbackState == 1) {
            /* Move fbBusSplitter to offNode */
            MMSigProc_remove(&fbBusSplitter);
            /* Zero the feedback bus */
            memset(fbBusSplitter.destBus->data,0,
                    sizeof(MMSample)
                    * fbBusSplitter.destBus->size
                    * fbBusSplitter.destBus->channels);
            feedbackState = 0;
        }
    }
}

void synth_control_feedback_tog(void)
{
    if (feedbackState) {
        synth_control_feedback_control(0);
    } else {
        synth_control_feedback_control(1);
    }
}

/* Calling this resets the gains of NOTE 2 and 3 to default (most likely 0, see
 * the synth_control.h header). */
static void synth_control_reset_aux_note_gains(void)
{
    int n;
    for (n = 1; n < NUM_NOTE_PARAM_SETS; n++) {
        noteParamSets[n].amplitude = SYNTH_CONTROL_DEFAULT_AMPLITUDE_AUXNOTE;
    }
    noteParamSets[0].amplitude = SYNTH_CONTROL_DEFAULT_AMPLITUDE;
}

/* Calling this sets stride to 0 (no position advancement) */
static void synth_control_reset_pos_stride(void)
{
    /* Reset note stride accumulator. */
    synth_control_reset_noteStrideAcc();
    synth_control_set_positionStride(0.5,
            synth_control_get_editingWhichParams());
    synth_control_set_noteStride(0.5,
            synth_control_get_editingWhichParams());
}

void
synth_control_expr_ctl_chosen_set(void)
{
    expr_ctl_chosen = 1;
}    

void
synth_control_expr_ctl_chosen_reset(void)
{
    expr_ctl_chosen = 0;
}    

int
synth_control_expr_ctl_chosen(void)
{
    return expr_ctl_chosen;
}

static void synth_control_fbk_reset_cb(void* data)
{
    int *_num_fbk_presses = (int*)data,
        press_cnt = *_num_fbk_presses;
    if (press_cnt == 1) {
        synth_control_reset_pos_stride();
    } else if (press_cnt == 2) {
        synth_control_reset_aux_note_gains();
    } else if (press_cnt == 3) {
        synth_control_reset_aux_note_all_params();
    } 
    *_num_fbk_presses = 0;
}

static void synth_control_fbk_tog_setup(void)
{
    static uni_stuff_t fbk_uni_stuff;
    fbk_uni_stuff.next = uni_reset_items_head;
    fbk_uni_stuff.reset_cb = synth_control_fbk_reset_cb;
    fbk_uni_stuff.data = &num_fbk_presses;
    uni_reset_items_head = &fbk_uni_stuff;
}

/* Counts number of times fbk was pressed while in UNI mode. If first time,
 * just resets gain of note 2 and 3. If second time, resets all parameters
 * of note 2 and 3. */
void synth_control_fbk_tog(void)
{
    SynthControlPosMode pm;
    pm = synth_control_get_posMode_curParams();
    if (!synth_control_expr_ctl_chosen()) {
        if (pm == SynthControlPosMode_UNI){
            num_fbk_presses++;
            synth_control_set_uni_stuff_changed();
        } else {
            /* this also checks to see if enabling feedback is allowed */
            synth_control_feedback_tog();
        }
    }
    synth_control_expr_ctl_chosen_reset();
    /* Otherwise a knob was moved to choose what the expression
       pedal controls, so we ignore the toggle functionality of the fbk
       switch. */
}

static void free_playing_spsp_voice(void *voice_number)
{
    MMEnvelope_startRelease(
            ((MMEnvedSamplePlayer*)&spsps[*((int*)voice_number)])->envelope);
}

/* Pass a pointer to the first NoteOnEventListNode */
static void schedulerState_off_helper(void *data)
{
    int n;
    for (n = 0; n < NUM_NOTE_PARAM_SETS; n++) {
        /* Disactivate all events of all parameter sets */
        set_noteOnEvents_inactive(
                (NoteOnEventListNode*)MMDLList_getNext(
                    &(((NoteOnEventListNode*)data)[n])));
        /* Reset the note on event counts */
        noteOnEventCount[n] = 0;
    }
    /* Disactivate the noteSchedEvents */
    set_noteSchedEvents_inactive(
            (NoteSchedEventListNode*)MMDLList_getNext(&noteSchedEventListHead));
    /* Disactivate measure LED off events */
    set_measureLEDOffEvents_inactive(
            (MeasureLEDOffEventListNode*)MMDLList_getNext(&measureLEDOffEventListHead));
    /* Turn off LED */
    MEASURE_LED_RESET();
    /* Turn off all playing notes */
    pm_do_for_each_busy_voice(&voiceAllocator,free_playing_spsp_voice);
    schedulerState = 0;
}

static void schedulerState_on_helper(void)
{
    /* schedule 1st event which is initially active */
    if (noteSched_scheduling_helper( NoteSchedEvent_new(1))) {
        schedulerState = 1;
    }
}

void synth_control_schedulerState_control(void *data_, uint32_t schedulerState_param)
{
    if (schedulerState_param > 0) {
        schedulerState_on_helper();
    } else {
        schedulerState_off_helper(&noteOnEventListHead);
    }
}

void synth_control_schedulerState_on(void)
{
    schedulerState_on_helper();
}

void synth_control_schedulerState_off(void)
{
    /* If record scheduling on, it is turned off when the scheduler is
     * turned off. */
    if (scheduleRecording > 0) {
        synth_control_autoRecord_stop_helper();
    }
    schedulerState_off_helper(&noteOnEventListHead);
}

void synth_control_schedulerState_tog(void)
{
    SynthControlPosMode pm;
    pm = synth_control_get_posMode_curParams();
    if (pm == SynthControlPosMode_UNI){
        scheduler_advance_mode_cycle();
    } else {
        if (schedulerState) {
            synth_control_schedulerState_off();
        } else {
            synth_control_schedulerState_on();
        }
    }
}

void synth_control_editingWhichParams_control(void *data_,
        uint32_t editingWhichParams_param)
{
    if (editingWhichParams_param >= NUM_NOTE_PARAM_SETS) {
            editingWhichParams_param = NUM_NOTE_PARAM_SETS - 1;
    } 
    editingWhichParams = editingWhichParams_param;
}

void synth_control_deltaButtonMode_control(void *data_,
        uint32_t deltaButtonMode_param)
{
    deltaButtonMode =
        (SynthControlDeltaButtonMode)deltaButtonMode_param;
}

void synth_control_gainMode_control(void *data_,
        uint32_t gainMode_param)
{
    gainMode = (SynthControlGainMode)gainMode_param;
}

void synth_control_set_wet(float gain_param, int note_params_idx)
{
    /* Gain in dB */
    gain_param = SYNTH_CONTROL_MIN_GAIN 
        + (SYNTH_CONTROL_MAX_GAIN - SYNTH_CONTROL_MIN_GAIN)*gain_param;
    if (gain_param < SYNTH_CONTROL_GAIN_THRESH) {
        noteParamSets[note_params_idx].amplitude = 0.;
    } else {
        noteParamSets[note_params_idx].amplitude =
            powf(10.,gain_param / 20.);
    }
}

void synth_control_set_wet_curParams(float gain_param)
{
    synth_control_set_wet(gain_param,
            synth_control_get_editingWhichParams());
}

void synth_control_set_fade(float gain_param, int num_repeats, int note_params_idx)
{
    /* This sets the amplitude scaling at the last repeat */
    gain_param = powf(gain_param,1./((float)(num_repeats+1)));
    noteParamSets[note_params_idx].fadeRate = gain_param;
}

void synth_control_set_fade_curParams(float gain_param, int num_repeats)
{
    synth_control_set_fade(gain_param,
            num_repeats,
            synth_control_get_editingWhichParams());
}

void synth_control_set_fade_rate(float rate, int note_params_idx)
{
    noteParamSets[note_params_idx].fadeRate = rate;
}

static void
update_fade_rates(int note_params_idx)
{
    int numRepeats = noteParamSets[note_params_idx].numRepeats;
    float amp_last_echo = noteParamSets[note_params_idx].ampLastEcho,
          fade_rate = numRepeats == 0 ? 1 : powf(amp_last_echo,1./numRepeats),
          initial_fade = fade_rate <= 1 ? 1. : 1./amp_last_echo;
    noteParamSets[note_params_idx].initialFade = initial_fade;
    noteParamSets[note_params_idx].fadeRate = fade_rate;
}
    

void synth_control_set_ampLastEcho(float gain_param, int note_params_idx)
{
    float amp_last_echo = powf(10,(gain_param*100.f - 60.f)/20.f);
    noteParamSets[note_params_idx].ampLastEcho = amp_last_echo;
    update_fade_rates(note_params_idx);
}

void synth_control_set_ampLastEcho_curParams(float gain_param)
{
    synth_control_set_ampLastEcho(gain_param,
            synth_control_get_editingWhichParams());
}

void synth_control_gain_control(void *data_, float gain_param)
{
    switch (gainMode) {
        case SynthControlGainMode_WET:
            synth_control_set_wet_curParams(gain_param);
            break;
        case SynthControlGainMode_FADE:
            synth_control_set_ampLastEcho_curParams(gain_param);
            break;
        default:
            break;
    }
}

void synth_control_presetNumber_control(void *data_, uint32_t presetNumber_param)
{
    if (presetNumber_param >= NUM_SYNTH_CONTROL_PRESETS) {
        presetNumber_param = NUM_SYNTH_CONTROL_PRESETS - 1;
    }
    presetNumber = (int)(presetNumber_param);
}

void synth_control_presetStore_control(void *data_, uint32_t presetStore_param)
{
    sc_presets_store(presetNumber);
}

void synth_control_presetRecall_control(void *data_, uint32_t presetRecall_param)
{
    sc_presets_recall(presetNumber);
}

void synth_control_presetStore_tog(void)
{
    sc_presets_store(presetNumber);
}

void synth_control_presetRecall_tog(void)
{
    sc_presets_recall(presetNumber);
}

void synth_control_reset_param_sets(NoteParamSet *param_sets, int size)
{

    param_sets[0].attackTime = SYNTH_CONTROL_DEFAULT_ATTACKTIME;
    param_sets[0].sustainTime  = SYNTH_CONTROL_DEFAULT_SUSTAINTIME; 
    param_sets[0].releaseTime = SYNTH_CONTROL_DEFAULT_RELEASETIME; 
    param_sets[0].eventDeltaBeats = SYNTH_CONTROL_DEFAULT_EVENTDELTABEATS;   
    param_sets[0].amplitude = SYNTH_CONTROL_DEFAULT_AMPLITUDE;
    param_sets[0].startPoint = SYNTH_CONTROL_DEFAULT_STARTPOINT;
    param_sets[0].numRepeats = SYNTH_CONTROL_DEFAULT_NUMREPEATS;
    param_sets[0].offsetBeats = SYNTH_CONTROL_DEFAULT_OFFSETBEATS;
    param_sets[0].intermittency = SYNTH_CONTROL_DEFAULT_INTERMITTENCY;
    param_sets[0].fadeRate      = SYNTH_CONTROL_DEFAULT_FADERATE;
    param_sets[0].initialFade      = SYNTH_CONTROL_DEFAULT_INITIALFADE;
    param_sets[0].ampLastEcho   = SYNTH_CONTROL_DEFAULT_AMPLASTECHO;
    param_sets[0].positionStride = SYNTH_CONTROL_DEFAULT_POSITIONSTRIDE;
    param_sets[0].noteStride = SYNTH_CONTROL_DEFAULT_NOTESTRIDE;
    param_sets[0].noteStrideAcc = SYNTH_CONTROL_DEFAULT_NOTESTRIDEACC;
    uint32_t _n;
    for (_n = 0; _n < SYNTH_CONTROL_PITCH_TABLE_SIZE; _n++) {
        param_sets[0].pitches[_n] = SYNTH_CONTROL_DEFAULT_PITCH;
        param_sets[0].fine_pitches[_n] = SYNTH_CONTROL_DEFAULT_FINEPITCH;
        param_sets[0].rate_busses[_n] = SYNTH_CONTROL_DEFAULT_RATEBUSRATE;
    }
    for (_n = 0 ; _n < SYNTH_CONTROL_SWING_TABLE_SIZE; _n++) {
        param_sets[0].swing[_n] = SYNTH_CONTROL_DEFAULT_SWING;
    }
    while (size-- > 1) {
        param_sets[size].attackTime = SYNTH_CONTROL_DEFAULT_ATTACKTIME;     
        param_sets[size].sustainTime  = SYNTH_CONTROL_DEFAULT_SUSTAINTIME; 
        param_sets[size].releaseTime = SYNTH_CONTROL_DEFAULT_RELEASETIME; 
        param_sets[size].eventDeltaBeats = SYNTH_CONTROL_DEFAULT_EVENTDELTABEATS;   
        param_sets[size].amplitude = 0;
        param_sets[size].startPoint = SYNTH_CONTROL_DEFAULT_STARTPOINT;
        param_sets[size].numRepeats = SYNTH_CONTROL_DEFAULT_NUMREPEATS;
        param_sets[size].offsetBeats = SYNTH_CONTROL_DEFAULT_OFFSETBEATS;
        param_sets[size].intermittency = SYNTH_CONTROL_DEFAULT_INTERMITTENCY;
        param_sets[size].fadeRate      = SYNTH_CONTROL_DEFAULT_FADERATE_AUXNOTE;
        param_sets[size].initialFade      = SYNTH_CONTROL_DEFAULT_INITIALFADE;
        param_sets[size].ampLastEcho   = SYNTH_CONTROL_DEFAULT_AMPLASTECHO_AUXNOTE;
        param_sets[size].positionStride = SYNTH_CONTROL_DEFAULT_POSITIONSTRIDE;
        param_sets[size].noteStride = SYNTH_CONTROL_DEFAULT_NOTESTRIDE;
        param_sets[size].noteStrideAcc = SYNTH_CONTROL_DEFAULT_NOTESTRIDEACC;
        for (_n = 0; _n < SYNTH_CONTROL_PITCH_TABLE_SIZE; _n++) {
            param_sets[size].pitches[_n] = SYNTH_CONTROL_DEFAULT_PITCH;
            param_sets[size].fine_pitches[_n] = SYNTH_CONTROL_DEFAULT_FINEPITCH;
            param_sets[size].rate_busses[_n] = SYNTH_CONTROL_DEFAULT_RATEBUSRATE;
        }
        for (_n = 0 ; _n < SYNTH_CONTROL_SWING_TABLE_SIZE; _n++) {
            param_sets[size].swing[_n] = SYNTH_CONTROL_DEFAULT_SWING;
        }
    };
}

static void synth_control_reset_aux_note_all_params(void)
{
    synth_control_reset_param_sets(noteParamSets,NUM_NOTE_PARAM_SETS);
}

void synth_control_reset_global_params(void)
{
    noteDeltaFromBuffer = 0;
    editingWhichParams  = 0;
    tempoBPM     = SYNTH_CONTROL_DEFAULT_TEMPOBPM_COARSE;
    tempoBPM_scale      = SYNTH_CONTROL_DEFAULT_TEMPOBPM_SCALE;
    tempoBPM            = SYNTH_CONTROL_DEFAULT_TEMPOBPM;
    deltaButtonMode     = SynthControlDeltaButtonMode_EVENT_DELTA_FREE;
    recMode             = SynthControlRecMode_NORMAL;
    gainMode            = SynthControlGainMode_WET;
    feedbackState       = 0;
    scheduleRecording   = 0;
    schedulerState      = 0;
    editing_which_pitch = 0;
    pitchMode           = SYNTH_CONTROL_DEFAULT_PITCHMODE;
}

void synth_control_setup(void)
{
    synth_control_reset_param_sets(noteParamSets,NUM_NOTE_PARAM_SETS);
    synth_control_reset_global_params();
    HannWindowTable_init(REC_LOOP_FADE_TIME_S * 2.);
    synth_control_fbk_tog_setup();
}

SynthControlEditingWhichParamsIndex synth_control_get_editingWhichParams(void)
{
    return editingWhichParams;
}

void synth_control_set_editingWhichParams(
        SynthControlEditingWhichParamsIndex editingWhichParams_param)
{
    if (editingWhichParams_param >= NUM_NOTE_PARAM_SETS) {
            editingWhichParams_param = NUM_NOTE_PARAM_SETS - 1;
    } 
    editingWhichParams = editingWhichParams_param;
}

void synth_control_set_deltaButtonMode(SynthControlDeltaButtonMode 
        deltaButtonMode_param)
{
    deltaButtonMode = deltaButtonMode_param;
}

SynthControlDeltaButtonMode synth_control_get_deltaButtonMode(void)
{
    return deltaButtonMode;
}

void synth_control_set_recMode(SynthControlRecMode recMode_param)
{
    if (recMode_param < SynthControlRecMode_START__) {
        recMode_param = SynthControlRecMode_START__;
    }
    if (recMode_param > SynthControlRecMode_END__) {
        recMode_param = SynthControlRecMode_END__;
    }
    SynthControlRecMode _recMode = synth_control_get_recMode();
    if (_recMode != recMode_param) {
        recMode = recMode_param;
        /* This effectively means we're leaving 
         * SynthControlRecMode_REC_LEN_1_BEAT_REC_SCHED mode and when we do this
         * we turn off auto record. */
        if (recMode != SynthControlRecMode_REC_LEN_1_BEAT_REC_SCHED) {
            synth_control_autoRecord_stop_helper();
        }
    }
}

/* Only set if mode being set different from last mode set.
 * Second argument NULL pointer sets regardless. */
void synth_control_set_recMode_onChange(SynthControlRecMode recMode_param,
                               SynthControlRecMode *last_recMode_param)
{
    /* Only do the following if recMode_param different from last recMode_param
     * that this function was called with. */
    if (last_recMode_param) {
        if (recMode_param == *last_recMode_param) {
            return;
        }
        *last_recMode_param = recMode_param;
    }
    synth_control_set_recMode(recMode_param);
}

SynthControlRecMode synth_control_get_recMode(void)
{
    return recMode;
}

void synth_control_set_posMode(SynthControlPosMode posMode_param)
{
    posMode = (SynthControlPosMode)posMode_param;
}

/*
Only set if mode being set different from last mode set.
Second argument NULL pointer sets regardless. 
This style of setting was created when MIDI could also control the posMode so
that setting a posMode different from the position of the physical switch would
be possible (otherwise the next time the switches were checked, the posMode of
the physical switch would be forced, making it impossible to choose posMode with
MIDI). But now it is not possible to choose posMode with MIDI, so this style is
deprecated, but kept because it works.
*/
void synth_control_set_posMode_onChange(SynthControlPosMode posMode_param,
                                        SynthControlPosMode *last_posMode_param)
{
    if (last_posMode_param) {
        if (*last_posMode_param == posMode_param) {
            return;
        }
        *last_posMode_param = posMode_param;
    }
    synth_control_set_posMode(posMode_param);
}

/* Only set if mode being set different from last mode set */
void synth_control_set_posMode_onChange_curParams(SynthControlPosMode posMode_param,
                                        SynthControlPosMode *last_posMode_param)
{
    synth_control_set_posMode_onChange(posMode_param,
                                       last_posMode_param);
}

SynthControlPosMode synth_control_get_posMode_curParams(void)
{
    return posMode;
}

void synth_control_set_gainMode(SynthControlGainMode gainMode_param)
{
    gainMode = (SynthControlGainMode)gainMode_param;
}

SynthControlGainMode synth_control_get_gainMode(void)
{
    return gainMode;
}

void synth_control_set_presetNumber(SynthControlPresetNumber presetNumber_param)
{
    if (presetNumber_param >= NUM_SYNTH_CONTROL_PRESETS) {
        presetNumber_param = NUM_SYNTH_CONTROL_PRESETS - 1;
    }
    presetNumber = presetNumber_param;
}

SynthControlPresetNumber synth_control_get_presetNumber(void)
{
    return presetNumber;
}

int synth_control_get_noteDeltaFromBuffer(void)
{
    return noteDeltaFromBuffer;
}

/* Returns 1 if currently recording, 0 otherwise */
int synth_control_get_recordState(void)
{
    switch (wtr.state) {
        case MMWavTabRecorderState_RECORDING:
            return 1;
    }
    return 0;
}

/* Returns 1 if scheduler play, 0 otherwise */
int synth_control_get_schedulerState(void)
{
    return schedulerState;
}

/* Returns 1 if feedback enable, 0 otherwise */
int synth_control_get_feedbackState(void)
{
    return feedbackState;
}

/*
This is the tempo after scaling. This tempo represents how often the beat
light flashes.
*/
float synth_control_get_tempoBPM(void)
{
    return tempoBPM;
}

float synth_control_get_tempoBPM_scale(void)
{
    return tempoBPM_scale;
}

void synth_control_set_editing_which_pitch(int _param)
{
    editing_which_pitch = _param;
}

int synth_control_get_editing_which_pitch(void)
{
    return editing_which_pitch;
}

int synth_control_get_uni_stuff_changed(void)
{
    return uni_stuff_changed;
}

void synth_control_set_uni_stuff_changed(void)
{
    uni_stuff_changed = 1;
}

/* Sets uni_stuff_changed flag to zero. Will also called registered callbacks
 * (so that things that triggered its change can also be reset). */
void synth_control_reset_uni_stuff_changed(void)
{
    uni_stuff_changed = 0;
    uni_stuff_t *ptr;
    ptr = uni_reset_items_head;
    while (ptr) {
        ptr->reset_cb(ptr->data);
        ptr = ptr->next;
    }
}

/* This checks to see if stuff changed while the PosMode was
 * SynthControlPosMode_UNI. If it was, it will not reset the pitches.
 * Otherwise it will. This means that it only works with a configuration where
 * it is called when the switch returns to the rest position (not when it goes
 * to the active position). */
void synth_control_pitch_reset_tog(void)
{
    uint32_t _m, _n;
    if (synth_control_get_uni_stuff_changed()) {
        synth_control_reset_uni_stuff_changed();
    } else {
        _n = synth_control_get_editingWhichParams();
        for (_m = 0; _m < SYNTH_CONTROL_PITCH_TABLE_SIZE; _m++) {
            noteParamSets[_n].pitches[_m] = SYNTH_CONTROL_DEFAULT_PITCH;
            noteParamSets[_n].fine_pitches[_m] = SYNTH_CONTROL_DEFAULT_FINEPITCH;
            noteParamSets[_n].rate_busses[_m] = SYNTH_CONTROL_DEFAULT_RATEBUSRATE;
        }
    }
}

void synth_control_one_shot(MMSample pitch,
                            MMSample amplitude)
{
    /* schedule 1st event which is initially active */
    NoteSchedEvent *nse;
    nse = NoteSchedEvent_new(1);
    if (!nse) {
        return;
    }
    NoteSchedEvent_set_pitch_offset(nse,pitch);
//    NoteSchedEvent_set_pitch_mode(nse,SynthControlPitchMode_ABSOLUTE);
    NoteSchedEvent_set_pitch_mode(nse,SynthControlPitchMode_BUS);
    NoteSchedEvent_set_amplitude_scalar(nse,amplitude);
    NoteSchedEvent_set_one_shot(nse,1);
    noteSched_scheduling_helper(nse);
}

void synth_control_note_on(int parameterSet,
                           MMSample pitch,
                           MMSample amplitude)
{
    if (parameterSet < 0) {
        parameterSet = 0;
    }
    if (parameterSet >= NUM_NOTE_PARAM_SETS) {
        parameterSet = NUM_NOTE_PARAM_SETS - 1;
    }
    MMSample voiceNum = pm_get_next_free_voice_number();
    if (voiceNum != -1 && 
            (noteParamSets[parameterSet].amplitude > SCHEDULING_AMP_FLOOR)) { 
        /* there is a voice free */
        pm_claim_params_from_allocator((void*)&voiceAllocator,
                (void*)&voiceNum);
        ((MMEnvedSamplePlayer*)&spsps[(int)voiceNum])->onDone =
            autorelease_on_done;
        MMTrapEnvedSamplePlayer_noteOnStruct no;
        no.note = voiceNum;
        no.amplitude = amplitude;
        no.index = noteParamSets[parameterSet].startPoint
            * MMArray_get_length(theSound->wavtab);
        /* sustainTime is the length of the audio, times
         * noteParamSets[parameterSet].sustainTime *
         * length_of_sound_seconds * (1 -
         * noteParamSets[parameterSet].attackTime -
         * noteParamSets[parametersSet].releaseTime) */
        no.sustainTime =
            noteParamSets[parameterSet].sustainTime
            * (MMSample)MMArray_get_length(theSound->wavtab)
            / (MMSample)audio_hw_get_sample_rate(NULL)
            * (1. - noteParamSets[parameterSet].attackTime
                  - noteParamSets[parameterSet].releaseTime);
#ifdef SIG_CHAIN_FILL_BUF_ONES
        no.attackTime = 0;
        no.releaseTime = 0;
#else
        no.attackTime = 
            noteParamSets[parameterSet].sustainTime
            * (MMSample)MMArray_get_length(theSound->wavtab)
            / (MMSample)audio_hw_get_sample_rate(NULL)
            * noteParamSets[parameterSet].attackTime;
        no.releaseTime = 
            noteParamSets[parameterSet].sustainTime
            * (MMSample)MMArray_get_length(theSound->wavtab)
            / (MMSample)audio_hw_get_sample_rate(NULL)
            * noteParamSets[parameterSet].releaseTime;
#endif
        no.samples = theSound->wavtab;
        MMWavTab_inc_n_players(theSound->wavtab);
        /* 69 is added because MMCC_et12_rate considers pitch 69 to be a note of no
         * transposition. In this we consider 0 to be a note of no
         * transposition, so we add 69 */
        no.rate = MMCC_et12_rate(pitch + 69);
        MMTrapEnvedSamplePlayer_noteOn_Rate(
                &spsps[(int)voiceNum], &no);
    }
}

MMSample synth_control_clip_valid_pitch(MMSample pitch)
{
    if (pitch < SYNTH_CONTROL_PITCH_MIN)
    {
        pitch = SYNTH_CONTROL_PITCH_MIN;
    }
    if (pitch > SYNTH_CONTROL_PITCH_MAX) {
        pitch = SYNTH_CONTROL_PITCH_MAX;
    }
    return pitch;
}

void synth_control_set_swing(float param,
                             int idx)
{
    size_t n;
    n = (size_t)(param * TABLES_N_SWING_SETS);
    if (n >= TABLES_N_SWING_SETS) {
        n = TABLES_N_SWING_SETS - 1;
    }
    TABLES_SWING_PTS_LOOKUP(n,
                            &noteParamSets[idx].swing[0],
                            &noteParamSets[idx].swing[1]);
}

void synth_control_set_swing_curParams(float param)
{
    synth_control_set_swing(param,synth_control_get_editingWhichParams());
}
