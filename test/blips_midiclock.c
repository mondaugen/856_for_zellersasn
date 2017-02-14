#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <jack/jack.h>
#include <jack/midiport.h>

#define N_NOTE_FREQS 1 
#define CLIENT_NAME "blips" 

jack_port_t *output_port;
jack_port_t *output_midi_port;

jack_default_audio_sample_t ramp=0.;
jack_nframes_t frame_accum = 0;
jack_nframes_t mclk_period = 1000;
jack_nframes_t mclk_cnt = 0;

/* Sample period in seconds */
jack_default_audio_sample_t Ts=1.;
/* Sample rate in Hz */
jack_nframes_t Fs=1;

/* number of MIDI clocks per blip period */
int mcp=24;
int mcp_scale=1;


jack_default_audio_sample_t note_on;
unsigned char note = 69;
jack_default_audio_sample_t note_frqs[N_NOTE_FREQS];

struct pet_synth {
    jack_default_audio_sample_t cur;
    jack_default_audio_sample_t duty;
    jack_default_audio_sample_t period;
} ps;

jack_default_audio_sample_t pet_synth_get_sample_inc(struct pet_synth *ps,
                                                     jack_default_audio_sample_t inc)
{
    if (ps->cur > ps->duty) {
        return 0.;
    }
    ps->cur += inc;
    return 1.;
}

jack_default_audio_sample_t pet_synth_reset(struct pet_synth *ps)
{
    ps->cur = 0.;
}

void calc_note_frqs(jack_default_audio_sample_t srate)
{
    int i;
    for(i=0; i<128; i++)
    {
        note_frqs[i] = (2.0 * 440.0 / 32.0) * pow(2, (((jack_default_audio_sample_t)i - 9.0) / 12.0)) / srate;
    }
}

int process(jack_nframes_t nframes, void *arg)
{
    int i;
    jack_default_audio_sample_t *out = (jack_default_audio_sample_t *) jack_port_get_buffer (output_port, nframes);
    void* port_buf = jack_port_get_buffer(output_midi_port, nframes);
	jack_midi_clear_buffer(port_buf);
    /* Get time of this frame */
    jack_nframes_t cur_time = jack_last_frame_time((jack_client_t*)arg);
    /* Fill buffer with sine */
    for(i=0; i<nframes; i++)
    {
        ramp += note_frqs[note];
        ramp = (ramp > 1.0) ? ramp - 2.0 : ramp;
        out[i] = sin(2*M_PI*ramp);
    }
    i = 0;
    while (frame_accum < nframes) {
        if (mclk_cnt != 0) {
            /* Zero this section of buffer, we want buffer to only output
             * from the time of the first midi clock to the second every 24 MIDI
             * clocks */
            while (i < frame_accum) {
                out[i++]=0;
            }
        } else {
            i = frame_accum;
        }
        mclk_cnt += 1;
        mclk_cnt = (mclk_cnt >= 24) ? 0 : mclk_cnt;
        /* Pass off MIDI event */
//        jack_midi_data_t midi_data[] = {0x90,60,100};
        jack_midi_data_t *midi_data = jack_midi_event_reserve(port_buf, frame_accum, 1);
        midi_data[0]=0xf8;
//        jack_midi_event_write(port_buf, cur_time, midi_data, 3);
//        jack_midi_event_write(port_buf, cur_time + frame_accum, midi_data, 1);
        frame_accum += mclk_period;
    }
    if (mclk_cnt != 0) {
        while (i < nframes) {
            out[i++]=0;
        }
    }
    frame_accum -= nframes;
    return 0;      
}

int srate(jack_nframes_t nframes, void *arg)
{
    printf("the sample rate is now %" PRIu32 "/sec\n", nframes);
    Ts = 1./(jack_default_audio_sample_t)nframes;
    Fs = nframes;
    mclk_period = Fs / mcp * mcp_scale;
    calc_note_frqs((jack_default_audio_sample_t)nframes);
    return 0;
}

void jack_shutdown(void *arg)
{
    exit(1);
}

int main(int narg, char **args)
{
    jack_client_t *client;

    if ((client = jack_client_open (CLIENT_NAME, JackNullOption, NULL)) == 0)
    {
        fprintf(stderr, "jack server not running?\n");
        return 1;
    }

//    ps = {0.,0.1,1.0};
    
    calc_note_frqs(jack_get_sample_rate (client));

    jack_set_process_callback (client, process, (void*)client);

    jack_set_sample_rate_callback (client, srate, 0);

    jack_on_shutdown (client, jack_shutdown, 0);

    output_port = jack_port_register (client, "audio_out", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    output_midi_port = jack_port_register (client, "midi_out", JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0);

    if (jack_activate (client))
    {
        fprintf(stderr, "cannot activate client");
        return 1;
    }

    /* run until interrupted */
    while(1)
    {
        sleep(1);
    }
    jack_client_close(client);
    exit (0);
}
