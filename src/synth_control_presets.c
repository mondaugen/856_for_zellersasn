#include "synth_control.h" 
#include "presets_lowlevel.h"
#include "switches.h" 
#include <string.h> 


typedef struct __SCPreset {
    NoteParamSet                 noteParamSets[NUM_NOTE_PARAM_SETS];
    MMSample                     tempoBPM; 
    SynthControlPosMode          posMode;
    SynthControlDeltaButtonMode  deltaButtonMode;
    SynthControlPitchMode        pitchMode;
    SynthControlGainMode         gainMode;
} SCPreset;

static presets_lowlevel_handle_t *scpresets_handle;
static SCPreset scpresets[NUM_SYNTH_CONTROL_PRESETS];

/* File is a file in which to store presets. This will obviously depend on the
 * implementation. */
void sc_presets_init(void)
{
    /* (The flash implementation requires no initialization) */
    presets_lowlevel_init(&scpresets_handle,NULL);
    /* If both footswitches down on startup, set presets to default values. This
     * doesn't overwrite what is in flash. That will only happen if the user
     * chooses to save the presets. */
    if ((fsw1_get_state() == 0) && (fsw2_get_state() == 0)) {
        int n;
        for (n = 0; n < NUM_SYNTH_CONTROL_PRESETS; n++) {
            synth_control_reset_param_sets(scpresets[n].noteParamSets,NUM_NOTE_PARAM_SETS);
            scpresets[n].tempoBPM = SYNTH_CONTROL_DEFAULT_TEMPOBPM;
        }
        /* Wait for switches to go high before continuing */
        while ((fsw1_get_state() == 0) || (fsw2_get_state() == 0));
    } else {
        presets_lowlevel_read(scpresets_handle,(void*)scpresets,
            sizeof(scpresets),NULL);
    }
}

void sc_presets_store(int npreset)
{
    memcpy(scpresets[npreset].noteParamSets,noteParamSets,
            sizeof(NoteParamSet)*NUM_NOTE_PARAM_SETS);
    scpresets[npreset].tempoBPM = tempoBPM; 
    /* Store every time, because program could be terminated at any moment */
    presets_lowlevel_write(scpresets_handle,(void*)scpresets,
            sizeof(scpresets),NULL);
}

void sc_presets_recall(int npreset)
{
    /* No need to load from file, this is done only on initialization */
    memcpy(noteParamSets,scpresets[npreset].noteParamSets,sizeof(NoteParamSet)*NUM_NOTE_PARAM_SETS);
    if (schedulerState == 0) {
        /* Only recall tempo if sequencer not playing */
        tempoBPM = scpresets[npreset].tempoBPM; 
    }
}
