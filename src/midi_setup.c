#include "midi_setup.h" 

MIDI_Router_Standard midiRouter;

int midi_setup(void *data)
{
    midi_hw_setup_t mhs;
    mhs.device_in = (char*)data;
    mhs.verbose = 0;
    return(midi_hw_setup(&mhs));
}

void midi_hw_process_msg(MIDIMsg *msg)
{
    MIDI_Router_handleMsg(&midiRouter.router, msg);
}
