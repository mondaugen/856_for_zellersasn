sox -r 48000 -t alsa hw:CARD=Pro,DEV=1 -t f64 -c 2 -r 48000 \
    /tmp/tmp.XXXX trim 0 1;
# This has to be opened yourself because octave just closes after plotting
#octave -f idle_noise_test.m;


