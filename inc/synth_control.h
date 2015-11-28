#ifndef SYNTH_CONTROL_H
#define SYNTH_CONTROL_H 
#include "mm_time.h" 
#include "signal_chain.h" 
#include <stdint.h> 
#include "synth_control_presets.h" 

/* The amount of fade at the end of the recording in seconds */
#define REC_LOOP_FADE_TIME_S 0.010
/* Envelope parameters */
#define SYNTH_CONTROL_MIN_ATTACK_TIME 0.001 
#define SYNTH_CONTROL_MAX_ATTACK_TIME 0.5
#define SYNTH_CONTROL_MIN_RELEASE_TIME 0.001
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
#define SYNTH_CONTROL_DEFAULT_PITCH 60
#define SYNTH_CONTROL_DEFAULT_AMPLITUDE .5
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
/* Fade rate doesn't apply to the first
   parameter set */
#define SYNTH_CONTROL_DEFAULT_FADERATE      0      
/* The amount the starting point in the sample
   is advanced each time it is scheduled (if
   stride enabled) */
/* The fade rate of notes other than the first note is not 0 */
#define SYNTH_CONTROL_DEFAULT_FADERATE_AUXNOTE 1.
#define SYNTH_CONTROL_DEFAULT_POSITIONSTRIDE 0      
#define SYNTH_CONTROL_DEFAULT_TEMPOBPM_COARSE 120 
#define SYNTH_CONTROL_DEFAULT_TEMPOBPM_FINE   0 
#define SYNTH_CONTROL_DEFAULT_TEMPOBPM_SCALE  1. 
#define SYNTH_CONTROL_DEFAULT_TEMPOBPM\
   ((SYNTH_CONTROL_DEFAULT_TEMPOBPM_COARSE\
    + SYNTH_CONTROL_DEFAULT_TEMPOBPM_FINE)\
    * SYNTH_CONTROL_DEFAULT_TEMPOBPM_SCALE)
#define SYNTH_CONTROL_TEMPOBPM_COARSE_MIN     40.
#define SYNTH_CONTROL_TEMPOBPM_COARSE_MAX     240.
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

typedef uint32_t SynthControlEditingWhichParamsIndex;
/* The number of sets of note parameters */
#define NUM_NOTE_PARAM_SETS 3 
typedef uint32_t SynthControlPresetNumber;


typedef enum {
    /* Control the absolute starting point in the sound file */
    SynthControlPosMode_ABSOLUTE,
    /* Control the amount by which the starting point is incremented or
     * decremented each time playback is repeated. The starting point is reset
     * when the initial note is played. */
    SynthControlPosMode_STRIDE,
    /* There is room for an additional mode here. */
    SynthControlPosMode_UNKNOWN
} SynthControlPosMode;
#define SYNTH_CONTROL_POS_MODE_N_MODES 3

/*
typedef enum {
    SynthControlEventDeltaMode_FREE,
    SynthControlEventDeltaMode_QUANT
} SynthControlEventDeltaMode;
*/

typedef enum {
    /* Adjust the event delta freely */
    SynthControlDeltaButtonMode_EVENT_DELTA_FREE,
    /* Adjust the event delta, quantizing to some fractions of a beat */
    SynthControlDeltaButtonMode_EVENT_DELTA_QUANT,
    /* Adjust how often an event occurs. */
    SynthControlDeltaButtonMode_INTERMITTENCY
} SynthControlDeltaButtonMode;
#define SYNTH_CONTROL_DELTA_BUTTON_MODE_N_MODES 3 

typedef enum {
    /* The pitches chosen are quantized to the chromatic notes -12 to 12 where 0
     * will play the soundfile at a speed of 0. Or perhaps free, limited by
     * precision of input. */
    SynthControlPitchMode_CHROM,
    /* Pitches chosen are quantized to ... -7, -5, 0, 5, 7 ... for some yet
     * undetermined range. */
    SynthControlPitchMode_4TH5TH,
    /* Pitches change according to some sequence, starting at the pitch given by
     * CHROM or 4TH5TH. The sequences have yet to be determined. */
    SynthControlPitchMode_ARP
} SynthControlPitchMode;
#define SYNTH_CONTROL_PITCH_MODE_N_MODES 3 

typedef enum {
    /* Control the fade amount for repeated notes. */
    SynthControlGainMode_FADE,
    /* Control the gain of new notes */
    SynthControlGainMode_WET
} SynthControlGainMode;
#define SYNTH_CONTROL_GAIN_MODE_N_MODES 2 

typedef enum {
    /* The length of the recording does not influence the scheduler tempo. */
    SynthControlRecMode_NORMAL,
    /* The length of the recording is used to adjust the tempo of the scheduler
     * so that its length is equal to 1 beat. */
    SynthControlRecMode_REC_LEN_1_BEAT,
    /* The scheduler's tempo is adjusted as with
     * SynthControlRecMode_REC_LEN_1_BEAT but after the first recording, new
     * recordings are automatically scheduled which will have the same duration
     * as the first recording. */
    SynthControlRecMode_REC_LEN_1_BEAT_REC_SCHED
} SynthControlRecMode;
#define SYNTH_CONTROL_REC_MODE_N_MODES 2

typedef struct __NoteParamSet {
    MMSample attackTime;
    MMSample sustainTime;
    MMSample releaseTime;
    /* The time between two scheduled events */
    MMSample eventDeltaBeats; /* The amount of time between repeats */
    MMSample pitch;
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
} NoteParamSet;

/* Stuff that could be saved in a preset */
extern NoteParamSet                 noteParamSets[];
extern SynthControlPosMode          posMode;
extern SynthControlDeltaButtonMode  deltaButtonMode;
extern SynthControlPitchMode        pitchMode;
extern SynthControlGainMode         gainMode;
extern SynthControlRecMode          recMode;
extern int                          schedulerState;

extern int16_t  dryGain;
/* The amount the scheduler is incremented each block */
extern int                         scheduleRecording;
extern int                         firstScheduledRecording;

void synth_control_setup(void);
void autorelease_on_done(MMEnvedSamplePlayer * esp);
void synth_control_record_stop_helper(void);
void synth_control_record_start_helper(void);
void synth_control_record_tog(void);
void synth_control_schedulerState_tog(void);
void synth_control_presetStore_tog(void);
void synth_control_presetRecall_tog(void);
void synth_control_feedback_tog(void);

SynthControlEditingWhichParamsIndex synth_control_get_editingWhichParams(void);
void synth_control_set_editingWhichParams(
        SynthControlEditingWhichParamsIndex editingWhichParams_param);
void synth_control_set_deltaButtonMode(SynthControlDeltaButtonMode
        deltaButtonMode_param);
SynthControlDeltaButtonMode synth_control_get_deltaButtonMode(void);
void synth_control_set_recMode(SynthControlRecMode recMode_param);
SynthControlRecMode synth_control_get_recMode(void);
void synth_control_set_pitchMode(SynthControlPitchMode pitchMode_param);
SynthControlPitchMode synth_control_get_pitchMode(void);
void synth_control_set_posMode(SynthControlPosMode posMode_param);
SynthControlPosMode synth_control_get_posMode(void);
void synth_control_set_gainMode(SynthControlGainMode gainMode_param);
SynthControlGainMode synth_control_get_gainMode(void);
void synth_control_set_presetNumber(SynthControlPresetNumber presetNumber_param);
SynthControlPresetNumber synth_control_get_presetNumber(void);
void synth_control_set_envelopeTime(float envelopeTime_param);
void synth_control_set_sustainTime(float sustainTime_param);
void synth_control_set_startPoint(float startPoint_param);
void synth_control_set_positionStride(float positionStride_param);
void synth_control_set_eventDelta_quant(float eventDeltaBeats_param);
void synth_control_set_eventDelta_free(float eventDeltaBeats_param);
void synth_control_set_intermittency(float intermittency_param);
void synth_control_set_pitch_chrom(float pitch_param);
void synth_control_set_pitch_4ths5ths(float pitch_param);
void synth_control_set_pitch_arp(float pitch_param);
void synth_control_set_wet(float gain_param);
void synth_control_set_fade(float gain_param, int num_repeats);
int synth_control_get_noteDeltaFromBuffer(void);
void synth_control_tempoNudge(float tempoNudge_param);
void synth_control_set_tempo(float _tempoBPM_coarse,
                             float _tempoBPM_fine,
                             float _tempoBPM_scale);
void synth_control_set_repeats(float repeats_param);
int synth_control_get_recordState(void);
int synth_control_get_schedulerState(void);
int synth_control_get_feedbackState(void);
void synth_control_set_offset(float offset_param);
void synth_control_set_ampLastEcho(float gain_param);
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

#endif /* SYNTH_CONTROL_H */
