#ifndef SYNTH_CONTROL_H
#define SYNTH_CONTROL_H 
#include "mm_time.h" 
#include "signal_chain.h" 
#include <stdint.h> 

/* The amount of fade at the end of the recording in seconds */
#define REC_LOOP_FADE_TIME_S 0.1 

extern MMSample attackTime;
extern MMSample shortReleaseTime;
extern MMSample sustainTime;
extern MMSample releaseTime;
/* The amount the scheduler is incremented each block */
extern MMSample tempoBPM; 
/* The time between two scheduled events */
extern MMSample eventDeltaBeats; 
extern MMSample pitch;
extern MMSample amplitude;
extern MMSample startPoint; /* between 0 and 1 */
extern int16_t  dryGain;
extern int      schedulerState;

void autorelease_on_done(MMEnvedSamplePlayer * esp);

#endif /* SYNTH_CONTROL_H */
