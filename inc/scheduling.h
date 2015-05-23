#ifndef SCHEDULING_H
#define SCHEDULING_H 
#include "mm_seq.h"

#define EVENT_DELTA 1000 

typedef struct __NoteOnEvent NoteOnEvent;

extern MMSeq *sequence;
void scheduler_setup(void);
void schedule_event(MMTime timeFromNow);
void scheduler_incTimeAndDoEvents(void);

#endif /* SCHEDULING_H */
