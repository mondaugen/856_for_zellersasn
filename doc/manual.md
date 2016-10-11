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

The momentary switches "acknowledge" a press when they are released. For
example, when you want to record a loop (in A=R mode), press and hold the record
footswitch on the left and release it when you want to start recording. When you
want to stop recording press and hold the record footswitch and release when you
want recording to end.

You can only record for a maximum of 20 seconds. After this time, the recording
will automatically stop.

## Terminology

When we say "parameter" in this manual, we refer to the value that a particular
knob represents. For example, the "pitch" knob controls the pitch parameter.

When we say "click a switch" in this manual, we mean press and release a switch.
The time between pressing and releasing should be short, but it acutally doesn't
matter if nothing else changes while the switch is down (no knob is turned,
etc.).

## Setting up

The pedal is on moments after it is plugged in (it takes a few milliseconds to
boot up). The power socket on the back is for a 9 volt adapter with a negative
tip and a positive shell (this is standard for most guitar pedals).

Looking from the top, the 1/4 inch jack furthest to the right is the audio input
and the one just to the left of it the audio output.

The jack just to the left of the audio output jack is for incoming MIDI
messages.

## Tutorial

Before going through all the controls on the 856, we'll take you through a
tutorial' to get a flavour for what con be done with it.

See the "setting up" section to set up the pedal before proceding.

To reset all the parameters hold the STRIDE/ABS/(UNI) switch in the down
position while clicking the FADE/GAIN/(FBK) switch in the downward direction
twice.

Set the controls to control note set 1 by putting the N1/BAR/N2/N3 switch in the
upward position.

Put the pedal in R=B mode by putting the FREE/R=B/AREC switch in the middle
position.

The rest of the switches can stay as they are for now.

Play a loop you are interested in recording. The first time we recommend just
playing a note and letting it sound, then recording it for 1 or 2 seconds --
what is going on will be clearer this way. Press and hold the left footswitch
and let go when you want to start recording. Press and hold the switch and let
go when you want to stop.  The loop you played should now be playing.

As the N1/BAR/N2/N3 switch is in the upward position and we are in A=R mode, the
TMPO/REP knob only slightly changes the tempo. This is useful for adjusting your
loop if it wasn't quite recorded right. The &#x394; knob controls the tempo more
coarsely if the FREE/QUANT/SKIP switch is in the upward position. If in the
middle position, it doubles, triples or quadruples the tempo depending on the
&#x394; knob's position. This knob turned all the way clockwise does no scaling
to the tempo, it turned all the way counter-clockwise quadruples the tempo.

With the FADE/GAIN/(FBK) switch in the middle position, turn the GAIN knob all
the way counter-clockwise. The loop should not sound the next time it plays (the
gain adjustment will not be immediate). Put the N1/BAR/N2/N3 switch in the
middle position. You are now controlling the second note set. Turn the LEN knob
to about half, the TMPO/REP knob to about half. Set the FREE/QUANT/SKIP to the
middle position and turn the &#x394; knob to the middle. Now turn up the GAIN
knob to maximum. You should hear many repeated copies of the beginning of your
loop. Try adjusting the TMPO/REP knob to add fewer or more copies. Try adjusting
the &#x394; knob to change the time between the repeats.

You can choose to sample a different part of your loop. To do this, set the
STRIDE/ABS/(UNI) knob in the middle position and move the POS knob. You will
hear the part of the loop that is sampled change based on where the POS knob is.

The pitch of the repeats can be controlled. Set the TMPO/REP knob to maximum to
get a lot of repeats. We can control the pitches of sub-sets of the repeated
notes. The P1/P2/P3 switch set in the upward position allows controlling the
pitch of the 1st playback and 3rd, 6th ... repeat. This switch in the middle
position controls the pitch of the 1st, 4th ... repeats and in the bottom
position controls the pitch of the 2nd, 5th ... repeats. With the PITCH knob set
to a different position for each of the P1/P2/P3 switch settings, you should
hear a sort of arpeggio in the repeats (or chord if the time between the repeats
is short).

You can make the repeats fade out or fade in by setting the FADE/GAIN/(FBK)
switch to the upward position. The GAIN knob turned counter-clockwise will cause
the repeats to fade out, there will be no fading if it is in the middle and the
repeats will fade in if the knob is turned to the right. For fading in to be
audible, the overall GAIN should not be maximum as the fading in is stopped when
the notes would clip. To experience this, set the FADE/GAIN/(FBK) switch to the
middle position and turn the GAIN knob to the middle. Then set the
FADE/GAIN/(FBK) switch back to the top position and turn the GAIN knob to the
right. You should hear the notes fade in.

Our original loop is still present, just turned down. To hear it along with what
you've set up on note 2, set the N1/BAR/N2/N3 switch to the top position, set
the FADE/GAIN/(FBK) switch to the middle position and turn the GAIN knob to the
top (if it is already at the top, turn the knob down then up again). You should
hear the original loop with the repeats you created on note 2 playing at the
same time as the beginning of the loop. We can change when these repeats occur
relative to note 1. Set the N1/BAR/N2/N3 switch to the middle position. Now
adjust the OFST knob. You should hear the repeats of note 2 starting sometime
within the loop playing on note 1 -- when OFST is all the way counter-clockwise,
the repeats start at the beginning of the loop and all the way clockwise they
start at the end (which is close to the beginning as it's a loop).

If you like what you've created and want to apply it to other loops, you can
save the parameters by setting the PRE1/PRE2/PRE3 switch and clicking the
STR-RCL switch upwards.

<!-- ![Why no image?](/tmp/test.svg) -->

## MIDI Control

### Setting the MIDI channel

Before you can control the device via MIDI messages it is necessary to set the
channel of the MIDI messages it responds to. By default it responds to messages
on channel 1. If you have a new device and you have never set up the MIDI
channel and are content with the 856 only responding to messages transmitted on
channel 1 then you can skip this step. Otherwise follow the following
instructions:

Turn off the 856 by unplugging it. Set the switches in the top row according to
the MIDI channel you would like to set. See the following table for the
settings:

<table border="1">
<tr>
<td>Channel</td>
<td>Switch 1 position</td>
<td>Switch 2 position</td>
<td>Switch 3 position</td>
<td>Switch 4 position</td>
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
on channel 10, set the top left-hand switch to the upward position, the top
middle-left switch to the middle or downward position, the top middle-right switch to the middle
or downward position and the top right-hand switch to the upward position.

Now, while holding the (UNI) switch down, plug in the power to the back of the
pedal. The MIDI channel you selected should display on the LEDs using the same
pattern as in the table (the illumniated LEDs will correspond to switches in the
upward position, the dark LEDs to those in the middle or downward position).
When you release the (UNI) switch, the pedal will continue to power up normally.
It should now respond to messages on the channel you chose.

### MIDI control change messages


