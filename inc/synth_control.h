#ifndef SYNTH_CONTROL_H
#define SYNTH_CONTROL_H 
#include "mm_time.h" 
#include "signal_chain.h" 

/* The amount of fade at the end of the recording in seconds */
#define REC_LOOP_FADE_TIME_S 0.1 

extern MMSample attackTime;
extern MMSample shortReleaseTime;
extern MMSample sustainTime;
extern MMSample releaseTime;
extern MMTime   schedulerInc;
extern MMTime   eventDelta; 
extern MMSample pitch;
extern MMSample amplitude;
extern MMSample startPoint; /* between 0 and 1 */

void autorelease_on_done(MMEnvedSamplePlayer * esp);

#endif /* SYNTH_CONTROL_H */
