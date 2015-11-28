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
#include "timers.h" 

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
    SampleTable_init();
    signal_chain_setup();
    synth_control_setup();
    scheduler_setup();
    leds_setup();
    timers_setup();
    switches_setup();
    adc_setup_dma_scan();
    adc_channel_setup();
    synth_adc_control_setup();
    /* Preset init must be before switch control setup so that switch toggling
     * to recall default presets doesn't screw up the synth switch control. */
    sc_presets_init();
    synth_switch_control_setup();
    audio_start();
#if defined(TIMER_EVENT_TEST) || defined(TIMER_TEST)
    timers_enable();
#endif
    while(1) {
    }
#endif /* AUDIO_HW_TEST_THROUGHPUT */
    return(0);
}
