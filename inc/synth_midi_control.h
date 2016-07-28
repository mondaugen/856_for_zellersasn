/* Copyright (c) 2016 Nicholas Esterer. All rights reserved. */

#ifndef SYNTH_MIDI_CONTROL_H
#define SYNTH_MIDI_CONTROL_H 
#include "synth_control.h" 
#include "midi_setup.h"

#define SYNTH_MIDI_CONTROL_DEFAULT_CHANNEL 0
#define SYNTH_MIDI_NUM_NOTE_PARAMS 17
#define SYNTH_MIDI_NUM_GLOBAL_PARAMS 10
#define SYNTH_MIDI_TOT_NUM_NOTE_FUNS (SYNTH_MIDI_NUM_NOTE_PARAMS \
                                * NUM_NOTE_PARAM_SETS)
#define SYNTH_MIDI_TOT_NUM_FUNS SYNTH_MIDI_NUM_NOTE_PARAMS \
                                * NUM_NOTE_PARAM_SETS \
                                + SYNTH_MIDI_NUM_GLOBAL_PARAMS

typedef enum {
    synth_midi_cc_type_t_PITCH_FINE1 = 0,
    synth_midi_cc_type_t_PITCH_FINE2,
    synth_midi_cc_type_t_PITCH_FINE3,
    synth_midi_cc_type_t_ENV,
    synth_midi_cc_type_t_SUS,
    synth_midi_cc_type_t_PITCH1,
    synth_midi_cc_type_t_PITCH2,
    synth_midi_cc_type_t_PITCH3,
    synth_midi_cc_type_t_GAIN,
    synth_midi_cc_type_t_POS,
    synth_midi_cc_type_t_STRIDE,
    synth_midi_cc_type_t_OFFSET,
    synth_midi_cc_type_t_FBK_RATE,
    synth_midi_cc_type_t_EVENT_DELTA,
    synth_midi_cc_type_t_NUM_REPS,
    synth_midi_cc_type_t_STRIDE_STATE,
    synth_midi_cc_type_t_INTERM,
    synth_midi_cc_type_t_TEMPO_COARSE 
        = (SYNTH_MIDI_NUM_NOTE_PARAMS * NUM_NOTE_PARAM_SETS),
    synth_midi_cc_type_t_TEMPO_FINE,
    synth_midi_cc_type_t_TEMPO_SCALE,
    synth_midi_cc_type_t_TEMPO_NUDGE,
    synth_midi_cc_type_t_PRESET_STORE,
    synth_midi_cc_type_t_PRESET_RECALL,
    synth_midi_cc_type_t_REC,
    synth_midi_cc_type_t_PLAY,
    synth_midi_cc_type_t_REC_MODE,
    synth_midi_cc_type_t_FBK_STATE
} synth_midi_cc_type_t;

void synth_midi_control_setup(int midi_channel);

#endif /* SYNTH_MIDI_CONTROL_H */
