/* Copyright (c) 2016 Nicholas Esterer. All rights reserved. */

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
#include "synth_midi_control.h" 
#include "startup_polling.h" 

#if defined(RAM_INTEGRITY_TEST) || defined(RAM_INTEGRITY_TEST2) || defined(RAM_INTEGRITY_TEST3)
#include "fmc.h"
static void
debug_ram_integrity_wait(void)
{
    while (1);
}
static void
debug_ram_integrity(void)
{
    uint32_t i;
    uint32_t *ptr = (uint32_t*)SDRAM_BANK_ADDR;
    for (i = 0; i < SDRAM_LENGTH/sizeof(uint32_t); i++) {
        *ptr++ = i;
    }
    /* Just halt here. */
    debug_ram_integrity_wait();
}
static void __attribute__((optimize("O0")))
debug_ram_integrity_read_write(void) 
{
    uint8_t i = 0;
    uint8_t *ptr = (uint8_t*)SDRAM_BANK_ADDR;
    while (1) {
        *ptr = i;
        if (*ptr != i) {
            break;
        }
        ptr++;
        ptr = ptr >= (SDRAM_BANK_ADDR + SDRAM_LENGTH) ? (ptr - SDRAM_LENGTH) : ptr;
        i++;
    }
    debug_ram_integrity_wait();
}
#endif 

#define INITIAL_COUNT 1000000L 

int main (void)
{
#ifdef RAM_INTEGRITY_TEST
    debug_ram_integrity();
#endif 
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
    adc_setup_dma_scan(adc_mode_1SHOT);
    adc_channel_setup();
    synth_adc_control_setup();
    adc_start_conversion();
    int midi_channel = -1;
    int reset_request = 0;
    supo_get_midi_channel(&midi_channel);
    supo_get_preset_reset_rqst(&reset_request);
    sc_presets_init(reset_request,&midi_channel);
    synth_switch_control_setup();
    synth_midi_control_setup(midi_channel);
    audio_start();
#ifdef RAM_INTEGRITY_TEST2
    debug_ram_integrity();
#endif 
#ifdef RAM_INTEGRITY_TEST3
    debug_ram_integrity_read_write();
#endif 
#if defined(TIMER_EVENT_TEST) || defined(TIMER_TEST)
    timers_enable();
#endif
    while(1) {
    }
#endif /* AUDIO_HW_TEST_THROUGHPUT */
    return(0);
}
