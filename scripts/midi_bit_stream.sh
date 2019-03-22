MIDIDEV=hw:1,0,0

MIDI_CHANNEL=0
CC=61
DELAY=0.01

while [ 1 ];
do
    # start playback
    mididat="B2 4B 24"
    amidi -p $MIDIDEV -S "$mididat"

    sleep $DELAY

done

