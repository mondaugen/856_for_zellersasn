#ifndef SCHEDULING_H
#define SCHEDULING_H 
#include "mm_seq.h"
#include "mm_dllist.h" 
#include "mm_sample.h" 

typedef struct __NoteOnEvent NoteOnEvent;
typedef struct __NoteSchedEvent NoteSchedEvent;
typedef struct __NoteOnEventListNode NoteOnEventListNode;
typedef struct __NoteSchedEventListNode NoteSchedEventListNode;

struct __NoteOnEventListNode {
    MMDLList head;
    NoteOnEvent *child;
};

struct __NoteSchedEventListNode {
    MMDLList head;
    NoteSchedEvent *child;
};

extern MMSeq *sequence;
extern NoteOnEventListNode noteOnEventListHead[];
extern NoteSchedEventListNode noteSchedEventListHead;
extern int noteOnEventCount[];
void scheduler_setup(void);
void schedule_noteOn_event(MMTime timeFromNow, NoteOnEvent *ev);
void scheduler_incTimeAndDoEvents(void);
void set_noteOnEvents_active(NoteOnEventListNode *head);
void set_noteOnEvents_inactive(NoteOnEventListNode *head);
NoteOnEvent *NoteOnEvent_new(int active,
        int parameterSet,
        int numRepeats,
        int repeatIndex,
        MMSample currentFade);

#endif /* SCHEDULING_H */
