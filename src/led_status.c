/* Copyright (c) 2016 Nicholas Esterer. All rights reserved. */

#include "scheduling.h"
#include "synth_control.h" 
#include "leds.h"

void led_status_update(void)
{
    SynthControlPosMode pm;
    pm = synth_control_get_posMode_curParams();
    if (pm == SynthControlPosMode_UNI){
        sched_advance_mode_t sam;
        sam = scheduler_get_advance_mode();
        switch (sam) {
            case sched_advance_mode_INTERNAL:
                led1_set();
                led3_reset();
                led5_reset();
                break;
            case sched_advance_mode_MIDI:
                led1_reset();
                led3_set();
                led5_reset();
                break;
        }

    } else {
        if (synth_control_get_recordState()) {
            led1_set();
        } else {
            led1_reset();
        }
        if (synth_control_get_schedulerState()) {
            led3_set();
        } else {
            led3_reset();
        }
        if (synth_control_get_feedbackState()) {
            led5_set();
        } else {
            led5_reset();
        }
    }
}

