/* Copyright (c) 2016 Nicholas Esterer. All rights reserved. */

#include "scheduling.h"
#include "synth_control.h" 
#include "leds.h"

#define FBK_MODE_INDICATOR_PERIOD 0.125
#define FMI_ROLLOVER ((int)(FBK_MODE_INDICATOR_PERIOD / ((float)BUFFER_SIZE/CODEC_SAMPLE_RATE)))

struct fbk_mode_indicator {
    /*
    Stuff for a flashing indicator
    how far we've counted
    */
    int cnt;
    /* at what point to change the indicator state */
    int roll_over;
    /* indicator state */
    int indicator_state;
};

static struct fbk_mode_indicator fbk_mode_indicator = {0,FMI_ROLLOVER,0};

static void
fbk_mode_indicator_update(void)
{
    int fbk_state = synth_control_get_feedbackState();
    switch (fbk_state) {
        case 0:
            led5_reset();
            return;
        case 1:
            led5_set();
            return;
        case 2:
            fbk_mode_indicator.cnt++;
            if (fbk_mode_indicator.cnt >= fbk_mode_indicator.roll_over) {
                fbk_mode_indicator.indicator_state = 1 - fbk_mode_indicator.indicator_state;
                fbk_mode_indicator.cnt = 0;
            }
            if (fbk_mode_indicator.indicator_state) {
                led5_set();
            } else {
                led5_reset();
            }
    }
}

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
        fbk_mode_indicator_update();
    }
}

