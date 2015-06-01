#ifndef SYNTH_CONTROL_H
#define SYNTH_CONTROL_H 
#include "mm_time.h" 
#include "signal_chain.h" 
#include <stdint.h> 

/* The amount of fade at the end of the recording in seconds */
#define REC_LOOP_FADE_TIME_S 0.025 
/* The number of sets of note parameters */
#define NUM_NOTE_PARAM_SETS 3 
/* Envelope parameters */
#define SYNTH_CONTROL_MIN_ATTACK_TIME 0.001 
#define SYNTH_CONTROL_MAX_ATTACK_TIME 0.5
#define SYNTH_CONTROL_MIN_RELEASE_TIME 0.001
#define SYNTH_CONTROL_MAX_RELEASE_TIME 0.5

typedef enum {
    SynthControlPosMode_ABSOLUTE,
    SynthControlPosMode_STRIDE
} SynthControlPosMode;

/*
typedef enum {
    SynthControlEventDeltaMode_FREE,
    SynthControlEventDeltaMode_QUANT
} SynthControlEventDeltaMode;
*/

typedef enum {
    SynthControlDeltaButtonMode_EVENT_DELTA,
    SynthControlDeltaButtonMode_INTERMITTENCY
} SynthControlDeltaButtonMode;

typedef enum {
    SynthControlPitchMode_CHROM,
    SynthControlPitchMode_4TH5TH,
    SynthControlPitchMode_LINKTEMPO
} SynthControlPitchMode;

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

} NoteParamSet;

extern NoteParamSet noteParamSets[];

extern SynthControlPosMode         posMode;
//extern SynthControlEventDeltaMode  eventDeltaMode;
extern SynthControlDeltaButtonMode  deltaButtonMode;
extern SynthControlPitchMode       pitchMode;

extern int16_t  dryGain;
/* The amount the scheduler is incremented each block */
extern MMSample tempoBPM; 

void autorelease_on_done(MMEnvedSamplePlayer * esp);

#endif /* SYNTH_CONTROL_H */
