#include "err.h" 
#include "audio_setup.h"
#include "midi_setup.h" 
#include "wavetables.h" 
#include "signal_chain.h" 
#include "synth_control.h" 
#include "synth_control_presets.h" 
#include "scheduling.h" 
#include "leds.h" 
#include "switches.h" 
#include "adc.h" 
#include "leds.h" 
#include "switch_control.h" 
#include "synth_switch_control.h" 
#include "adc_channel.h" 
#include "synth_adc_control.h" 

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

#define INITIAL_COUNT 1000000L 

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
    switches_setup();
    adc_setup_dma_scan();
    adc_channel_setup();
    synth_adc_control_setup();
    SampleTable_init();
    /* Preset init must be before switch control setup so that switch toggling
     * to recall default presets doesn't screw up the synth switch control. */
    sc_presets_init();
    synth_switch_control_setup();
    signal_chain_setup();
    synth_control_setup();
    scheduler_setup();
    audio_start();
    while(1) {
    }
#endif /* AUDIO_HW_TEST_THROUGHPUT */
    return(0);
}
