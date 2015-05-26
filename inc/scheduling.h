#ifndef SCHEDULING_H
#define SCHEDULING_H 
#include "mm_seq.h"
#include "mm_dllist.h" 

typedef struct __NoteOnEvent NoteOnEvent;
typedef struct __NoteOnEventListNode NoteOnEventListNode;

struct __NoteOnEventListNode {
    MMDLList head;
    NoteOnEvent *child;
};

extern MMSeq *sequence;
extern NoteOnEventListNode noteOnEventListHead;
void scheduler_setup(void);
void schedule_event(MMTime timeFromNow, NoteOnEvent *ev);
void scheduler_incTimeAndDoEvents(void);
void set_noteOnEvents_active(NoteOnEventListNode *head);
void set_noteOnEvents_inactive(NoteOnEventListNode *head);
NoteOnEvent *NoteOnEvent_new(int active, int parameterSet);

#endif /* SCHEDULING_H */
