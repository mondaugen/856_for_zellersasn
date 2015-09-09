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

int audio_ready = 0;

/* Pass in the name of the audio device as a string */
int audio_setup(void *data)
{
    audio_hw_setup_t ahs;
    ahs = CODEC_SAMPLE_RATE;
    return audio_hw_setup(&ahs);
}

void audio_hw_io(audio_hw_io_t *params)
{
#ifdef AUDIO_HW_TEST_THROUGHPUT
    int n;
    for (n = 0; n < params->length; n++) {
        params->out[n*params->nchans_out] =
            params->in[n*params->nchans_in];
    }
#else
    /* Process switches. MIDI trumps switches if messages present */
    switch_control_do_all();
    /* Process MIDI once every audioblock */
    midi_hw_process_input(NULL);
    /* Increment scheduler and do pending events */
    scheduler_incTimeAndDoEvents();
    /* Process audio */
    MMSigProc_tick(&sigChain);
    int n;
    for (n = 0; n < params->length; n++) {
        /* Only the first channel is written/read */
        params->out[n*params->nchans_out] =
            outBus->data[outBus->channels*n] * AUDIO_HW_SAMPLE_T_MAX;
        inBus->data[n*inBus->channels] = 
            ((MMSample)params->in[n*params->nchans_in])
            /AUDIO_HW_SAMPLE_T_MAX;
        /* Mix some dry signal into the output */
        params->out[n*params->nchans_out] 
            += ((int16_t)((((MMSample)(params->in[n*params->nchans_in]))
                            /AUDIO_HW_SAMPLE_T_MAX) * AUDIO_HW_SAMPLE_T_MAX))
            * dryGain / 127;
    }
#endif /* AUDIO_HW_TEST_THROUGHPUT */
}
