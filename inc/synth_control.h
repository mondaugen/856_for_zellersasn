/* Copyright (c) 2016 Nicholas Esterer. All rights reserved. */

#ifndef SYNTH_CONTROL_H
#define SYNTH_CONTROL_H 
#include "mm_time.h" 
#include "signal_chain.h" 
#include <stdint.h> 
#include "synth_control_presets.h" 
#include "tables.h"

typedef int SynthControlPitchIndex;

typedef enum {
    /* Control the absolute starting point in the sound file */
    SynthControlPosMode_ABSOLUTE,
    /* Control the amount by which the starting point is incremented or
     * decremented each time playback is repeated. The starting point is reset
     * when the initial note is played. */
    SynthControlPosMode_STRIDE,
    /* There is room for an additional mode here. */
    SynthControlPosMode_UNI
} SynthControlPosMode;
#define SYNTH_CONTROL_POS_MODE_N_MODES 3

typedef enum {
    /* Adjust the event delta freely */
    SynthControlDeltaButtonMode_EVENT_DELTA_FREE,
    /* Adjust the event delta, quantizing to some fractions of a beat */
    SynthControlDeltaButtonMode_EVENT_DELTA_QUANT,
    /* Adjust how often an event occurs. */
    SynthControlDeltaButtonMode_INTERMITTENCY
} SynthControlDeltaButtonMode;
#define SYNTH_CONTROL_DELTA_BUTTON_MODE_N_MODES 3 

#define SYNTH_CONTROL_PITCH_TABLE_SIZE 3 

typedef enum {
    /* Control the fade amount for repeated notes. */
    SynthControlGainMode_FADE,
    /* Control the gain of new notes */
    SynthControlGainMode_WET,
    /* If the feedback switch is held in downward position, the switch is in this state */
    SynthControlGainMode_FBKHOLD
} SynthControlGainMode;
#define SYNTH_CONTROL_GAIN_MODE_N_MODES 3

typedef enum {
    SynthControlRecMode_START__ = 0,
    /* The length of the recording does not influence the scheduler tempo. */
    SynthControlRecMode_NORMAL = SynthControlRecMode_START__,
    /* The length of the recording is used to adjust the tempo of the scheduler
     * so that its length is equal to 1 beat. */
    SynthControlRecMode_REC_LEN_1_BEAT,
    /* The scheduler's tempo is adjusted as with
     * SynthControlRecMode_REC_LEN_1_BEAT but after the first recording, new
     * recordings are automatically scheduled which will have the same duration
     * as the first recording. */
    SynthControlRecMode_REC_LEN_1_BEAT_REC_SCHED,
    SynthControlRecMode_END__ = SynthControlRecMode_REC_LEN_1_BEAT_REC_SCHED
} SynthControlRecMode;

typedef enum __ {
    SynthControlPitchMode_ABSOLUTE = 0,
    SynthControlPitchMode_RELATIVE,
    SynthControlPitchMode_BUS /* Controlled by a value that can update between
                                 signal processing ticks */
} SynthControlPitchMode;

#define SYNTH_CONTROL_SWING_TABLE_SIZE TABLES_SWING_SET_SIZE 

typedef struct __NoteParamSet {
    MMSample attackTime;
    MMSample sustainTime;
    MMSample releaseTime;
    /* The time between two scheduled events */
    MMSample eventDeltaBeats; /* The amount of time between repeats */
    MMSample pitches[SYNTH_CONTROL_PITCH_TABLE_SIZE];
    MMSample fine_pitches[SYNTH_CONTROL_PITCH_TABLE_SIZE];
    /* The "pitch busses" that are read between signal processing ticks to get
     * the current rate */
    mm_q8_24_t rate_busses[SYNTH_CONTROL_PITCH_TABLE_SIZE];
    MMSample amplitude;
    MMSample startPoint; /* between 0 and 1 */
    int numRepeats;      /* The number of times repeated */
    MMSample offsetBeats;/* The amount of beats offset from the beginning of the bar */
    int intermittency;   /* Canonically the number of repeats that are ignored
                            so that you obtain some patern of played/unplayed.
                            For example, if intermittency = 1, you get
                            played, played, played ... for the number of repeats
                            if intermittency = 2, you get
                            played, unplayed, played ... for the number of
                            repeats (it will be played numRepeats/2 times).
                            Of course this number could later stand for some
                            more complicated pattern. */
    MMSample ampLastEcho; /* The amplitude of the last echo, which is used to
                             calculate the fade rate based on the number of
                             repeats */
    MMSample fadeRate;
    MMSample positionStride; /* If stride enabled, how much the position head is advanced each playback */
    SynthControlPosMode posMode; /* Whether there is stride or the position stays absolute */
    MMSample swing[SYNTH_CONTROL_SWING_TABLE_SIZE];
} NoteParamSet;

/* The amount of fade at the end of the recording in seconds */
#define REC_LOOP_FADE_TIME_S 0.010
/* Envelope parameters */
#define SYNTH_CONTROL_MIN_ATTACK_TIME 0.01 
#define SYNTH_CONTROL_MAX_ATTACK_TIME 0.5
#define SYNTH_CONTROL_MIN_RELEASE_TIME 0.01
#define SYNTH_CONTROL_MAX_RELEASE_TIME 0.5

/* Gain parameters */
#define SYNTH_CONTROL_MAX_GAIN 0
#define SYNTH_CONTROL_MIN_GAIN -35
/* gains below this are effectively 0 */
#define SYNTH_CONTROL_GAIN_THRESH -30

/* Repeat parameters */
#define SYNTH_CONTROL_MAX_NUM_REPEATS 16
/* The minimum amplitude for the last repeat */
#define SYNTH_CONTROL_ECHO_MIN 1.E-3 /* -60dB */
#define SYNTH_CONTROL_ECHO_MAX 4.    /* 12 dB */

#define SYNTH_CONTROL_DEFAULT_ATTACKTIME 0.01     
#define SYNTH_CONTROL_DEFAULT_SUSTAINTIME  1 
#define SYNTH_CONTROL_DEFAULT_RELEASETIME 0.01
#define SYNTH_CONTROL_DEFAULT_EVENTDELTABEATS 1
#define SYNTH_CONTROL_PITCH_OFFSET 69
#define SYNTH_CONTROL_DEFAULT_PITCH (0+SYNTH_CONTROL_PITCH_OFFSET)
#define SYNTH_CONTROL_DEFAULT_FINEPITCH 0.
#define SYNTH_CONTROL_DEFAULT_AMPLITUDE 1.
/* Other notes are off by default */
#define SYNTH_CONTROL_DEFAULT_AMPLITUDE_AUXNOTE 0 
#define SYNTH_CONTROL_DEFAULT_STARTPOINT 0
#define SYNTH_CONTROL_DEFAULT_NUMREPEATS 0
/* The amount of beats offset from the beginning
   of the bar */
#define SYNTH_CONTROL_DEFAULT_OFFSETBEATS 0       
/* Canonically the number of repeats that are
   ignored */
#define SYNTH_CONTROL_DEFAULT_INTERMITTENCY 0      
#define SYNTH_CONTROL_DEFAULT_FADERATE      1      
#define SYNTH_CONTROL_DEFAULT_AMPLASTECHO   1      
/* The amount the starting point in the sample
   is advanced each time it is scheduled (if
   stride enabled) */
/* The fade rate of notes other than the first note is not 0 */
#define SYNTH_CONTROL_DEFAULT_FADERATE_AUXNOTE      1.
#define SYNTH_CONTROL_DEFAULT_AMPLASTECHO_AUXNOTE   1.     
#define SYNTH_CONTROL_DEFAULT_POSITIONSTRIDE 0      
#define SYNTH_CONTROL_DEFAULT_POSMODE  SynthControlPosMode_ABSOLUTE     
#define SYNTH_CONTROL_DEFAULT_TEMPOBPM_FINE   0 
#define SYNTH_CONTROL_DEFAULT_TEMPOBPM_SCALE  1. 
#define SYNTH_CONTROL_DEFAULT_TEMPOBPM\
   ((SYNTH_CONTROL_DEFAULT_TEMPOBPM_COARSE\
    + SYNTH_CONTROL_DEFAULT_TEMPOBPM_FINE)\
    * SYNTH_CONTROL_DEFAULT_TEMPOBPM_SCALE)
#define SYNTH_CONTROL_TEMPOBPM_COARSE_MIN     11.
#define SYNTH_CONTROL_TEMPOBPM_COARSE_MAX     60.
#define SYNTH_CONTROL_DEFAULT_TEMPOBPM_COARSE \
(0.5*(SYNTH_CONTROL_TEMPOBPM_COARSE_MAX - SYNTH_CONTROL_TEMPOBPM_COARSE_MIN))
#define SYNTH_CONTROL_TEMPOBPM_COARSE_QUANT   10.
#define SYNTH_CONTROL_TEMPOBPM_FINE_MIN       -10.
#define SYNTH_CONTROL_TEMPOBPM_FINE_MAX       10.
#define SYNTH_CONTROL_TEMPOBPM_FINE_QUANT     0.1
#define SYNTH_CONTROL_TEMPOBPM_SCALE_TABLE\
    { 1./4., 1./3., 1./2., 1. }
#define SYNTH_CONTROL_TEMPOBPM_SCALE_TABLE_LENGTH 4 
#define SYNTH_CONTROL_EVENTDELTA_QUANT_TABLE\
    { 1./16., 1./12., 1/8., 1./6., 1./4., 1./3., 1./2., 3./4. }
#define SYNTH_CONTROL_EVENTDELTA_QUANT_TABLE_LENGTH 8
#define SYNTH_CONTROL_INTERMITTENCY_TABLE_LENGTH 4
#define SYNTH_CONTROL_INTERMITTENCY_TABLE\
    { 0, 1, 2, 3 } 
#define SYNTH_CONTROL_PITCH_CHROM_MIN -12
#define SYNTH_CONTROL_PITCH_CHROM_MAX  12
#define SYNTH_CONTROL_PITCH_CHROM_QUANT 1 
#define SYNTH_CONTROL_PITCH_FINE_MIN -0.5
#define SYNTH_CONTROL_PITCH_FINE_MAX  0.5
#define SYNTH_CONTROL_PITCH_FINE_QUANT 0.01
#define SYNTH_CONTROL_DEFAULT_RATEBUSRATE (1L << 24L) /* Rate of 1 in Q8_24 format */
/* The MIDI pitch that plays at a rate of 1 */
#define SYNTH_CONTROL_PITCH_UNISON  60 
#define SYNTH_CONTROL_DEFAULT_PITCHMODE SynthControlPitchMode_BUS
#define SYNTH_CONTROL_PITCH_MIN     (0)
#define SYNTH_CONTROL_PITCH_MAX     (127)
#define SYNTH_CONTROL_POS_STRIDE_SCALE 0.2
#define SYNTH_CONTROL_POS_STRIDE_OFFSET 0.1 
#define SYNTH_CONTROL_ABS_MIN_TEMPO_BPM (60. / SAMPLE_TABLE_LENGTH_SEC)
#define SYNTH_CONTROL_ABS_MAX_TEMPO_BPM (60. / (float)(audio_hw_get_block_size(NULL) \
                                        / (float)audio_hw_get_sample_rate(NULL)))
#define SYNTH_CONTROL_DEFAULT_SWING 1. 

typedef uint32_t SynthControlEditingWhichParamsIndex;
/* The number of sets of note parameters */
#define NUM_NOTE_PARAM_SETS 3 
typedef uint32_t SynthControlPresetNumber;

typedef enum {
    scrsh_source_USER,
    scrsh_source_SCHEDULER
} scrsh_source_t;



/* Stuff that could be saved in a preset */
extern NoteParamSet                 noteParamSets[];
extern SynthControlDeltaButtonMode  deltaButtonMode;
extern SynthControlGainMode         gainMode;
extern SynthControlRecMode          recMode;
extern int                          schedulerState;

/* The amount the scheduler is incremented each block */
extern int                         scheduleRecording;
extern int                         firstScheduledRecording;
extern int noteOnEventCount[];

void synth_control_setup(void);
void autorelease_on_done(MMEnvedSamplePlayer * esp);
void synth_control_record_stop_helper(scrsh_source_t origin);
void synth_control_record_start_helper(void);
void synth_control_record_tog(void);
void synth_control_schedulerState_tog(void);
void synth_control_presetStore_tog(void);
void synth_control_presetRecall_tog(void);
void synth_control_feedback_control(uint32_t feedback_param);
void synth_control_feedback_tog(void);

SynthControlEditingWhichParamsIndex synth_control_get_editingWhichParams(void);
void synth_control_set_editingWhichParams(
        SynthControlEditingWhichParamsIndex editingWhichParams_param);
void synth_control_set_deltaButtonMode(SynthControlDeltaButtonMode
        deltaButtonMode_param);
SynthControlDeltaButtonMode synth_control_get_deltaButtonMode(void);
void synth_control_set_recMode(SynthControlRecMode recMode_param);
void synth_control_set_recMode_onChange(SynthControlRecMode recMode_param,
                               SynthControlRecMode *last_recMode_param);
SynthControlRecMode synth_control_get_recMode(void);
void synth_control_set_posMode(SynthControlPosMode posMode_param,
                               int which_params);
SynthControlPosMode synth_control_get_posMode_curParams(void);
void synth_control_set_gainMode(SynthControlGainMode gainMode_param);
SynthControlGainMode synth_control_get_gainMode(void);
void synth_control_set_presetNumber(SynthControlPresetNumber presetNumber_param);
SynthControlPresetNumber synth_control_get_presetNumber(void);
int synth_control_get_noteDeltaFromBuffer(void);
void synth_control_tempoNudge(float tempoNudge_param);
void synth_control_set_tempo(float _tempoBPM_coarse,
                             float _tempoBPM_fine,
                             float _tempoBPM_scale);
int synth_control_get_recordState(void);
int synth_control_get_schedulerState(void);
int synth_control_get_feedbackState(void);
void synth_control_reset_param_sets(NoteParamSet *param_sets, int size);
float synth_control_get_tempoBPM(void);
float synth_control_get_tempoBPM_coarse(void);
float synth_control_get_tempoBPM_fine(void);
float synth_control_get_tempoBPM_scale(void);
void synth_control_update_tempo_coarse(float param);
void synth_control_update_tempo_fine(float param);
void synth_control_update_tempo_scale(float param);
void synth_control_set_tempo_coarse_norm(float param);
void synth_control_set_tempo_fine_norm(float param);
void synth_control_set_tempo_scale_norm(float param);
void synth_control_set_tempoBPM_absolute(float _tempoBPM);
void synth_control_set_editing_which_pitch(int _param);
int synth_control_get_editing_which_pitch(void);
void synth_control_pitch_reset_tog(void);
void synth_control_reset_noteOnEventCounts(void);
void synth_control_set_posMode_onChange_curParams(SynthControlPosMode posMode_param,
                                        SynthControlPosMode *last_posMode_param);
void synth_control_set_posMode_onChange(SynthControlPosMode posMode_param,
                                        SynthControlPosMode *last_posMode_param,
                                        int which_params);

void synth_control_set_envelopeTime(float envelopeTime_param,
                                    int note_param_idx);
void synth_control_set_envelopeTime_curParams(float envelopeTime_param);
void synth_control_set_sustainTime(float sustainTime_param,
                                    int note_param_idx);
void synth_control_set_sustainTime_curParams(float sustainTime_param);
void synth_control_set_numRepeats(int numRepeats_param, int note_params_idx);
void synth_control_set_numRepeats_curParams(int numRepeats_param);
void synth_control_set_repeats(float repeats_param);
void synth_control_set_pitch(float pitch_param,
                             int which_pitch,
                             int note_params_idx);
void synth_control_set_pitch_curParams(float pitch_param);
void synth_control_set_pitch_chrom(float pitch_param,
                                   int which_pitch,
                                   int note_params_idx);
void synth_control_set_pitch_chrom_curParams(float pitch_param);
void synth_control_set_pitch_chrom_quant(float param,
                                         int which_pitch,
                                         int note_params_idx);
void synth_control_set_pitch_chrom_quant_curParams(float param);
void synth_control_set_pitch_fine_quant(float param, 
                                        int which_pitch,
                                        int note_param_idx);
void synth_control_set_pitch_fine_curParams(float param);
void synth_control_set_startPoint(float startPoint_param,
                                  int note_params_idx);
void synth_control_set_startPoint_curParams(float startPoint_param);
void synth_control_set_positionStride(float positionStride_param,
                                      int note_params_idx);
void synth_control_set_positionStride_curParams(float positionStride_param);
void synth_control_set_eventDelta_quant(float eventDeltaBeats_param,
                                        int note_params_idx);
void synth_control_set_eventDelta_quant_curParams(float eventDeltaBeats_param);
void synth_control_set_eventDelta_free(float eventDeltaBeats_param,
                                       int note_params_idx);
void synth_control_set_eventDelta_free_curParams(float eventDeltaBeats_param);
void synth_control_set_intermittency(float intermittency_param,
                                     int note_params_idx);
void synth_control_set_intermittency_curParams(float intermittency_param);
void synth_control_set_offset(float offset_param,
                              int note_params_idx);
void synth_control_set_offset_curParams(float offset_param);
void synth_control_set_wet(float gain_param,
                           int note_params_idx);
void synth_control_set_wet_curParams(float gain_param);
void synth_control_set_fade(float gain_param,
                            int num_repeats,
                            int note_params_idx);
void synth_control_set_fade_curParams(float gain_param,
                                      int num_repeats);
void synth_control_set_ampLastEcho(float gain_param,
                                   int note_params_idx);
void synth_control_set_ampLastEcho_curParams(float gain_param);

void synth_control_record_stop(void);
void synth_control_record_start(void);
void synth_control_schedulerState_on(void);
void synth_control_schedulerState_off(void);
void synth_control_note_on(int parameterSet,
                           MMSample pitch,
                           MMSample amplitude);
void synth_control_one_shot(MMSample pitch,
                            MMSample amplitude);

MMSample synth_control_clip_valid_pitch(MMSample pitch);

int synth_control_get_uni_stuff_changed(void);
void synth_control_set_uni_stuff_changed(void);
void synth_control_reset_uni_stuff_changed(void);
void synth_control_fbk_tog(void);
void synth_control_set_swing(float param,
                             int idx);
void synth_control_set_swing_curParams(float param);
void synth_control_expr_ctl_chosen_set(void);

#endif /* SYNTH_CONTROL_H */
