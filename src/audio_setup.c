#include "audio_setup.h" 
#include "midi_setup.h" 
#include "signal_chain.h" 
#include <math.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <stdint.h> 
#include "synth_control.h" 
#include "scheduling.h" 

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
    /* Process MIDI once every audioblock */
    midi_hw_process_input(NULL);
    /* Increment scheduler and do pending events */
    scheduler_incTimeAndDoEvents();
    /* Process audio */
    MMSigProc_tick(&sigChain);
    int n, c;
    for (n = 0; n < params->length; n++) {
        /* Only the first channel is written/read */
        params->out[n*params->nchans_out] =
            outBus->data[outBus->channels*n] * AUDIO_HW_SAMPLE_T_MAX;
        inBus->data[n*params->nchans_out] = 
            ((MMSample)params->in[n*params->nchans_out])
            /AUDIO_HW_SAMPLE_T_MAX;
        /* Mix some dry signal into the output */
        params->out[n*params->nchans_out] 
            += ((int16_t)((((MMSample)params->in[n*params->nchans_out])
                            /AUDIO_HW_SAMPLE_T_MAX) * AUDIO_HW_SAMPLE_T_MAX))
            * dryGain / 127;
        /* The rest of the channels are 0 */
        /* 
        for (c = 1; c < params->nchans_out; c++) {
            params->out[n*params->nchans_out + c] = 0;
            inBus->data[n*params->nchans_out + c] = 0;
        }
        */
    }
}
