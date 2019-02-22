/* Copyright (c) 2016 Nicholas Esterer. All rights reserved. */

#include "audio_setup.h" 
#include "midi_setup.h" 
#include "signal_chain.h" 
#include <math.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <stdint.h> 
#include "synth_control.h" 
#include "scheduling.h" 
#include "switch_control.h" 
#include "adc_channel.h" 
#include "led_status.h" 

#define CODEC_LIVE_INPUT_CHANNEL 0
#define CODEC_LIVE_OUTPUT_CHANNEL 0

int audio_ready = 0;

float max_val = 0;

/* Pass in the name of the audio device as a string */
int audio_setup(void *data)
{
    audio_hw_setup_t ahs;
    ahs = CODEC_SAMPLE_RATE;
    return audio_hw_setup(&ahs);
}

static void track_max_val(float f)
{
    float f_ = fabsf(f);
    if (f_ > max_val) { max_val = f_; }
}

static void saturate_output(audio_hw_io_t *params)
{
    int n;
    for (n = 0; n < params->length; n++) {
        /* saturate output */
        track_max_val(outBus->data[outBus->channels*n]);
        if (outBus->data[outBus->channels*n] > 1.) {
            outBus->data[outBus->channels*n] = 1.;
        } else if (outBus->data[outBus->channels*n] < -1.) {
            outBus->data[outBus->channels*n] = -1.;
        }
    }
}

void audio_hw_io(audio_hw_io_t *params)
{
    int n;
#ifdef AUDIO_HW_TEST_THROUGHPUT
#if defined(BOARD_V1)
    for (n = 0; n < params->length; n++) {
        params->out[n*params->nchans_out+CODEC_LIVE_OUTPUT_CHANNEL] =
            params->in[n*params->nchans_in+CODEC_LIVE_INPUT_CHANNEL];
    }
#elif defined(BOARD_V2)
    for (n = 0; n < params->length; n++) {
        params->out[n*params->nchans_out+CODEC_LIVE_OUTPUT_CHANNEL] =
            params->in[n*params->nchans_in+CODEC_LIVE_INPUT_CHANNEL];
    }
#endif
#elif defined(AUDIO_HW_TEST_OUTPUT)
#if defined(AUDIO_HW_TEST_OUTPUT_RAMP)
    static int16_t _rampval = 0;
    for (n = 0; n < params->length; n++) {
        params->out[n*params->nchans_out+CODEC_LIVE_OUTPUT_CHANNEL] = (int16_t)_rampval;
        params->out[n*params->nchans_out+CODEC_LIVE_OUTPUT_CHANNEL+1] = (int16_t)(_rampval+=1000);
    }
#else
    /* Generate square wave in bits */
    for (n = 0; n < params->length; n++) {
        params->out[n*params->nchans_out+CODEC_LIVE_OUTPUT_CHANNEL] = (int16_t)0xff00;
        params->out[n*params->nchans_out+CODEC_LIVE_OUTPUT_CHANNEL+1] = (int16_t)0xff00;
    }
#endif
#else
    /* Process switches. MIDI trumps switches if messages present */
    switch_control_do_all();
    /* Process knobs. MIDI trumps knobs if messages present. */
    if (adc_get_adc_ready()) {
        adc_channels_update();
        adc_channel_do_all_sets();
        adc_clear_adc_ready();
        adc_start_conversion();
    }
    /* Process MIDI once every audioblock */
    midi_hw_process_input(NULL);
    /* Update LEDs */
    led_status_update();
    /* Increment scheduler and do pending events */
    scheduler_incTimeAndDoEvents();
    /* Process audio */
    MMSigProc_tick(&sigChain);
    saturate_output(params);
    for (n = 0; n < params->length; n++) {
        /* Only the first channel is written/read */
#if defined(BOARD_V1)
        params->out[n*params->nchans_out+CODEC_LIVE_OUTPUT_CHANNEL] =
            outBus->data[outBus->channels*n] * AUDIO_HW_SAMPLE_T_MAX;
        inBus->data[n*inBus->channels] = 
            ((MMSample)params->in[n*params->nchans_in+CODEC_LIVE_INPUT_CHANNEL])
            /AUDIO_HW_SAMPLE_T_MAX;
#elif defined(BOARD_V2)
#if defined(AUDIO_HW_TEST_WET_DRY_MIX)
        params->out[n*params->nchans_out+CODEC_LIVE_OUTPUT_CHANNEL] =
            params->in[n*params->nchans_in+CODEC_LIVE_INPUT_CHANNEL];
#else
#if defined(AUDIO_HW_WET_DRY_MIX_SOFTWARE)
        params->out[n*params->nchasrc/audio_setup.cns_out+CODEC_LIVE_OUTPUT_CHANNEL] = __SADD16(
            outBus->data[outBus->channels*n] * AUDIO_HW_SAMPLE_T_MAX,
            params->in[n*params->nchans_in+CODEC_LIVE_INPUT_CHANNEL]);
#else
        params->out[n*params->nchans_out+CODEC_LIVE_OUTPUT_CHANNEL] =
            outBus->data[outBus->channels*n] * AUDIO_HW_SAMPLE_T_MAX;
#endif
#endif
        inBus->data[n*inBus->channels] = 
            ((MMSample)params->in[n*params->nchans_in+CODEC_LIVE_INPUT_CHANNEL])
            /AUDIO_HW_SAMPLE_T_MAX;
#endif
    }
#endif /* AUDIO_HW_TEST_THROUGHPUT */
}
