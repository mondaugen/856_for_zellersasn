/* Copyright (c) 2016 Nicholas Esterer. All rights reserved. */

#ifndef SYNTH_CONTROL_PRESETS_H
#define SYNTH_CONTROL_PRESETS_H 

#define NUM_SYNTH_CONTROL_PRESETS 3 
#define SCP_FIRST_READ_KW 0x42069A55
void sc_presets_init(int reset_request, int *midi_channel);
void sc_presets_store(int npreset);
void sc_presets_recall(int npreset);

#endif /* SYNTH_CONTROL_PRESETS_H */
