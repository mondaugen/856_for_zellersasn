#include "err.h" 
#include "audio_setup.h"
#include "midi_setup.h" 
#include "wavetables.h" 
#include "signal_chain.h" 
#include "synth_control.h" 
#include "synth_control_presets.h" 
#include "scheduling.h" 
#include "leds.h" 

void play_note_rate(int midinote, float rate)
{
    MMTrapEnvedSamplePlayer_noteOnStruct no;
    no.note = midinote;
    no.amplitude = 1;
    no.index = 0;
    no.attackTime = 1.;
    no.releaseTime = 4.;
    no.sustainTime = 10.;
    no.samples = theSound.wavtab;
    no.rate = rate;
    MMTrapEnvedSamplePlayer_noteOn_Rate(
        &spsps[0],&no);
}

void play_note(int midinote)
{
    MMTrapEnvedSamplePlayer_noteOnStruct no;
    no.note = midinote;
    no.amplitude = 1;
    no.index = 0;
    no.attackTime = 1.;
    no.releaseTime = 4.;
    no.sustainTime = 10.;
    no.samples = theSound.wavtab;
    MMTrapEnvedSamplePlayer_noteOn(
        &spsps[0],&no);
}

int main (void)
{
#ifdef AUDIO_HW_TEST_THROUGHPUT 
    if (audio_setup(NULL)) {
        THROW_ERR("Error setting up audio.");
    }
    audio_start();
    while(1) {
    }
#else
    if (audio_setup(NULL)) {
        THROW_ERR("Error setting up audio.");
    }
    if (midi_setup(NULL)) {
        THROW_ERR("Error setting up MIDI.");
    }
    leds_setup();
    SampleTable_init();
    sc_presets_init();
    signal_chain_setup();
//    poly_management_setup();
    synth_control_setup();
    scheduler_setup();
    audio_start();
    while(1) {
    }
#endif /* AUDIO_HW_TEST_THROUGHPUT */
    return(0);
}
