#ifndef SYNTH_CONTROL_H
#define SYNTH_CONTROL_H 
#include "mm_time.h" 
#include "signal_chain.h" 
#include <stdint.h> 

/* The amount of fade at the end of the recording in seconds */
#define REC_LOOP_FADE_TIME_S 0.025 
/* The number of sets of note parameters */
#define NUM_NOTE_PARAM_SETS 2 

typedef enum {
    SynthControlPosMode_ABSOLUTE,
    SynthControlPosMode_STRIDE
} SynthControlPosMode;

typedef enum {
    SynthControlEventDeltaMode_FREE,
    SynthControlEventDeltaMode_QUANT
} SynthControlEventDeltaMode;

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
    MMSample eventDeltaBeats; 
    MMSample pitch;
    MMSample amplitude;
    MMSample startPoint; /* between 0 and 1 */
} NoteParamSet;

extern NoteParamSet noteParamSets[];

extern SynthControlPosMode         posMode;
extern SynthControlEventDeltaMode  eventDeltaMode;
extern SynthControlPitchMode       pitchMode;

extern int16_t  dryGain;
/* The amount the scheduler is incremented each block */
extern MMSample tempoBPM; 
extern int multiParamSetsAllowed;

void autorelease_on_done(MMEnvedSamplePlayer * esp);

#endif /* SYNTH_CONTROL_H */
