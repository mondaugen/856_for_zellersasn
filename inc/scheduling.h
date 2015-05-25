#ifndef SCHEDULING_H
#define SCHEDULING_H 
#include "mm_seq.h"
#include "mm_dllist.h" 

#define EVENT_DELTA 1000 

typedef struct __NoteOnEvent NoteOnEvent;
typedef struct __NoteOnEventListNode NoteOnEventListNode;

struct __NoteOnEventListNode {
    MMDLList head;
    NoteOnEvent *child;
};

extern MMSeq *sequence;
extern NoteOnEventListNode noteOnEventListHead;
void scheduler_setup(void);
void schedule_event(MMTime timeFromNow);
void scheduler_incTimeAndDoEvents(void);
void set_noteOnEvents_active(NoteOnEventListNode *head);
void set_noteOnEvents_inactive(NoteOnEventListNode *head);

#endif /* SCHEDULING_H */
