#ifndef TIMERS_H
#define TIMERS_H 
#include <stdint.h> 

typedef struct __timer_event_t timer_event_t;

struct __timer_event_t {
    /* the next event in the list of events */
    timer_event_t *next;
    /* is event active ? */
    uint32_t active;
    /* The number of ticks remaining before timeout. See timers.c for period of
     * one tick. */
    uint32_t time_rem;
    /* What should happen on timeout */
    void (*on_timeout)(timer_event_t*);
    /* Stuff that can be used on timeout */
    void *data;
};

void timers_setup(void);
void timers_enable(void);
void timers_disable(void);
void timer_events_add_event(timer_event_t *ev);
void timer_event_init(timer_event_t *ev);
int timers_get_state(void);

#define TIMER_EVENT_TIMED_OUT(ev) ((ev)->time_rem == 0) 

#endif /* TIMERS_H */
