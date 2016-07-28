/* Copyright (c) 2016 Nicholas Esterer. All rights reserved. */

#include "synth_control.h" 
#include "presets_lowlevel.h"
#include "switches.h" 
#include <string.h> 
#include "leds.h" 
#include "synth_midi_control.h" 

typedef struct __SCPreset {
    NoteParamSet                 noteParamSets[NUM_NOTE_PARAM_SETS];
    MMSample                     tempoBPM; 
} SCPreset;

typedef struct __SCStorage {
    /* If not equal to a special keyword, flash was either corrupted or never written */
    uint32_t first_read; 
    SCPreset scpresets[NUM_SYNTH_CONTROL_PRESETS];
    int midi_channel;
} SCStorage;

static presets_lowlevel_handle_t *scpresets_handle;
static SCStorage scstorage;

/* File is a file in which to store presets. This will obviously depend on the
 * implementation.
 * If reset_request is 1, then presets in SRAM overwritten with default
 * values. The default values must be explicitly stored (e.g., by pressing the store
 * button).
 * If reset_request is 2, then presets are overwritten as with 1 but then also
 * saved into flash .
 * If midi_channel is non-negative, then this will be stored as the new default
 * midi channel, otherwise the last default will be loaded. */
void sc_presets_init(int reset_request, int *midi_channel)
{
    /* (The flash implementation requires no initialization) */
    presets_lowlevel_init(&scpresets_handle,NULL);
    presets_lowlevel_read(scpresets_handle,(void*)&scstorage,
            sizeof(scstorage),NULL);
    /* Check if keyword present, if not, flash is fresh and needs to be
     * initialized. */
    if (scstorage.first_read != SCP_FIRST_READ_KW) {
        reset_request = 2;
        scstorage.first_read = SCP_FIRST_READ_KW;
        scstorage.midi_channel = SYNTH_MIDI_CONTROL_DEFAULT_CHANNEL;
    }
    if (*midi_channel >= 0) {
        scstorage.midi_channel = *midi_channel;
        /* Store new midi_channel */
        if (reset_request != 2) {
            presets_lowlevel_write(scpresets_handle,(void*)&scstorage,
                    sizeof(scstorage),NULL);
        }
    } else {
        *midi_channel = scstorage.midi_channel;
    }
    /* If both footswitches down on startup, set presets to default values. This
     * doesn't overwrite what is in flash. That will only happen if the user
     * chooses to save the presets. */
    if (reset_request > 0) {
        int n;
        for (n = 0; n < NUM_SYNTH_CONTROL_PRESETS; n++) {
            synth_control_reset_param_sets(scstorage.scpresets[n].noteParamSets,NUM_NOTE_PARAM_SETS);
            scstorage.scpresets[n].tempoBPM = SYNTH_CONTROL_DEFAULT_TEMPOBPM;
        }
    }
    if (reset_request == 2) {
        presets_lowlevel_write(scpresets_handle,(void*)&scstorage,
                sizeof(scstorage),NULL);
    }
}

void sc_presets_store(int npreset)
{
    if (npreset < 0) {
        npreset = 0;
    }
    if (npreset > NUM_SYNTH_CONTROL_PRESETS) {
        npreset = NUM_SYNTH_CONTROL_PRESETS - 1;
    }
    memcpy(scstorage.scpresets[npreset].noteParamSets,noteParamSets,
            sizeof(NoteParamSet)*NUM_NOTE_PARAM_SETS);
    scstorage.scpresets[npreset].tempoBPM = synth_control_get_tempoBPM(); 
    /* Store every time, because program could be terminated at any moment */
    presets_lowlevel_write(scpresets_handle,(void*)&scstorage,
            sizeof(scstorage),NULL);
}

void sc_presets_recall(int npreset)
{
    if (npreset < 0) {
        npreset = 0;
    }
    if (npreset > NUM_SYNTH_CONTROL_PRESETS) {
        npreset = NUM_SYNTH_CONTROL_PRESETS - 1;
    }
    /* No need to load from file, this is done only on initialization */
    memcpy(noteParamSets,scstorage.scpresets[npreset].noteParamSets,sizeof(NoteParamSet)*NUM_NOTE_PARAM_SETS);
    /* Reset event counts so intermittency off all notes always has same phase
     * */
    synth_control_reset_noteOnEventCounts();
    if (schedulerState == 0) {
        /* Only recall tempo if sequencer not playing */
        synth_control_set_tempoBPM_absolute(scstorage.scpresets[npreset].tempoBPM); 
    }
}
