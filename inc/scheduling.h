/* Copyright (c) 2016 Nicholas Esterer. All rights reserved. */

#ifndef SCHEDULING_H
#define SCHEDULING_H 
#include "mm_seq.h"
#include "mm_dllist.h" 
#include "mm_sample.h" 
#include "leds.h" 
#include "synth_control.h" 

/* Amplitude below which playback is not triggered */
#define SCHEDULING_AMP_FLOOR 3.05E-5 /* ~ 2^-15 */

#define MEASURE_LED_SET() led7_set()
#define MEASURE_LED_RESET() led7_reset()
/* The fraction of the measure from the beginning of the measure at which time
 * the LED will be turned off. 4 is quarter of the measure, 3 is third, etc. */
#define MEASURE_LED_LENGTH_SCALAR (16ULL)

typedef struct __NoteOnEvent NoteOnEvent;
typedef struct __NoteSchedEvent NoteSchedEvent;
typedef struct __MeasureLEDOffEvent MeasureLEDOffEvent;
typedef struct __NoteOnEventListNode NoteOnEventListNode;
typedef struct __NoteSchedEventListNode NoteSchedEventListNode;
typedef struct __MeasureLEDOffEventListNode MeasureLEDOffEventListNode;

struct __NoteOnEventListNode {
    MMDLList head;
    NoteOnEvent *child;
};

struct __NoteSchedEventListNode {
    MMDLList head;
    NoteSchedEvent *child;
};

struct __MeasureLEDOffEventListNode {
    MMDLList head;
    MeasureLEDOffEvent *child;
};

extern MMSeq *sequence;
extern NoteOnEventListNode noteOnEventListHead[];
extern NoteSchedEventListNode noteSchedEventListHead;
extern MeasureLEDOffEventListNode measureLEDOffEventListHead;
extern int noteOnEventCount[];
void scheduler_setup(void);
void schedule_noteOn_event(MMTime timeFromNow, NoteOnEvent *ev);
void scheduler_incTimeAndDoEvents(void);
void set_noteOnEvents_active(NoteOnEventListNode *head);
void set_noteOnEvents_inactive(NoteOnEventListNode *head);
void set_noteSchedEvents_active(NoteSchedEventListNode *head);
void set_noteSchedEvents_inactive(NoteSchedEventListNode *head);
void set_measureLEDOffEvents_inactive(MeasureLEDOffEventListNode *head);
void schedule_noteSched_event(uint64_t timeFromNow, NoteSchedEvent *ev);
NoteSchedEvent *NoteSchedEvent_new(int active);
NoteOnEvent *NoteOnEvent_new(int active,
        int parameterSet,
        int numRepeats,
        int repeatIndex,
        MMSample currentFade,
        MMSample currentPosition,
        MMSample currentPitch,
        MMSample pitchOffset,
        int pitch_idx);
void NoteSchedEvent_set_pitch_offset(NoteSchedEvent *nse, MMSample pitch);
void NoteSchedEvent_set_pitch_mode(NoteSchedEvent *nse, SynthControlPitchMode pitch_mode);
void NoteSchedEvent_set_amplitude_scalar(NoteSchedEvent *nse, MMSample amp);
void NoteSchedEvent_set_one_shot(NoteSchedEvent *nse, int one_shot);

#endif /* SCHEDULING_H */
