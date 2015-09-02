#include "switch_control.h"

static void fsw1_func(switch_debouncer_t *sd)
{
    if (schedulerState) {
        /* Toggle it off */
        synth_control_schedulerState_control((void*)noteOnEventListHead,0);
    } else {
        /* Toggle it on */
        synth_control_schedulerState_control((void*)noteOnEventListHead,1);
    }
}

static void synth_switch_schedulerState_setup(void)
{
    static mom_state_t mom_state; /* ... */
    static switch_debouncer_t debouncer;
    switch_debouncer_init(&debouncer,fsw1_func,1,&mom_state);
    switch_control_debounce_init(&switch_control[0],(void*)&fsw_debouncers[0]);
    static

