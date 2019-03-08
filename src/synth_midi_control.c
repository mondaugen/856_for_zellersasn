#include <stdlib.h>

typedef struct {
    int note;
    int pitch;
} synth_midi_cc_pitch_control_t;

synth_midi_cc_pitch_control_t *alloc_synth_midi_cc_pitch_control_t()
{
    return malloc(sizeof(synth_midi_cc_pitch_control_t));
}

//<td> N1 Pitch 1 control (fine) </td>
//<td> Adjust pitch of 1st, 4th, ... notes in arpeggio from -50 cents to +50 cents (hundreths of a semitone). </td>
//<td> N1 Pitch 2 control (fine) </td>
//<td> Adjust pitch of 2nd, 5th, ... notes in arpeggio from -50 cents to +50 cents (hundreths of a semitone). </td>
//<td> N1 Pitch 3 control (fine) </td>
//<td> Adjust pitch of 3rd, 6th, ... notes in arpeggio from -50 cents to +50 cents (hundreths of a semitone). </td>
/* note pitch */
void synth_midi_cc_pitch_fine_control(void *data, MIDIMsg *msg)
{
    synth_midi_cc_pitch_control_t *params = data;
    synth_control_set_pitch_fine_quant(
        midi_util_map_midpoint_exact(msg->data[2],0,1),
        params->pitch,
        params->note);
}
//<td> N1 Envelope control </td>
//<td> Control the amplitude envelope of the notes just as the ENV knob does. </td>
/* note */
void synth_midi_cc_env_control(void *data, MIDIMsg *msg)
{
    int *note = data;
    synth_control_set_envelopeTime(
            (float)msg->data[2]/(float)MIDIMSG_DATA_BYTE_MAX,
            *note);
}

//<td> N1 Length control </td>
//<td> Control the length of the notes just as the LEN knob does. </td>
/* note */
void synth_midi_cc_sus_control(void *data, MIDIMsg *msg)
{
    int *note = data;
    synth_control_set_sustainTime(
            (float)msg->data[2]/(float)MIDIMSG_DATA_BYTE_MAX,
            *note);
}

//<td> N1 Pitch 1 control </td>
//<td> Adjust pitch of 1st, 4th, ... notes in arpeggio according to the control
//change value. 60 is no transposition, 48 is an octave below, 72 an octave
//above, etc. Note that this is more range than is available on the physical
//interface. </td>
//<td> N1 Pitch 2 control </td>
//<td> Adjust pitch of 2nd, 5th, ... notes in arpeggio according to the control
//change value. 60 is no transposition, 48 is an octave below, 72 an octave
//above, etc. Note that this is more range than is available on the physical
//interface. </td>
//<td> N1 Pitch 3 control </td>
//<td> Adjust pitch of 3rd, 6th, ... notes in arpeggio according to the control
//change value. 60 is no transposition, 48 is an octave below, 72 an octave
//above, etc. Note that this is more range than is available on the physical
//interface. </td>
/* note pitch */
void synth_midi_cc_pitch_control(void *data, MIDIMsg *msg)
{
    synth_midi_cc_pitch_control_t *params = data;
    synth_control_set_pitch((float)msg->data[2],
            params->pitch,
            params->note);
}

//<td> N1 Gain control </td>
//<td> Control the gain just as the GAIN knob does with the FADE/GAIN/FBK switch in the middle position. </td>
/* note */
void synth_midi_cc_gain_control(void *data, MIDIMsg *msg)
{
    int *note = data;
    synth_control_set_wet(
            (float)msg->data[2]/(float)MIDIMSG_DATA_BYTE_MAX,
            *note);
}


//<td> N1 Position control </td>
//<td> Adjust the position just as the POS knob does with the STRIDE/ABS/UNI switch in the middle position. </td>
/* note */
void synth_midi_cc_pos_control(void *data, MIDIMsg *msg)
{
    int *note = data;
    synth_control_set_startPoint(
            (float)msg->data[2]/(float)MIDIMSG_DATA_BYTE_MAX,
            *note);
}

//<td> N1 Stride control </td>
//<td> Adjust the position advancement just as the POS knob does with the STRIDE/ABS/UNI switch in the upward position. </td>
/* note */
void synth_midi_cc_stride_control(void *data, MIDIMsg *msg)
{
    int *note = data;
    synth_control_set_positionStride(
            midi_util_map_midpoint_exact(msg->data[2],0,1),
            *note);
}

//<td> N1 Offset control </td>
//<td> Adjust the playback offset just as the OFST knob does. </td>
/* note */
void synth_midi_cc_offset_control(void *data, MIDIMsg *msg)
{
    int *note = data;
    synth_control_set_offset(
            (float)msg->data[2]/(float)MIDIMSG_DATA_BYTE_MAX,
            *note);
}

//<td> N1 Fade control </td>
//<td> Adjust the fade just as the GAIN knob does with the FADE/GAIN/FBK switch in the upward position. </td>
/* note */
void synth_midi_cc_fbk_rate_control(void *data, MIDIMsg *msg)
{
    int *note = data;
    synth_control_set_ampLastEcho(
            (float)msg->data[2]/(float)MIDIMSG_DATA_BYTE_MAX,
            *note);
}

//<td> N1 Free &#x394; control </td> <td> Adjust the time between notes just as
//the &#x394; knob does with the FREE/QUANT/SKIP switch in the upward position.
//* </td>
/* note */
void synth_midi_cc_event_delta_control(void *data, MIDIMsg *msg)
{
    int *note = data;
    synth_control_set_eventDelta_free(
            (float)msg->data[2]/(float)MIDIMSG_DATA_BYTE_MAX,
            *note);
}

//<td> N1 Number of repeats control </td>
//<td> Adjust the number of repeats (the number of notes in the arpeggio) just as the TMPO/REP knob does. * </td>
/* note */
void synth_midi_cc_num_reps_control(void *data, MIDIMsg *msg)
{
    int *note = data;
    synth_control_set_numRepeats(
            (float)msg->data[2]/(float)MIDIMSG_DATA_BYTE_MAX * (float)SYNTH_CONTROL_MAX_NUM_REPEATS,
            *note);
}

//<td> N1 Stride state </td>
// If 0, reset the note stride accumulator
// If 1, reset note stride
// If 2, reset position stride
// If >= 2, do everything
/* note */
void synth_midi_cc_stride_reset(void *data, MIDIMsg *msg)
{
    int *note = data;
    /* map to the middle */
    int datum = msg->data[2];
    if (datum >= 2) { goto all; }
    switch (datum) {
all:
    case 0:
        synth_control_reset_noteStrideAcc_note(*note);
        if (!(datum >= 2)) { break; }
    case 1:
        synth_control_set_noteStride(0.5, *note);
        if (!(datum >= 2)) { break; }
    case 2:
        synth_control_set_positionStride(0.5, *note);
        if (!(datum >= 2)) { break; }
    }
}

/*
TODO: Make this just return a struct containing everything the higher level
functions need to know.
Also this isn't needed this is just a note style function.
*/
void synth_midi_cc_stride_reset_t_init(
        MIDI_Router_Standard *router,
        int midi_channel,
        synth_midi_cc_stride_reset_t *controls,
        int num_params)
{
    int n;
    for (n = 0; n < num_params; n++) {
        controls[n].note = n;
        controls[n].last_state = SynthControlPosMode_ABSOLUTE;
        MIDI_CC_CB_Router_addCB(&router->cbRouters[midi_channel],
                /* cc number */
                n * SYNTH_MIDI_NUM_NOTE_PARAMS 
                    + synth_midi_cc_type_t_STRIDE_RESET,
                synth_midi_cc_stride_reset,
                &controls[n]);
    }
}

//<td> N1 skip control </td>
//<td> Control how often the arpeggio plays just like the &#x394; knob does with the FREE/QUANT/SKIP switch in the downward position. </td>
/* note */
void synth_midi_cc_interm_control(void *data, MIDIMsg *msg)
{
    int *note = data;
    synth_control_set_intermittency(
            (float)msg->data[2]/(float)MIDIMSG_DATA_BYTE_MAX,
            *note);
}

//<td> N1 swing control </td>
//<td> Control the swing factor for the time between repeats of notes.</td>
/* note */
void synth_midi_cc_swing_control(void *data, MIDIMsg *msg)
{
    int *note = data;
    synth_control_set_swing((float)msg->data[2]/(float)MIDIMSG_DATA_BYTE_MAX,
                            *note);
}

//<td> Coarse tempo control </td>
//<td> Control how often the sequence plays in beats per minute (BPM) from 11 BPM to 60 BPM. </td>
/* global */
void synth_midi_cc_tempo_coarse_control(void *data, MIDIMsg *msg)
{
    synth_control_set_tempo_coarse_norm(midi_util_map_midpoint_exact(msg->data[2],0,1));
}

//<td> Fine tempo control </td>
//<td> Control how often the sequence plays by adding to the current tempo -10 to +10 BPM. </td>
/* global */
void synth_midi_cc_tempo_fine_control(void *data, MIDIMsg *msg)
{
    synth_control_set_tempo_fine_norm(midi_util_map_midpoint_exact(msg->data[2],0,1));
}

//<td> Tempo scaling </td>
//<td> Scale the tempo like the &#x394; knob with N1/N2/N3 in the upward position and FREE/QUANT/SKIP in the middle position. </td>
/* global */
void synth_midi_cc_tempo_scale_control(void *data, MIDIMsg *msg)
{
    synth_control_set_tempo_scale_norm((float)msg->data[2] / (float)MIDIMSG_DATA_BYTE_MAX);
}

//<td> Tempo nudge </td>
//<td> Slightly adjust the tempo like the TMPO/REP knob with N1/N2/N3 in the upward position and FREE/R=B/AREC in the middle position. </td>
/* global */
void synth_midi_cc_tempo_nudge_control(void *data, MIDIMsg *msg)
{
    synth_control_tempoNudge((float)msg->data[2] / (float)MIDIMSG_DATA_BYTE_MAX);
}

//<td> Preset store </td>
//<td> Store the current settings in a specified register. The message value of 0 stores in the PRE1 register, a value of 1 stores in the PRE2 register and a value of 2 stores in the PRE3 register. If greater than 2, stores in the PRE3 register. </td>
/* global */
void synth_midi_cc_preset_store_control(void *data, MIDIMsg *msg)
{
    sc_presets_store(msg->data[2]);
}

//<td> Preset recall </td>
//<td> Recall the current settings from a specified register. The message value of 0 recalls from the PRE1 register, a value of 1 recalls from the PRE2 register and a value of 2 recalls from the PRE3 register.  If greater than 2, recalls from the PRE3 register. </td>
/* global */
void synth_midi_cc_preset_recall_control(void *data, MIDIMsg *msg)
{
    sc_presets_recall(msg->data[2]);
}

//<td> Record enable/disable </td>
//<td> Start/stop recording. A message value of 0 stops recording. A message value greater than 0 starts or restarts recording depending on whether or not recording is already going on. </td>
/* global */
void synth_midi_cc_rec_control(void *data, MIDIMsg *msg)
{
    if (msg->data[2] > 0) {
        synth_control_record_start();
    } else {
        synth_control_record_stop();
    }
}

//<td> Play start </td>
//<td> Start/stop playback. A message value of 0 disables all playback. A message value greater than 0 starts playback. If playback is already going on it will start a new set of notes along side the already playing set. </td>
/* global */
void synth_midi_cc_play_control(void *data, MIDIMsg *msg)
{
    if (msg->data[2] > 0) {
        synth_control_schedulerState_on();
    } else {
        synth_control_schedulerState_off();
    }
}

//<td> Record mode </td>
//<td> Sets the record mode just like the FREE/R=B/AREC switch. A message value of 0 sets the FREE record mode, a value of 1 sets the R=B mode and a value of 2 sets the AREC mode. A value greater than 2 sets the AREC mode. </td>
typedef SynthControlRecMode synth_midi_cc_rec_mode_control_t;
void synth_midi_cc_rec_mode_control(void *data, MIDIMsg *msg)
{
    synth_midi_cc_rec_mode_control_t *last_recMode_param = data;
    synth_control_set_recMode_onChange((SynthControlRecMode)msg->data[2],
                                       last_recMode_param);
}

void synth_midi_cc_rec_mode_control_t_init(
        MIDI_Router_Standard *router,
        int midi_channel,
        synth_midi_cc_rec_mode_control_t *control)
{
    *control = SynthControlRecMode_START__;
    MIDI_CC_CB_Router_addCB(&router->cbRouters[midi_channel],
            synth_midi_cc_type_t_REC_MODE,
            synth_midi_cc_rec_mode_control,
            control);
}

//<td> Feedback state </td>
//<td> Sets the feedback state just like the bottom position of the FADE/GAIN/FBK switch. A non-zero value turns feedback on, a zero value turns it off. </td>
/* global */
void synth_midi_cc_fbk_state_control(void *data, MIDIMsg *msg)
{
    synth_control_feedback_control(msg->data[2]);
}

void synth_midi_note_on_control(void *data, MIDIMsg *msg)
{
    float pitch, amplitude;
    pitch = msg->data[1] - SYNTH_CONTROL_PITCH_UNISON;
    amplitude = (float)msg->data[2] / (float)MIDIMSG_DATA_BYTE_MAX;
    amplitude = SYNTH_CONTROL_MIN_GAIN 
        + (SYNTH_CONTROL_MAX_GAIN - SYNTH_CONTROL_MIN_GAIN)*amplitude;
    amplitude = powf(10.,amplitude/20.);
    synth_control_one_shot(pitch,amplitude);
}

/*
initializes a MIDI control function that doesn't need any extra parameters,
so returns void
increments cc number
*/
static void synth_midi_cc_global_func_init(
        MIDI_Router_Standard *router,
        int midi_channel,
        void (*func)(void*,MIDIMsg*),
        unsigned int *cc)
{
    MIDI_CC_CB_Router_addCB(&router->cbRouters[midi_channel],
            /* cc number */
            *cc,
            func,
            NULL);
    *cc = *cc + 1;
}

/* using alloc will save a bunch of repeated stuff */
static int *alloc_int() { return malloc(sizeof(int)); }

/* initializes a function that takes a note as its parameter 
increments cc number
*/
static void *
synth_midi_cc_note_func_init(
        MIDI_Router_Standard *router,
        int midi_channel,
        void (*func)(void*,MIDIMsg*),
        unsigned int *cc,
        int note)
{
    int *pnote = alloc_int();
    if (!pnote) { goto fail; }
    MIDI_CC_CB_Router_addCB(&router->cbRouters[midi_channel],
            /* cc number */
            *cc,
            func,
            pnote);
    *cc = *cc + 1;
    return (void*)pnote;
fail:
    return NULL;
}

/*
initializes a function that takes a note and pitch as its parameter 
increments cc number
*/
static void *
synth_midi_cc_note_pitch_func_init(
        MIDI_Router_Standard *router,
        int midi_channel,
        void (*func)(void*,MIDIMsg*),
        unsigned int *cc,
        int note,
        int pitch)
{
    synth_midi_cc_pitch_control_t *p_note_pitch = alloc_synth_midi_cc_pitch_control_t();
    if (!p_note_pitch) { goto fail; }
    *p_note_pitch = (synth_midi_cc_pitch_control_t) {
        .note = note,
        .pitch = pitch
    };
    MIDI_CC_CB_Router_addCB(
        &router->cbRouters[midi_channel],
        /* cc number */
        *cc,
        func,
        p_note_pitch);
    *cc = *cc + 1;
    return (void*)p_note_pitch;
fail:
    return NULL;
}

/* TODO the remaining few functions */
