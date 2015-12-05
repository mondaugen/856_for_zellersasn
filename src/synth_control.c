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

#ifdef DEBUG
 #include <assert.h>
#endif  

/* Keeps track of how many times a note has been played. If the number of times
 * is divisible by the note-parameter set's intermittency value, this count gets
 * reset */
int noteOnEventCount[NUM_NOTE_PARAM_SETS];

/* Stuff that could be saved */
NoteParamSet                noteParamSets[NUM_NOTE_PARAM_SETS];
/* The tempo, calculated as (tempoBPM_coarse + tempoBPM_fine) * tempoBPM_scale
 * */
static float                tempoBPM; 
static float                tempoBPM_coarse;
static float                tempoBPM_fine;
static float                tempoBPM_scale;
static const float          tempoBPM_scale_table[] = 
                                SYNTH_CONTROL_TEMPOBPM_SCALE_TABLE;
static const float          eventDelta_quant_table[] = 
                                SYNTH_CONTROL_EVENTDELTA_QUANT_TABLE;
static const int            intermittency_table[] =
                                SYNTH_CONTROL_INTERMITTENCY_TABLE;
SynthControlPosMode         posMode;
SynthControlDeltaButtonMode deltaButtonMode;
SynthControlGainMode        gainMode;
SynthControlRecMode         recMode;

/* Stuff that shouldn't really be saved */
int                         noteDeltaFromBuffer;
SynthControlEditingWhichParamsIndex editingWhichParams;
int                         currentPreset;
int                         feedbackState;
int                         scheduleRecording;
int                         firstScheduledRecording;
int                         editing_which_pitch;
/* Is the scheduler on or off ? */
int                         schedulerState;
/* What preset would be recalled/stored. First preset is numbered 0. */
SynthControlPresetNumber    presetNumber;

/* Stuff that might not make it into the final application */
int16_t                     dryGain;

static void schedulerState_off_helper(void *data);
static void schedulerState_on_helper(void);

void autorelease_on_done(MMEnvedSamplePlayer * esp)
{
    pm_yield_params_to_allocator((void*)&voiceAllocator,
            (void *)&(MMEnvedSamplePlayer_getSamplePlayerSigProc(esp).note));
}

void synth_control_set_envelopeTime(float envelopeTime_param)
{
    env_map_attack_release_f(
            &noteParamSets[editingWhichParams].attackTime,
            &noteParamSets[editingWhichParams].releaseTime,
            envelopeTime_param,
            SYNTH_CONTROL_MIN_ATTACK_TIME,
            SYNTH_CONTROL_MAX_ATTACK_TIME,
            SYNTH_CONTROL_MIN_RELEASE_TIME,
            SYNTH_CONTROL_MAX_RELEASE_TIME);
}

void synth_control_envelopeTime_control(void *data_, float envelopeTime_param)
{
    synth_control_set_envelopeTime(envelopeTime_param);
}

void synth_control_set_sustainTime(float sustainTime_param)
{
    /* Sustain time is relative to length of recording, so here just 0-1.
     * It is scaled this way so that the length selection is more precise for
     * short lengths and less precise for longer ones */
    noteParamSets[editingWhichParams].sustainTime
        = powf(2.,-7.*(1 - sustainTime_param));
}

void synth_control_sustainTime_control(void *data_, float sustainTime_param)
{
    synth_control_set_sustainTime(sustainTime_param);
}

void synth_control_tempoNudge(float tempoNudge_param)
{
    /* the tempo is calculated so that 1 buffer * K is played per 1 beat where K
     * is some scalar. The value is negated so if K < 1, the tempo is slower and
     * K > 1 the tempo is faster */
    MMSample K =  1.05 - tempoNudge_param * 0.1;
    synth_control_update_tempo_coarse( 60. * (MMSample)audio_hw_get_sample_rate(NULL) 
            / ((MMSample)((MMArray*)theSound.wavtab)->length * K));
}

static void synth_control_set_tempoBPM(float tempoBPM_param)
{
    tempoBPM = tempoBPM_param;
}

void synth_control_set_tempo(float _tempoBPM_coarse,
                             float _tempoBPM_fine,
                             float _tempoBPM_scale)
{
    synth_control_set_tempoBPM(
            (_tempoBPM_coarse + _tempoBPM_fine) / _tempoBPM_scale);
}

void synth_control_update_tempo(void)
{
    synth_control_set_tempo(synth_control_get_tempoBPM_coarse(),
                            synth_control_get_tempoBPM_fine(),
                            synth_control_get_tempoBPM_scale());
}

void synth_control_set_tempo_coarse(float _tempoBPM_coarse)
{
    tempoBPM_coarse = _tempoBPM_coarse;
}

void synth_control_set_tempo_fine(float _tempoBPM_fine)
{
    tempoBPM_fine = _tempoBPM_fine;
}

void synth_control_set_tempo_scale(float _tempoBPM_scale)
{
    tempoBPM_scale = _tempoBPM_scale;
}

void synth_control_update_tempo_coarse(float _tempoBPM_coarse)
{
    synth_control_set_tempo_coarse(_tempoBPM_coarse);
    synth_control_update_tempo();
}

void synth_control_update_tempo_fine(float _tempoBPM_fine)
{
    synth_control_set_tempo_fine(_tempoBPM_fine);
    synth_control_update_tempo();
}

void synth_control_update_tempo_scale(float _tempoBPM_scale)
{
    synth_control_set_tempo_scale(_tempoBPM_scale);
    synth_control_update_tempo();
}

/* sets tempo and resets fine and scaling controls */
void synth_control_set_tempoBPM_absolute(float _tempoBPM)
{
    synth_control_set_tempo_fine(SYNTH_CONTROL_DEFAULT_TEMPOBPM_FINE);
    synth_control_set_tempo_scale(SYNTH_CONTROL_DEFAULT_TEMPOBPM_SCALE);
    synth_control_update_tempo_coarse(_tempoBPM);
}

/* Param should be in [0,1] */
void synth_control_set_tempo_coarse_norm(float param)
{
    float _tmp;
    _tmp = floor(((SYNTH_CONTROL_TEMPOBPM_COARSE_MAX 
                    - SYNTH_CONTROL_TEMPOBPM_COARSE_MIN)
                / SYNTH_CONTROL_TEMPOBPM_COARSE_QUANT) 
            * param)
            * SYNTH_CONTROL_TEMPOBPM_COARSE_QUANT 
            + SYNTH_CONTROL_TEMPOBPM_COARSE_MIN;
    synth_control_update_tempo_coarse(_tmp);
}

/* Param should be in [0,1] */
void synth_control_set_tempo_fine_norm(float param)
{
    float _tmp;
    _tmp = floor(((SYNTH_CONTROL_TEMPOBPM_FINE_MAX 
                    - SYNTH_CONTROL_TEMPOBPM_FINE_MIN)
                / SYNTH_CONTROL_TEMPOBPM_FINE_QUANT) 
            * param)
            * SYNTH_CONTROL_TEMPOBPM_FINE_QUANT 
            + SYNTH_CONTROL_TEMPOBPM_FINE_MIN;
    synth_control_update_tempo_fine(_tmp);
}

/* Param should be in [0,1) */
void synth_control_set_tempo_scale_norm(float param)
{
    int _tmp = (int)floor(SYNTH_CONTROL_TEMPOBPM_SCALE_TABLE_LENGTH
                            * param);
    if (_tmp == SYNTH_CONTROL_TEMPOBPM_SCALE_TABLE_LENGTH) {
        _tmp--;
    }
    synth_control_update_tempo_scale(tempoBPM_scale_table[_tmp]);
}

void synth_control_set_numRepeats(int numRepeats_param)
{
    noteParamSets[editingWhichParams].numRepeats = numRepeats_param;
    synth_control_set_fade(noteParamSets[editingWhichParams].ampLastEcho,
                           noteParamSets[editingWhichParams].numRepeats);
}

void synth_control_set_repeats(float repeats_param)
{
    synth_control_set_numRepeats((int)(
                ((float)SYNTH_CONTROL_MAX_NUM_REPEATS) * repeats_param));        
}

/* Set the pitch to any pitch between midi note 48 and 72, limited by the
 * precision of pitch_param */
void synth_control_set_pitch_chrom(float pitch_param)
{
    noteParamSets[editingWhichParams].pitches[editing_which_pitch]
        = -12 + 24 * pitch_param;
}

/* Set pitch to a midi note. Rounds according to quantization parameters. */
void synth_control_set_pitch_chrom_quant(float param)
{
    float _tmp;
    _tmp = floor(((SYNTH_CONTROL_PITCH_CHROM_MAX 
                    - SYNTH_CONTROL_PITCH_CHROM_MIN)
                / SYNTH_CONTROL_PITCH_CHROM_QUANT) 
            * param)
            * SYNTH_CONTROL_PITCH_CHROM_QUANT 
            + SYNTH_CONTROL_PITCH_CHROM_MIN;
    noteParamSets[editingWhichParams].pitches[editing_which_pitch]
        = _tmp;
}

void synth_control_set_pitch_4ths5ths(float pitch_param)
{
    static int32_t ivals[] = {-19,-17,-12,-7,-5,0,5,7,12,17,19};
    /* Half the total number of intervals that aren't 0 */
    static uint32_t n_ivals = 5; 
    pitch_param = pitch_param * 2. - 1.;
    uint32_t idx = (uint32_t)((int32_t)n_ivals
            + (int32_t)((float)n_ivals*pitch_param));
    noteParamSets[editingWhichParams].pitches[editing_which_pitch]
        = (float)ivals[idx];
}

void synth_control_set_pitch_arp(float pitch_param)
{
    /* Not yet capable of supporting this, just set to middle C */
    noteParamSets[editingWhichParams].pitches[editing_which_pitch] = 0;
}

void synth_control_pitch_control(void *data_, float pitch_param)
{
    synth_control_set_pitch_chrom(pitch_param);
}

void synth_control_set_startPoint(float startPoint_param)
{
    noteParamSets[editingWhichParams].startPoint
        = startPoint_param;
}

void synth_control_set_positionStride(float positionStride_param)
{
            noteParamSets[editingWhichParams].positionStride
                = positionStride_param * 0.2 - 0.1;
}

void synth_control_startPoint_control(void *data_, float startPoint_param)
{
    switch (posMode) {
        case SynthControlPosMode_ABSOLUTE:
            synth_control_set_startPoint(startPoint_param);
            break;
        case SynthControlPosMode_UNKNOWN:
        case SynthControlPosMode_STRIDE:
            synth_control_set_positionStride(startPoint_param);
            break;
    }
}

void synth_control_set_eventDelta_quant(float eventDeltaBeats_param)
{
    int _tmp;
    _tmp = (int)floor(SYNTH_CONTROL_EVENTDELTA_QUANT_TABLE_LENGTH 
            * eventDeltaBeats_param);
    if (_tmp == SYNTH_CONTROL_EVENTDELTA_QUANT_TABLE_LENGTH) {
        _tmp--;
    }
    noteParamSets[editingWhichParams].eventDeltaBeats
        = eventDelta_quant_table[_tmp];

//        = (MMSample)(1 + (int)(3. * (MMSample)eventDeltaBeats_param));
}

void synth_control_set_eventDelta_free(float eventDeltaBeats_param)
{
    noteParamSets[editingWhichParams].eventDeltaBeats
        = powf(2.,-6.*(1 - eventDeltaBeats_param));
}

void synth_control_reset_noteOnEventCounts(void)
{
    uint32_t _idx;
    for (_idx = 0; _idx < NUM_NOTE_PARAM_SETS; _idx++) {
        noteOnEventCount[_idx] = 0;
    }
}

void synth_control_set_intermittency(float intermittency_param)
{
    uint32_t _idx = (uint32_t)((float)SYNTH_CONTROL_INTERMITTENCY_TABLE_LENGTH 
            * intermittency_param);
    noteParamSets[editingWhichParams].intermittency = intermittency_table[_idx];
    /* Reset all events' event count so that all sequences has same phase, no
     * matter when intermittency was set */
    synth_control_reset_noteOnEventCounts();
}

void synth_control_eventDeltaBeats_control(void *data_, float eventDeltaBeats_param)
{
    switch (deltaButtonMode) {
        case SynthControlDeltaButtonMode_EVENT_DELTA_QUANT:
            synth_control_set_eventDelta_quant(eventDeltaBeats_param);
        case SynthControlDeltaButtonMode_EVENT_DELTA_FREE:
            synth_control_set_eventDelta_free(eventDeltaBeats_param);
            break;
        case SynthControlDeltaButtonMode_INTERMITTENCY:
            synth_control_set_intermittency(eventDeltaBeats_param);
            break;
    }
}

void synth_control_set_offset(float offset_param)
{
    if (editingWhichParams == 0) {
        noteParamSets[editingWhichParams].offsetBeats
            = offset_param + 0.001;
    } else {
        /* The offset is relative to the total event delta of the note event
         * with parameterSet 0 */
        noteParamSets[editingWhichParams].offsetBeats
            = noteParamSets[0].eventDeltaBeats * offset_param;
    }
}

void synth_control_offsetBeats_control(void *data_, float offsetBeats_param)
{
    if (editingWhichParams == 0) {
        noteParamSets[editingWhichParams].offsetBeats
            = offsetBeats_param + 0.001;
    } else {
        /* The offset is relative to the total event delta of the note event
         * with parameterSet 0 */
        noteParamSets[editingWhichParams].offsetBeats
            = noteParamSets[0].eventDeltaBeats * offsetBeats_param;
    }
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

void synth_control_record_stop_helper(void)
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
        synth_control_set_tempoBPM_absolute(60. 
                * (MMSample)audio_hw_get_sample_rate(NULL) 
                / (MMSample)((MMArray*)wtr.buffer)->length);
        if (feedbackState == 1) {
            int n;
            noteParamSets[0].eventDeltaBeats = 1;
            noteParamSets[0].pitches[0] = 0.;
            noteParamSets[0].amplitude = 1.;
            for (n = 1; n < NUM_NOTE_PARAM_SETS; n++) {
                noteParamSets[n].amplitude = 0;
            }
            noteParamSets[0].sustainTime = 1.;
            noteParamSets[0].intermittency = 0;
            noteParamSets[0].offsetBeats = 0;
            noteParamSets[0].startPoint = 0;
        }
    }
    /* Swap the playing and the recording sounds */
    WavTabAreaPair tmp = recordingSound;
    recordingSound = theSound;
    theSound = tmp;
}

void synth_control_record_start_helper(void)
{
    /* Set to max length so it would be possible to record all the way to
     * the end of allocated space */
    ((MMArray*)recordingSound.wavtab)->length = soundSampleMaxLength;
    /* Set the area it is pointing to to the beginning of the allocated space */
    ((MMArray*)recordingSound.wavtab)->data = recordingSound.area;
    wtr.buffer = recordingSound.wavtab;
    wtr.currentIndex = 0;
    wtr.state = MMWavTabRecorderState_RECORDING;
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
        if (scheduleRecording == 1) {
            /* Turn off scheduleRecording so the recording is not stopped
             * prematurely when an event that does that is called by the
             * scheduler. when an event that does that is called by the
             * scheduler. */
            scheduleRecording = 0;
            synth_control_record_start_helper();
        } else {
            if (schedulerState == 1) {
                schedulerState_off_helper((void*)noteOnEventListHead);
            }
            SynthControlRecMode _recMode = synth_control_get_recMode();
            if (_recMode == SynthControlRecMode_REC_LEN_1_BEAT_REC_SCHED) {
                /* Recording will be stopped by event in scheduler */
                scheduleRecording = 1;
            } else {
                synth_control_record_stop_helper();
            }
            schedulerState_on_helper();
        }
    } else if (wtr.state == MMWavTabRecorderState_STOPPED) {
#ifdef DEBUG
       assert(scheduleRecording == 0);
#endif  
       synth_control_record_start_helper();
    }
}

void synth_control_feedback_control(void *data_, uint32_t feedback_param)
{
    if (feedback_param > 0) {
        /* Move fbBusSplitter to onNode */
        MMSigProc_insertAfter(fbOnNode,&fbBusSplitter);
        feedbackState = 1;
    } else {
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

void synth_control_feedback_tog(void)
{
    if (feedbackState) {
        synth_control_feedback_control(NULL,0);
    } else {
        synth_control_feedback_control(NULL,1);
    }
}

void synth_control_dryGain_control(void *data_, float dryGain_param)
{
    dryGain = (int16_t)(dryGain_param * 127.);
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
    schedule_noteSched_event(0, NoteSchedEvent_new(1));
    schedulerState = 1;
}

void synth_control_schedulerState_control(void *data_, uint32_t schedulerState_param)
{
    if (schedulerState_param > 0) {
        schedulerState_on_helper();
    } else {
        schedulerState_off_helper(&noteOnEventListHead);
    }
}

void synth_control_schedulerState_tog(void)
{
    if (schedulerState) {
        /* If record scheduling on, it is turned off when the scheduler is
         * turned off. */
        if (scheduleRecording == 1) {
            synth_control_autoRecord_stop_helper();
        }
        schedulerState_off_helper(&noteOnEventListHead);
    } else {
        schedulerState_on_helper();
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

void synth_control_recordScheduling_control(void *data_,
        uint32_t recordScheduling_param)
{
    if (recordScheduling_param) {
        /* If recording in progress, stop it */
        if (wtr.state == MMWavTabRecorderState_RECORDING) {
            wtr.state = MMWavTabRecorderState_STOPPED;
        }
        scheduleRecording = 1;
        /* Set first scheduled recording to true so that when the first
         * scheduled recording happens, the buffers aren't swapped. This is
         * because the buffer it swaps with might contain garbage. */
        firstScheduledRecording = 1;
    } else {
        scheduleRecording = 0;
        /* Stop recording (it will most likely be in progress) but don't swap
         * the recording and playing sounds. We discard the most recent
         * recording to give the user time to flip the switch if they like the
         * previous recording */
        wtr.state = MMWavTabRecorderState_STOPPED;
    }
}

void synth_control_gainMode_control(void *data_,
        uint32_t gainMode_param)
{
    gainMode = (SynthControlGainMode)gainMode_param;
}

void synth_control_set_wet(float gain_param)
{
    /* Gain in dB */
    gain_param = SYNTH_CONTROL_MIN_GAIN 
        + (SYNTH_CONTROL_MAX_GAIN - SYNTH_CONTROL_MIN_GAIN)*gain_param;
    if (gain_param < SYNTH_CONTROL_GAIN_THRESH) {
        noteParamSets[editingWhichParams].amplitude = 0.;
    } else {
        noteParamSets[editingWhichParams].amplitude =
            powf(10.,gain_param / 20.);
    }
}

void synth_control_set_fade(float gain_param, int num_repeats)
{
    /* This sets the amplitude scaling at the last repeat */
    gain_param = powf(gain_param,1./((float)(num_repeats+1)));
    noteParamSets[editingWhichParams].fadeRate = gain_param;
}

void synth_control_set_ampLastEcho(float gain_param)
{
    if (gain_param <= 0.5) {
        gain_param *= 2.;
        gain_param = (1. - SYNTH_CONTROL_ECHO_MIN) * gain_param
            + SYNTH_CONTROL_ECHO_MIN;
    } else {
        gain_param = (gain_param - 0.5) * 2.;
        gain_param =  (SYNTH_CONTROL_ECHO_MAX - 1.) * gain_param + 1.;
    }
    noteParamSets[editingWhichParams].ampLastEcho = gain_param;
    synth_control_set_fade(noteParamSets[editingWhichParams].ampLastEcho,
                           noteParamSets[editingWhichParams].numRepeats);
}

void synth_control_gain_control(void *data_, float gain_param)
{
    switch (gainMode) {
        case SynthControlGainMode_WET:
            synth_control_set_wet(gain_param);
            break;
        case SynthControlGainMode_FADE:
            synth_control_set_ampLastEcho(gain_param);
            break;
    }
}

void synth_control_posMode_control(void *data_, uint32_t posMode_param)
{
    posMode = (SynthControlPosMode)posMode_param;
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
    param_sets[0].positionStride = SYNTH_CONTROL_DEFAULT_POSITIONSTRIDE;
    uint32_t _n;
    for (_n = 0; _n < SYNTH_CONTROL_PITCH_TABLE_SIZE; _n++) {
        param_sets[0].pitches[_n] = SYNTH_CONTROL_DEFAULT_PITCH;
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
        param_sets[size].positionStride = SYNTH_CONTROL_DEFAULT_POSITIONSTRIDE;
        for (_n = 0; _n < SYNTH_CONTROL_PITCH_TABLE_SIZE; _n++) {
            param_sets[0].pitches[_n] = SYNTH_CONTROL_DEFAULT_PITCH;
        }
    };
}

void synth_control_setup(void)
{
    synth_control_reset_param_sets(noteParamSets,NUM_NOTE_PARAM_SETS);
    noteDeltaFromBuffer = 0;
    dryGain             = 0;
    editingWhichParams  = 0;
    tempoBPM_coarse     = SYNTH_CONTROL_DEFAULT_TEMPOBPM_COARSE;
    tempoBPM_fine       = SYNTH_CONTROL_DEFAULT_TEMPOBPM_FINE;
    tempoBPM_scale      = SYNTH_CONTROL_DEFAULT_TEMPOBPM_SCALE;
    tempoBPM            = SYNTH_CONTROL_DEFAULT_TEMPOBPM;
    posMode             = SynthControlPosMode_ABSOLUTE;
    deltaButtonMode     = SynthControlDeltaButtonMode_EVENT_DELTA_FREE;
    recMode             = SynthControlRecMode_NORMAL;
    gainMode            = SynthControlGainMode_WET;
    feedbackState       = 0;
    scheduleRecording   = 0;
    schedulerState      = 0;
    editing_which_pitch = 0;
    /* The recorder trigger requires the zero crossing search be initialized */
    HannWindowTable_init(REC_LOOP_FADE_TIME_S * 2.);
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

SynthControlRecMode synth_control_get_recMode(void)
{
    return recMode;
}

void synth_control_set_posMode(SynthControlPosMode posMode_param)
{
    posMode = (SynthControlPosMode)posMode_param;
}

SynthControlPosMode synth_control_get_posMode(void)
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

float synth_control_get_tempoBPM(void)
{
    return tempoBPM;
}

float synth_control_get_tempoBPM_coarse(void)
{
    return tempoBPM_coarse;
}

float synth_control_get_tempoBPM_fine(void)
{
    return tempoBPM_fine;
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

void synth_control_pitch_reset_tog(void)
{
    uint32_t _m, _n;
    _n = synth_control_get_editingWhichParams();
    for (_m = 0; _m < SYNTH_CONTROL_PITCH_TABLE_SIZE; _m++) {
        noteParamSets[_n].pitches[_m] = 0;
    }
}
