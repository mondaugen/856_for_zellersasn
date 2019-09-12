/* Copyright (c) 2016 Nicholas Esterer. All rights reserved. */

#ifndef SYNTH_MIDI_CONTROL_H
#define SYNTH_MIDI_CONTROL_H 
#include "synth_control.h" 
#include "midi_setup.h"

#define SYNTH_MIDI_CONTROL_DEFAULT_CHANNEL 0
#define SYNTH_MIDI_NUM_NOTE_PARAMS 18
#define SYNTH_MIDI_NUM_GLOBAL_PARAMS 10
#define SYNTH_MIDI_TOT_NUM_NOTE_FUNS (SYNTH_MIDI_NUM_NOTE_PARAMS \
                                * NUM_NOTE_PARAM_SETS)
#define SYNTH_MIDI_TOT_NUM_FUNS SYNTH_MIDI_NUM_NOTE_PARAMS \
                                * NUM_NOTE_PARAM_SETS \
                                + SYNTH_MIDI_NUM_GLOBAL_PARAMS

void synth_midi_control_setup(int midi_channel);

/* For debugging */
typedef struct synth_midi_control_params_t synth_midi_control_params_t;
const synth_midi_control_params_t ** synth_midi_control_get_midi_cc_controls();
synth_midi_control_params_t * synth_midi_control_get_p_midi_contols_end();


#endif /* SYNTH_MIDI_CONTROL_H */
