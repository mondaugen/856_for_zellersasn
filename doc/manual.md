# The 856 for ZELLERSASN Manual

## Introduction

The 856 for ZELLERSASN (856) is a looper and sample sequencer. Samples are taken
of different parts of a loop and played back in various rhythms and at different
pitches. A designed sequence can be stored and can be applied to new recordings
(loops).

## Particularities

The knobs give the user the ability to design sequences. This should be kept in
mind when adjusting the knobs: what you adjust may not immediately be audible as
you are setting a parameter that will be changed the next time a sample plays.

Recording works as follows: press and release the left footswitch to start
recording. Press and release it again to stop.

You can only record for a maximum of 20 seconds. After this time, the recording
will automatically stop.

## Terminology

When we say “parameter”, we refer to the value that a particular
knob represents. For example, the “pitch” knob controls the pitch parameter.

When we say “click a switch” in this manual, we mean press and release a switch.
The time between pressing and releasing should be short, but it actually doesn't
matter if nothing else changes while the switch is down (no knob is turned,
etc.).

## Setting up

The pedal is on moments after it is plugged in (it takes a few milliseconds to
boot up). The power socket on the back is for a 9 volt adapter with a negative
tip and a positive shell (this is standard for most guitar pedals). The 856
can pull up to 300 mA and so requires a power supply that can supply at least
that much current.

Looking from the top, the 1/4 inch jack furthest to the right is the audio input
and the one just to the left of it the audio output.

The jack just to the left of the audio output jack is for incoming MIDI
messages.

## Tutorial

Before going through all the controls on the 856, we'll take you through a
tutorial to get a flavour for what can be done with it.

See the “setting up” section to set up the pedal before proceeding.

To reset all the parameters hold the STRIDE/ABS/UNI switch in the down
position while clicking the FADE/GAIN/FBK switch in the downward direction
twice.

Set the controls to control note set 1 by putting the N1/N2/N3 switch in the
upward position.

Put the pedal in R=B mode by putting the FREE/R=B/AREC switch in the middle
position.

The rest of the switches can stay as they are for now.

Play a loop you are interested in recording. The first time we recommend just
playing a note and letting it sound, then recording it for 1 or 2 seconds —
what is going on will be clearer this way. Click the left footswitch and to
start recording. Click the switch to stop. The loop you played should now be
playing.

As the N1/N2/N3 switch is in the upward position and we are in A=R mode, the
TMPO/REP knob only slightly changes the tempo. This is useful for adjusting your
loop if it wasn't quite recorded right. The &#x394; knob controls the tempo more
coarsely if the FREE/QUANT/SKIP switch is in the upward position. If in the
middle position, it doubles, triples or quadruples the tempo depending on the
&#x394; knob's position. This knob turned all the way clockwise does no scaling
to the tempo, it turned all the way counter-clockwise quadruples the tempo.

With the FADE/GAIN/FBK switch in the middle position, turn the GAIN knob all
the way counter-clockwise. This will turn down the note all the way, turning it
off. Put the N1/N2/N3 switch in the middle position. You are now controlling
the second note set. Turn the LEN knob to about half, the TMPO/REP knob to about
half. Set the FREE/QUANT/SKIP to the middle position and turn the &#x394; knob
to the middle. Now turn up the GAIN knob to maximum. You should hear many
repeated copies of the beginning of your loop. Try adjusting the TMPO/REP knob
to add fewer or more copies. Try adjusting the &#x394; knob to change the time
between the repeats.

You can choose to sample a different part of your loop. To do this, set the
STRIDE/ABS/UNI knob in the middle position and move the POS knob. You will
hear the part of the loop that is sampled change based on where the POS knob is.

The pitch of the repeats can be controlled. Set the TMPO/REP knob to maximum to
get a lot of repeats. We can control the pitches of sub-sets of the repeated
notes. The P1/P2/P3 switch set in the upward position allows controlling the
pitch of the 1st playback and 3rd, 6th ... repeats. This switch in the middle
position controls the pitch of the 1st, 4th ... repeats and in the bottom
position controls the pitch of the 2nd, 5th ... repeats. With the PITCH knob set
to a different position for each of the P1/P2/P3 switch settings, you should
hear a sort of arpeggio in the repeats (or chord if the time between the repeats
is short).

You can make the repeats fade out or fade in by setting the FADE/GAIN/FBK
switch to the upward position. The GAIN knob turned counter-clockwise will cause
the repeats to fade out, there will be no fading if it is in the middle and the
repeats will fade in if the knob is turned to the right. For fading in to be
audible, the overall GAIN should not be maximum as the fading in is stopped when
the notes would clip. To experience this, set the FADE/GAIN/FBK switch to the
middle position and turn the GAIN knob to the middle. Then set the
FADE/GAIN/FBK switch back to the top position and turn the GAIN knob to the
right. You should hear the notes fade in.

Our original loop is still present, just turned down. To hear it along with what
you've set up on note 2, set the N1/N2/N3 switch to the top position, set
the FADE/GAIN/FBK switch to the middle position and turn the GAIN knob to the
top (if it is already at the top, turn the knob down then up again). You should
hear the original loop with the repeats you created on note 2 playing at the
same time as the beginning of the loop. We can change when these repeats occur
relative to note 1. Set the N1/N2/N3 switch to the middle position. Now
adjust the OFST knob. You should hear the repeats of note 2 starting sometime
within the loop playing on note 1 -- when OFST is all the way counter-clockwise,
the repeats start at the beginning of the loop and all the way clockwise they
start at the end (which is close to the beginning as it's a loop).

If you like what you've created and want to apply it to other loops, you can
save the parameters by setting the PRE1/PRE2/PRE3 switch and clicking the
STR-RCL switch upwards. To recall the setting later select the preset with the
PRE1/PRE2/PRE3 switch and click the STR-RCL downwards.

## Controls

### Left footswitch

Starts and stops recording. The command is acknowledged as soon as the switch is
pressed down.

### Right footswitch

Starts and stops playback. The command is acknowledged as soon as the switch is
pressed down. Note that playback is disabled if no recording has been made.

### ENV

Controls the amplitude shape of a playing note. Turned counter-clockwise or clockwise,
produces an amplitude envelope with fast attack and fast decay. In between these two settings the attack and decay times vary as shown in the following chart:

![Envelope time at ENV knob positions](env_ramp.png)

Note that the envelope also applies to a loop. If in the middle position for
example, this will give long attacks and decays to your loop, which you may or
may not want. When recording a loop in R=B mode, the loop is automatically given
an envelope with short attack and decay. This is because when recording a small
ramp is applied to the beginning and end of the recording anyways in order to make it smoothly
fade in and out. So if LEN is fully clockwise PITCH is unison and ENV is fully
counter-clockwise, you will still hear a very small ramp at the beginning and
end. This is to make the looper functionality pleasant sounding with less need
for intervention from the user to make the loop end-points sound smooth.

### N1/N2/N3

Controls what note set you are controlling. In the upward position, controls
N1 (we say “BAR” because TMPO/REP, FREE/QUANT/SKIP and &#x394; behave
different from N2 and N3 in this setting). In the middle and lower positions
controls N2 and N3 respectively.

### LEN

Controls the length of the notes samples from your recording. Fully clockwise
plays for the time of the whole loop (we say time of the whole loop because if
PITCH were set to 1 octave above, it would play through the loop twice as it is
playing at double the speed). This knob set in the middle is not half the loop
because the accuracy with which you can choose the length increases as you turn
the knob counter-clockwise. This is so you can accurately cut out small segments
from your loop, like a note or a specific drum, but choose the length of longer
samples less precisely. You can't have your cake and eat it with such a small
knob! (For more accuracy, check out the MIDI controls.)

### FREE/R=B/AREC

Determines what happens when you stop recording.

In FREE mode your recording does not affect the current tempo (see TMPO/REP for
how to choose the tempo).

In R=B mode the length of your recording adjusts the tempo so that the notes of
N1 are played with a delay equal to the length of your recording. This
effectively creates a looping sensation. If the FBK light is flashing (see
FADE/GAIN/FBK) then the gain of N2 and N3 is turned down so they do not play.
This is so you can record what the 856 is playing and have it play like a loop
without the sampling of the other notes making it sound chaotic. As a
side-effect, the timing determined by the &#x394; and OFST knobs are all based
on the length determined by the length of your recording. 

AREC mode is the same as R=B mode except that after you have recorded a loop,
the 856 records automatically again and again for the length that you recorded
the first time. This allows you to apply your sequence to what you just played.
If you have recorded a loop in R=B mode and would like to record for exactly its
length, while the loop is playing in R=B mode, switch to AREC mode, then press
and release the REC button. Recording might not happen immediately but will
start when the start of the loop happens then restart at the end of the loop. If
you want to keep what is currently playing, press and release the REC button.
Like FREE mode, if the FBK light is constant or flashing the gains of N2 and N3
are still preserved.

### TMPO/REP

This knob serves two functions: when N1 is selected it controls the tempo of
the loop or sequence. When N2 or N3 are selected, it determines the number of
repeats that are played of that note. This means repeats are not available for
the first note set.

When N1 is selected, the knob's function changes depending on the position
of FREE/A=B/AREC. When FREE/R=B/AREC is in FREE mode, the knob controls the
tempo so that in fully counter-clockwise position, the there are 11 loops per
minute (11 playings of N1) and in fully clockwise position, there are 60 loops
per minute. This can be finely adjusted ±10% with the &#x394; knob.

In R=B mode the knob slightly nudges the tempo: turning clockwise makes it
slightly faster and counter-clockwise slightly slower.

In AREC mode it behaves the same as in R=B mode.

### FREE/QUANT/SKIP

This switch controls the function of the &#x394; knob. The functions available
change depending on the N1/N2/N3 switch. The following describes the various
selections depending on whether or not the N1 mode is selected or N2 or N3
is selected.

#### FREE

If N1 selected, &#x394; controls the tempo so that if &#x394; is fully
counter-clockwise the tempo is 10% slower and clockwise 10% faster. A sort of
fine tempo adjustment.

If N2 or N3 selected, &#x394; controls the amount of time between the repeats of
the notes. &#x394; in fully clockwise position, the time between repeats is
equal to the length of the loop, &#x394; in fully counter-clockwise position,
the time is 1/64 the length of the loop. Note that after reset (see UNI) the time
between repeats is 0 until the &#x394; knob is adjusted.

#### QUANT

If N1 is selected, turning the &#x394; knob counter-clock wise from centre
multiplies the tempo by 1/2, 1/3, or 1/4 (1/4 at the extreme counter-clockwise
position, i.e., the notes play 4 times _less_ frequently). Turning the other way
multiplies the tempo by 2, 3, or 4. (4 in the fully clockwise position). When
the knob is in the middle no tempo scaling occurs.

To reset the tempo scaling to 1, you can also switch to FREE or SKIP and then
back to QUANT while N1 is selected. In other words, when QUANT is switched into
when N1 is selected, the tempo scaling is reset to 1.

If N2 or N3 are selected, &#x394; controls the time between repeats as fractions
of the loop length. From counter-clockwise to clockwise position, the fractions
are from 1/16, through 1/12, 1/8, 1/6, 1/4, 1/3, 1/2, to 3/4.

#### SKIP

This behaves the same for N1, N2 and N3. It determines how many loops to
wait until playing the set of notes again. Each note set can be set
individually. This is useful to play different note sets with different
densities as sometimes your sequences might get too cluttered. With &#x394; in the
fully counter-clockwise position, the note set is played every loop. In the
fully clockwise position, for every 4 loops, the note set plays only 1 time. In
between more frequent choices are possible, like once every 2 or 3 loops.

### &#x394;

Controls many things. See the other controls for clarification.

### P1/P2/P3

Controls what pitch in the repeats the PITCH knob affects. P1 means PITCH
changes the pitch of the 1st, 4th, etc. notes. P2 the 2nd, 5th etc. notes and so
on.

### STRIDE/ABS/UNI

If in the central position, ABS, the POS knob controls absolutely where samples
are taken from in the recording. If the POS knob is in the fully
counter-clockwise position, samples are taken from the beginning, and as
advanced clockwise, samples are taken from the end (playback wraps around to the
beginning so in the far clockwise position, you are effectively starting
playback at the beginning again).

If in the upward position, STRIDE, the POS knob controls the rate at which the
sample selection is advanced through positions in the sample every time a note
is played. A counter-clockwise position of the POS knob advances backward and a
clockwise position of the POS knob advances forward. The knob in the centre does
not advance the start position of sampling.

With UNI held down, adjusting POS controls the rate at which sample selection is
advanced each repeat (so REP must be turned up to hear this effect). The repeat
stride is separate from the note stride and the two strides are added together
to determine the starting point.

To reset the stride point, hold down UNI and press FBK once.

#### UNI

If the STRIDE/ABS/UNI switch is clicked down without adjusting any other controls
while the switch is down, the pitches of the repeated notes (previously set via
P1/P2/P3 and PITCH) are reset to unison, i.e., they are not transposed.

If held down while adjusting the PITCH knob, the pitches of all the repeats for
the currently selected note set are adjusted simultaneously. This is useful if
you would like to have repeated notes at a different pitch, but no arpeggio
effect.

If held down while adjusting the &#x394; knob when N2 or N3 is selected,
controls swing applied to the rhythm that separates the repeats. Putting the
knob in the middle position gives no swing.

If held down and the FBK switch is clicked once (the bottom position of the
FADE/GAIN/FBK switch), the STRIDE setting is reset to 0.

If held down and the FBK switch is clicked twice (the bottom position of the
FADE/GAIN/FBK switch), the gains of N2 and N3 are set to 0 (they are
silenced) and N1's gain is set to 1. Their settings still remain.

If held down and the FBK switch is clicked 3 times, all the settings are reset
and only N1 plays as if a recording had just been made using R=B mode.

### PITCH

Controls the pitch of the repeat selected with P1/P2/P3 (or all pitches if UNI
held down). Turned fully counter-clockwise, the note is transposed down an
octave and fully clockwise up an octave. The transpositions are quantized to the
nearest semi-tone. The transposition also affects the length of the playback.

### PRE1/PRE2/PRE3

Chooses what preset is stored or recalled using the STR/-/RCL switch.

### POS

Chooses either where sampling starts within the recording or the velocity of
sampling-start advancement according to the STRIDE/ABS/UNI switch.

### OFST

Chooses when the note set plays relative to when the BEAT light blinks. If in
the fully counter-clockwise position, the note set starts playing at the same
time as the BEAT light's illumination. If in the middle position the note set
starts playing at the time half way between two flashes of the BEAT light.

### FADE/GAIN/FBK

In the middle position, the GAIN knob controls the loudness of the note set.

In the top position, this controls the fading of repeated notes. If turned
counter-clockwise, the repeated notes fade-out and if turned clockwise, the
repeated notes fade-in.

The FBK position can be clicked, which will cycle through feedback modes.
If the FBK light is constantly illuminated, when recording, the output of
N1 is also recorded.
If the feedback light is flashing, the output of all notes is also recorded.
If the feedback light is off, only the input to the pedal is recorded.
FBK can also be used to reset note parameters, see STRIDE/ABS/UNI.

#### Overdubbing

You might want to record a loop and then add new layers to it, keeping the
length of the first loop. This is how to do it: To keep it simple, we suggest
stopping playback and turning down note 2 and 3 (you can try with this stuff
later). See the documentation on the UNI switch for a quick way to do this.

Switch to AREC mode and activate feedback for N1 only by clicking the FBK switch
until the FBK light is constant. Then click record and
play your first loop on your instrument. Click record again to stop recording.
The loop will start but it will also be recording the next loop (remember, AREC
is “automatic recording” mode). So, unless you are ready to add the next layer,
press record again and because record and playback are both active, recording
will stop but the first layer will continue looping (playback will remain on).

Okay, say you're ready to record the next layer. Now you can press record.
Notice that recording doesn't start right away if you start it in the middle of
a loop, it will start on the next beat (a flash of the BEAT light). Once it has
started (and feedback is on), it will record what you are playing and also
include the previous layer(s). Wait until after the BEAT light has lit up again
before clicking the record switch again as clicking record too early will
discard the recording. This last feature is handy though if you make a mistake:
you can click before the BEAT light comes on again and discard the recording
you just made, but keep the previous layer(s).

Note that if you keep FBK enabled and record additional loops the output can
become very dense.

### GAIN

Controls the loudness of the notes or their fading, depending on the setting of
the FADE/GAIN/FBK switch.

## MIDI

Before you can control the device via MIDI messages it is necessary to set the
channel of the MIDI messages it responds to. By default it responds to messages
on channel 1. If you have a new device and you have never set up the MIDI
channel and are content with the 856 only responding to messages transmitted on
channel 1 then you can skip this step. Otherwise follow the following
instructions:

Turn off the 856 by unplugging it. Set the switches according to
the MIDI channel you would like to set. See the following table for the
settings:

<table border="1">
<tr>
<td>Channel</td>
<td>N1/N2/N3 position</td>
<td>FREE/R=B/AREC position</td>
<td>FREE/QUANT/SKIP position</td>
<td>P1/P2/P3 position</td>
</tr>
<tr>
<td>1</td>
<td>middle/down</td>
<td>middle/down</td>
<td>middle/down</td>
<td>middle/down</td>

</tr>
<tr>
<td>2</td>
<td>middle/down</td>
<td>middle/down</td>
<td>middle/down</td>
<td>up</td>

</tr>
<tr>
<td>3</td>
<td>middle/down</td>
<td>middle/down</td>
<td>up</td>
<td>middle/down</td>

</tr>
<tr>
<td>4</td>
<td>middle/down</td>
<td>middle/down</td>
<td>up</td>
<td>up</td>

</tr>
<tr>
<td>5</td>
<td>middle/down</td>
<td>up</td>
<td>middle/down</td>
<td>middle/down</td>

</tr>
<tr>
<td>6</td>
<td>middle/down</td>
<td>up</td>
<td>middle/down</td>
<td>up</td>

</tr>
<tr>
<td>7</td>
<td>middle/down</td>
<td>up</td>
<td>up</td>
<td>middle/down</td>

</tr>
<tr>
<td>8</td>
<td>middle/down</td>
<td>up</td>
<td>up</td>
<td>up</td>

</tr>
<tr>
<td>9</td>
<td>up</td>
<td>middle/down</td>
<td>middle/down</td>
<td>middle/down</td>

</tr>
<tr>
<td>10</td>
<td>up</td>
<td>middle/down</td>
<td>middle/down</td>
<td>up</td>

</tr>
<tr>
<td>11</td>
<td>up</td>
<td>middle/down</td>
<td>up</td>
<td>middle/down</td>

</tr>
<tr>
<td>12</td>
<td>up</td>
<td>middle/down</td>
<td>up</td>
<td>up</td>

</tr>
<tr>
<td>13</td>
<td>up</td>
<td>up</td>
<td>middle/down</td>
<td>middle/down</td>

</tr>
<tr>
<td>14</td>
<td>up</td>
<td>up</td>
<td>middle/down</td>
<td>up</td>

</tr>
<tr>
<td>15</td>
<td>up</td>
<td>up</td>
<td>up</td>
<td>middle/down</td>

</tr>
<tr>
<td>16</td>
<td>up</td>
<td>up</td>
<td>up</td>
<td>up</td>

</tr>

</table>

For example, if you would like the 856 to respond to MIDI messages transmitted
on channel 10, set the N1/N2/N3 switch to the upward position, the FREE/R=B/AREC switch to the middle or downward position, the FREE/QUANT/SKIP switch to the middle
or downward position and the top P1/P2/P3 to the upward position.

Now, while holding the UNI switch down, plug in the power to the back of the
pedal. The MIDI channel you selected should display briefly on the LEDs in
binary with the most significant bit on the left most light. A light that is on
corresponds to a 1 whereas a light that is off corresponds to a 0. The light
(bit) patterns and the corresponding MIDI channel are shown in the following
table.

<table border="1">
<tr>
<td>MIDI Channel</td>
<td>Light (bit) pattern</td>
</tr>
<tr>
<td>1</td>
<td>0000</td>
</tr>
</tr>
<tr>
<td>2</td>
<td>0001</td>
</tr>
</tr>
<tr>
<td>3</td>
<td>0010</td>
</tr>
</tr>
<tr>
<td>4</td>
<td>0011</td>
</tr>
</tr>
<tr>
<td>5</td>
<td>0100</td>
</tr>
</tr>
<tr>
<td>6</td>
<td>0101</td>
</tr>
</tr>
<tr>
<td>7</td>
<td>0110</td>
</tr>
</tr>
<tr>
<td>8</td>
<td>0111</td>
</tr>
</tr>
<tr>
<td>9</td>
<td>1000</td>
</tr>
</tr>
<tr>
<td>10</td>
<td>1001</td>
</tr>
</tr>
<tr>
<td>11</td>
<td>1010</td>
</tr>
</tr>
<tr>
<td>12</td>
<td>1011</td>
</tr>
</tr>
<tr>
<td>13</td>
<td>1100</td>
</tr>
</tr>
<tr>
<td>14</td>
<td>1101</td>
</tr>
</tr>
<tr>
<td>15</td>
<td>1110</td>
</tr>
</tr>
<tr>
<td>16</td>
<td>1111</td>
</tr>
</table>

Shortly afterward, the pedal will continue to power up normally.
It should now respond to messages on the channel you chose. Note that for
channel 1, no LEDs will be illuminated.

### MIDI control change messages

<table border=1>
<tr>
<td> Control change number </td>
<td> Description </td>
<td> Notes </td>
</tr>
<tr>
<td> 0 </td>
<td> N1 Pitch 1 control (fine) </td>
<td> Adjust pitch of 1st, 4th, ... notes in arpeggio from -50 cents to +50 cents (hundreths of a semitone). </td>
</tr>
<tr>
<td> 1 </td>
<td> N1 Pitch 2 control (fine) </td>
<td> Adjust pitch of 2nd, 5th, ... notes in arpeggio from -50 cents to +50 cents (hundreths of a semitone). </td>
</tr>
<tr>
<td> 2 </td>
<td> N1 Pitch 3 control (fine) </td>
<td> Adjust pitch of 3rd, 6th, ... notes in arpeggio from -50 cents to +50 cents (hundreths of a semitone). </td>
</tr>
<tr>
<td> 3 </td>
<td> N1 Envelope control </td>
<td> Control the amplitude envelope of the notes just as the ENV knob does. </td>
</tr>
<tr>
<td> 4 </td>
<td> N1 Length control </td>
<td> Control the length of the notes just as the LEN knob does. </td>
</tr>
<tr>
<td> 5 </td>
<td> N1 Pitch 1 control </td>
<td> Adjust pitch of 1st, 4th, ... notes in arpeggio according to the control change value. 60 is no transposition, 48 is an octave below, 72 an octave above, etc. Note that this is more range than is available on the physical interface. </td>
</tr>
<tr>
<td> 6 </td>
<td> N1 Pitch 2 control </td>
<td> Adjust pitch of 2nd, 5th, ... notes in arpeggio according to the control change value. 60 is no transposition, 48 is an octave below, 72 an octave above, etc. Note that this is more range than is available on the physical interface. </td>
</tr>
<tr>
<td> 7 </td>
<td> N1 Pitch 3 control </td>
<td> Adjust pitch of 3rd, 6th, ... notes in arpeggio according to the control change value. 60 is no transposition, 48 is an octave below, 72 an octave above, etc. Note that this is more range than is available on the physical interface. </td>
</tr>
<tr>
<td> 8 </td>
<td> N1 Gain control </td>
<td> Control the gain just as the GAIN knob does with the FADE/GAIN/FBK switch in the middle position. </td>
</tr>
<tr>
<td> 9 </td>
<td> N1 Position control </td>
<td> Adjust the position just as the POS knob does with the STRIDE/ABS/UNI switch in the middle position. </td>
</tr>
<tr>
<td> 10 </td>
<td> N1 Stride control </td>
<td> Adjust the position advancement just as the POS knob does with the STRIDE/ABS/UNI switch in the upward position. </td>
</tr>
<tr>
<td> 11 </td>
<td> N1 Offset control </td>
<td> Adjust the playback offset just as the OFST knob does. </td>
</tr>
<tr>
<td> 12 </td>
<td> N1 Fade control </td>
<td> Adjust the fade just as the GAIN knob does with the FADE/GAIN/FBK switch in the upward position. </td>
</tr>
<tr>
<td> 13 </td>
<td> N1 Free &#x394; control </td>
<td> Adjust the time between notes just as the &#x394; knob does with the FREE/QUANT/SKIP switch in the upward position. * </td>
</tr>
<tr>
<td> 14 </td>
<td> N1 Number of repeats control </td>
<td> Adjust the number of repeats (the number of notes in the arpeggio) just as the TMPO/REP knob does. * </td>
</tr>
<tr>
<td> 15 </td>
<td> N1 Stride reset </td>
<td>
If 0, reset the note stride accumulator. This means the next playback will start from where the POS setting was set to.
If 1, reset note stride. This means the position of start of playback will not advance every time a note is played (i.e., every beat).
If 2, reset repeats stride. This means the position of start of playback will not advance every repeat.
If >= 2, do the above 3 things. This is provided for convenience.
</td>
</tr>
<tr>
<td> 16 </td>
<td> N1 skip control </td>
<td> Control how often the arpeggio plays just like the &#x394; knob does with the FREE/QUANT/SKIP switch in the downward position. </td>
</tr>
<tr>
<td> 17 </td>
<td> N1 swing control </td>
<td> Control the swing factor for the time between repeats of notes.</td>
</tr>
<tr>
<td> 18 </td>
<td> N2 Pitch 1 control (fine) </td>
<td> Adjust pitch of 1st, 4th, ... notes in arpeggio from -50 cents to +50 cents (hundreths of a semitone). </td>
</tr>
<tr>
<td> 19 </td>
<td> N2 Pitch 2 control (fine) </td>
<td> Adjust pitch of 2nd, 5th, ... notes in arpeggio from -50 cents to +50 cents (hundreths of a semitone). </td>
</tr>
<tr>
<td> 20 </td>
<td> N2 Pitch 3 control (fine) </td>
<td> Adjust pitch of 3rd, 6th, ... notes in arpeggio from -50 cents to +50 cents (hundreths of a semitone). </td>
</tr>
<tr>
<td> 21 </td>
<td> N2 Envelope control </td>
<td> Control the amplitude envelope of the notes just as the ENV knob does. </td>
</tr>
<tr>
<td> 22 </td>
<td> N2 Length control </td>
<td> Control the length of the notes just as the LEN knob does. </td>
</tr>
<tr>
<td> 23 </td>
<td> N2 Pitch 1 control </td>
<td> Adjust pitch of 1st, 4th, ... notes in arpeggio according to the control change value. 60 is no transposition, 48 is an octave below, 72 an octave above, etc. Note that this is more range than is available on the physical interface. </td>
</tr>
<tr>
<td> 24 </td>
<td> N2 Pitch 2 control </td>
<td> Adjust pitch of 2nd, 5th, ... notes in arpeggio according to the control change value. 60 is no transposition, 48 is an octave below, 72 an octave above, etc. Note that this is more range than is available on the physical interface. </td>
</tr>
<tr>
<td> 25 </td>
<td> N2 Pitch 3 control </td>
<td> Adjust pitch of 3rd, 6th, ... notes in arpeggio according to the control change value. 60 is no transposition, 48 is an octave below, 72 an octave above, etc. Note that this is more range than is available on the physical interface. </td>
</tr>
<tr>
<td> 26 </td>
<td> N2 Gain control </td>
<td> Control the gain just as the GAIN knob does with the FADE/GAIN/FBK switch in the middle position. </td>
</tr>
<tr>
<td> 27 </td>
<td> N2 Position control </td>
<td> Adjust the position just as the POS knob does with the STRIDE/ABS/UNI switch in the middle position. </td>
</tr>
<tr>
<td> 28 </td>
<td> N2 Stride control </td>
<td> Adjust the position advancement just as the POS knob does with the STRIDE/ABS/UNI switch in the upward position. </td>
</tr>
<tr>
<td> 29 </td>
<td> N2 Offset control </td>
<td> Adjust the playback offset just as the OFST knob does. </td>
</tr>
<tr>
<td> 30 </td>
<td> N2 Fade control </td>
<td> Adjust the fade just as the GAIN knob does with the FADE/GAIN/FBK switch in the upward position. </td>
</tr>
<tr>
<td> 31 </td>
<td> N2 Free &#x394; control </td>
<td> Adjust the time between notes just as the &#x394; knob does with the FREE/QUANT/SKIP switch in the upward position. </td>
</tr>
<tr>
<td> 32 </td>
<td> N2 Number of repeats control </td>
<td> Adjust the number of repeats (the number of notes in the arpeggio) just as the TMPO/REP knob does. </td>
</tr>
<tr>
<td> 33 </td>
<td> N2 Stride reset </td>
<td>
If 0, reset the note stride accumulator. This means the next playback will start from where the POS setting was set to.
If 1, reset note stride. This means the position of start of playback will not advance every time a note is played (i.e., every beat).
If 2, reset repeats stride. This means the position of start of playback will not advance every repeat.
If >= 2, do the above 3 things. This is provided for convenience.
</td>
</tr>
<tr>
<td> 34 </td>
<td> N2 skip control </td>
<td> Control how often the arpeggio plays just like the &#x394; knob does with the FREE/QUANT/SKIP switch in the downward position. </td>
</tr>
<tr>
<td> 35 </td>
<td> N2 swing control </td>
<td> Control the swing factor for the time between repeats of notes.</td>
</tr>
<tr>
<td> 36 </td>
<td> N3 Pitch 1 control (fine) </td>
<td> Adjust pitch of 1st, 4th, ... notes in arpeggio from -50 cents to +50 cents (hundreths of a semitone). </td>
</tr>
<tr>
<td> 37 </td>
<td> N3 Pitch 2 control (fine) </td>
<td> Adjust pitch of 2nd, 5th, ... notes in arpeggio from -50 cents to +50 cents (hundreths of a semitone). </td>
</tr>
<tr>
<td> 38 </td>
<td> N3 Pitch 3 control (fine) </td>
<td> Adjust pitch of 3rd, 6th, ... notes in arpeggio from -50 cents to +50 cents (hundreths of a semitone). </td>
</tr>
<tr>
<td> 39 </td>
<td> N3 Envelope control </td>
<td> Control the amplitude envelope of the notes just as the ENV knob does. </td>
</tr>
<tr>
<td> 40 </td>
<td> N3 Length control </td>
<td> Control the length of the notes just as the LEN knob does. </td>
</tr>
<tr>
<td> 41 </td>
<td> N3 Pitch 1 control </td>
<td> Adjust pitch of 1st, 4th, ... notes in arpeggio according to the control change value. 60 is no transposition, 48 is an octave below, 72 an octave above, etc. Note that this is more range than is available on the physical interface. </td>
</tr>
<tr>
<td> 42 </td>
<td> N3 Pitch 2 control </td>
<td> Adjust pitch of 2nd, 5th, ... notes in arpeggio according to the control change value. 60 is no transposition, 48 is an octave below, 72 an octave above, etc. Note that this is more range than is available on the physical interface. </td>
</tr>
<tr>
<td> 43 </td>
<td> N3 Pitch 3 control </td>
<td> Adjust pitch of 3rd, 6th, ... notes in arpeggio according to the control change value. 60 is no transposition, 48 is an octave below, 72 an octave above, etc. Note that this is more range than is available on the physical interface. </td>
</tr>
<tr>
<td> 44 </td>
<td> N3 Gain control </td>
<td> Control the gain just as the GAIN knob does with the FADE/GAIN/FBK switch in the middle position. </td>
</tr>
<tr>
<td> 45 </td>
<td> N3 Position control </td>
<td> Adjust the position just as the POS knob does with the STRIDE/ABS/UNI switch in the middle position. </td>
</tr>
<tr>
<td> 46 </td>
<td> N3 Stride control </td>
<td>
Adjust the position advancement just as the POS knob does with the STRIDE/ABS/UNI switch in the upward position. </td>
</tr>
<tr>
<td> 47 </td>
<td> N3 Offset control </td>
<td> Adjust the playback offset just as the OFST knob does. </td>
</tr>
<tr>
<td> 48 </td>
<td> N3 Fade control </td>
<td> Adjust the fade just as the GAIN knob does with the FADE/GAIN/FBK switch in the upward position. </td>
</tr>
<tr>
<td> 49 </td>
<td> N3 Free &#x394; control </td>
<td> Adjust the time between notes just as the &#x394; knob does with the FREE/QUANT/SKIP switch in the upward position. * </td>
</tr>
<tr>
<td> 50 </td>
<td> N3 Number of repeats control </td>
<td> Adjust the number of repeats (the number of notes in the arpeggio) just as the TMPO/REP knob does. * </td>
</tr>
<tr>
<td> 51 </td>
<td> N3 Stride reset </td>
<td>
If 0, reset the note stride accumulator. This means the next playback will start from where the POS setting was set to.
If 1, reset note stride. This means the position of start of playback will not advance every time a note is played (i.e., every beat).
If 2, reset repeats stride. This means the position of start of playback will not advance every repeat.
If > 2, do the above 3 things. This is provided for convenience.
</td>
</tr>
<tr>
<td> 52 </td>
<td> N3 skip control </td>
<td> Control how often the arpeggio plays just like the &#x394; knob does with the FREE/QUANT/SKIP switch in the downward position. </td>
</tr>
<tr>
<td> 53 </td>
<td> N3 swing control </td>
<td> Control the swing factor for the time between repeats of notes.</td>
</tr>
<tr>
<td> 54 </td>
<td> Coarse tempo control </td>
<td> Control how often the sequence plays in beats per minute (BPM) from 11 BPM to 60 BPM. </td>
</tr>
<tr>
<td> 55 </td>
<td> Fine tempo control </td>
<td> Control how often the sequence plays by adding to the current tempo -10 to +10 BPM. </td>
</tr>
<tr>
<td> 56 </td>
<td> Tempo scaling </td>
<td> Scale the tempo like the &#x394; knob with N1/N2/N3 in the upward position and FREE/QUANT/SKIP in the middle position. </td>
</tr>
<tr>
<td> 57 </td>
<td> Tempo nudge </td>
<td> Slightly adjust the tempo like the TMPO/REP knob with N1/N2/N3 in the upward position and FREE/R=B/AREC in the middle position. </td>
</tr>
<tr>
<td> 58 </td>
<td> Preset store </td>
<td> Store the current settings in a specified register. The message value of 0 stores in the PRE1 register, a value of 1 stores in the PRE2 register and a value of 2 stores in the PRE3 register. If greater than 2, stores in the PRE3 register. </td>
</tr>
<tr>
<td> 59 </td>
<td> Preset recall </td>
<td> Recall the current settings from a specified register. The message value of 0 recalls from the PRE1 register, a value of 1 recalls from the PRE2 register and a value of 2 recalls from the PRE3 register.  If greater than 2, recalls from the PRE3 register. </td>
</tr>
<tr>
<td> 60 </td>
<td> Record enable/disable </td>
<td> Start/stop recording. A message value of 0 stops recording. A message value greater than 0 starts or restarts recording depending on whether or not recording is already going on. </td>
</tr>
<tr>
<td> 61 </td>
<td> Play start </td>
<td> Start/stop playback. A message value of 0 disables all playback. A message value greater than 0 starts playback. If playback is already going on it will start a new set of notes along side the already playing set. </td>
</tr>
<tr>
<td> 62 </td>
<td> Record mode </td>
<td> Sets the record mode just like the FREE/R=B/AREC switch. A message value of 0 sets the FREE record mode, a value of 1 sets the R=B mode and a value of 2 sets the AREC mode. A value greater than 2 sets the AREC mode. </td>
</tr>
<tr>
<td> 63 </td>
<td> Feedback state </td>
<td>
Sets the feedback state. Data byte of 0 means off, 1 just N1 fed back, 2 all notes fed back,
otherwise does nothing.
</td>
</tr>
<tr>
<td> 64 </td>
<td> N1 stride control </td>
<td>
Sets the amount the playback position in the sample moves each time N1
plays (every beat). Like the POS knob when STRIDE/ABS/UNI is the in STRIDE
position and N1/N2/N3 is in N1 position.
</td>
</tr>
<tr>
<td> 65 </td>
<td> N2 stride control </td>
<td>
Sets the amount the playback position in the sample moves each time N2
plays (every beat). Like the POS knob when STRIDE/ABS/UNI is the in STRIDE
position and N1/N2/N3 is in N2 position.
</td>
</tr>
<tr>
<td> 66 </td>
<td> N3 stride control </td>
<td>
Sets the amount the playback position in the sample moves each time N3
plays (every beat). Like the POS knob when STRIDE/ABS/UNI is the in STRIDE
position and N1/N2/N3 is in N3 position.
</td>
</tr>
</table>

#### MIDI Tempo scaling control change ranges

<table border=1>
<tr>
<td> CC range </td>
<td> scale value (divisor) </td>
</tr>
<tr>
<td> [ 0 15] </td>
<td> 4. </td>
</tr>
<tr>
<td> [16 31] </td>
<td> 3. </td>
</tr>
<tr>
<td> [32 47] </td>
<td> 2. </td>
</tr>
<tr>
<td> [48 63] </td>
<td> 1. </td>
</tr>
<tr>
<td> [64 79] </td>
<td> 1. </td>
</tr>
<tr>
<td> [80 95] </td>
<td> 1./2. </td>
</tr>
<tr>
<td> [ 96 111] </td>
<td> 1./3. </td>
</tr>
<tr>
<td> [112 127] </td>
<td> 1./4. </td>
</tr>
</table>

### MIDI note on messages

Sending a MIDI note on message on the channel the 856 is set up to listen on
will play back one iteration of the loop or sequence. The pitch value will scale
all the pitches accordingly: a pitch value of 60 means no transposition, 72
means all pitches are transposed 1 octave up, 48 means they are all transposed
one octave down, etc. The velocity value scales the GAIN value of all notes. 127
means no scaling, 63 is a scaling of about half, etc. Note off messages are
ignored. 

### MIDI clock

The 856 can be synchronized to external hardware by sending it a MIDI clock. To
see if this is active, hold down the UNI switch. If the first LED is lit up it
means the 856 is using internal scheduling, if the second LED is lit up, it
means it is listening for a MIDI clock to control its scheduling.  To toggle
between the two modes click the PLAY footswitch while holding down the UNI
switch. You should see the LED change to indicate what mode has been set.  Note
that in MIDI clock mode no playback will happen if no clock is being sent!  The
856 expects 24 MIDI clock messages per quarter note, which is compatible with
most hardware. A quarter note is the amount of time between two flashings of the
BEAT light on the 856.

### MIDI Through

All the MIDI messages entering the unit via the MIDI in port are passed through
to the MIDI through port.

## The expression pedal 

(only on BOARD_V2 or higher)

It's easy to use an expression pedal with the 856. Simply plug one into the 1/4
inch jack closest to the DC jack. Then to choose what knob to control with the
expression pedal, simply hold down the FBK switch while turning the knob you want to
control. After you let go of FBK, the expression pedal should now control that knob.

## Version information

### v1.6

- Documentation on choosing the MIDI channel makes it clear what switches should be manipulated.
- Playback requests ignored if nothing has been recorded.
- Tempo range in FREE mode made more reasonable.
- Correctly indicate ±50 cents change (instead of ±100 cents) when adjusting the fine tuning.
- Sending MIDI 64 for respective control change sets fine tuning to 0 or stride
  rate to 0. Tempo controlling control changes also choose exact midpoint when sending 64.
- Clicking FBK when UNI held down resets STRIDE position now. Also stride always
  active (before it was only active when the switch was in that position). The
  previous functionality of zeroing the gain of and resetting all the settings
  of N2 and N3 is still possible
  but you now click FBK twice and 3 times respectively.
- v0.5.1 of mm_dsp used now. This allows detuning by 1 cent because the playback
  rate is compensated.
- Corrected erroneous description of envelope shapes, added plot describing the
  change in ramp time according to the position of the ENV knob.
- 2 feedback modes now available, one where only N1 fedback and one where all notes fed back

