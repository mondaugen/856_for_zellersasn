# Repeatedly send midi commands to start and stop playback
# NOTE: CC is not generated from code so might have to be updated for future
# versions

MIDIDEV=hw:1,0,0

MIDI_CHANNEL=0
CC=61

while [ 1 ];
do
    # start playback
    mididat=`printf "B%X %02X 01" $MIDI_CHANNEL $CC`  
    amidi -p $MIDIDEV -S "$mididat"

    sleep 1

    # stop playback
    mididat=`printf "B%X %02X 00" $MIDI_CHANNEL $CC`  
    amidi -p $MIDIDEV -S "$mididat"

    sleep 1
done

