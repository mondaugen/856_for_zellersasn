#include "err.h" 
#include "audio_setup.h"
#include "midi_setup.h" 
#include "wavetables.h" 
#include "signal_chain.h" 
#include <unistd.h> 

void play_note(int midinote)
{
    MMTrapEnvedSamplePlayer_noteOn(
        &spsps[0],
        midinote,
        1,
        MMInterpMethod_CUBIC,
        0,
        1.,
        4.,
        10.,
        &WaveTable,
        1);
}

int main (int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr,
                "Arguments are %s audio-device midi-input-port\n",argv[0]);
        return(-1);
    }
    if (audio_setup(argv[1])) {
        THROW_ERR("Error setting up audio.");
    }
    if (midi_setup(argv[2])) {
        THROW_ERR("Error setting up MIDI.");
    }
    SampleTable_init();
    signal_chain_setup();
    poly_management_setup();
    synth_control_setup();
    audio_start();
//    sleep(2);
//    play_note(60);
    while(1) {
    }
    return(0);
}
