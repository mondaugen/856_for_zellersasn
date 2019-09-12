#include <stdlib.h>
#include "synth_midi_control.h" 
#include "midi_util.h"
#include "scheduling.h"

/* We store the callback with all the controls we register, so we can check
automatically if it gets called. */
struct synth_midi_control_params_t {
    void (*func)(void*,MIDIMsg*);
};

static synth_midi_control_params_t *
alloc_synth_midi_control_params_t()
{
    return malloc(sizeof(synth_midi_control_params_t));
}

typedef struct {
    synth_midi_control_params_t super;
    int note;
    int pitch;
} synth_midi_cc_pitch_control_t;

static synth_midi_cc_pitch_control_t *
alloc_synth_midi_cc_pitch_control_t()
{
    return malloc(sizeof(synth_midi_cc_pitch_control_t));
}

typedef struct {
    synth_midi_control_params_t super;
    int note;
} synth_midi_cc_note_control_t;

static synth_midi_cc_note_control_t *
alloc_synth_midi_cc_note_control_t()
{
    return malloc(sizeof(synth_midi_cc_note_control_t));
}

//<td> N1 Pitch 1 control (fine) </td>
//<td> Adjust pitch of 1st, 4th, ... notes in arpeggio from -50 cents to +50 cents (hundreths of a semitone). </td>
//<td> N1 Pitch 2 control (fine) </td>
//<td> Adjust pitch of 2nd, 5th, ... notes in arpeggio from -50 cents to +50 cents (hundreths of a semitone). </td>
//<td> N1 Pitch 3 control (fine) </td>
//<td> Adjust pitch of 3rd, 6th, ... notes in arpeggio from -50 cents to +50 cents (hundreths of a semitone). </td>
/* note pitch */
static void
synth_midi_cc_pitch_fine_control(void *data, MIDIMsg *msg)
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
static void
synth_midi_cc_env_control(void *data, MIDIMsg *msg)
{
    int *note = data;
    synth_control_set_envelopeTime(
            (float)msg->data[2]/(float)MIDIMSG_DATA_BYTE_MAX,
            *note);
}

//<td> N1 Length control </td>
//<td> Control the length of the notes just as the LEN knob does. </td>
/* note */
static void
synth_midi_cc_sus_control(void *data, MIDIMsg *msg)
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
static void
synth_midi_cc_pitch_control(void *data, MIDIMsg *msg)
{
    synth_midi_cc_pitch_control_t *params = data;
    synth_control_set_pitch((float)msg->data[2],
            params->pitch,
            params->note);
}

//<td> N1 Gain control </td>
//<td> Control the gain just as the GAIN knob does with the FADE/GAIN/FBK switch in the middle position. </td>
/* note */
static void
synth_midi_cc_gain_control(void *data, MIDIMsg *msg)
{
    int *note = data;
    synth_control_set_wet(
            (float)msg->data[2]/(float)MIDIMSG_DATA_BYTE_MAX,
            *note);
}


//<td> N1 Position control </td>
//<td> Adjust the position just as the POS knob does with the STRIDE/ABS/UNI switch in the middle position. </td>
/* note */
static void
synth_midi_cc_pos_control(void *data, MIDIMsg *msg)
{
    int *note = data;
    synth_control_set_startPoint(
            (float)msg->data[2]/(float)MIDIMSG_DATA_BYTE_MAX,
            *note);
}

//<td> N1 Stride control </td>
//<td> Adjust the position advancement just as the POS knob does with the STRIDE/ABS/UNI switch in the upward position. </td>
/* note */
static void
synth_midi_cc_stride_control(void *data, MIDIMsg *msg)
{
    int *note = data;
    synth_control_set_positionStride(
            midi_util_map_midpoint_exact(msg->data[2],0,1),
            *note);
}

//<td> N1 Offset control </td>
//<td> Adjust the playback offset just as the OFST knob does. </td>
/* note */
static void
synth_midi_cc_offset_control(void *data, MIDIMsg *msg)
{
    int *note = data;
    synth_control_set_offset(
            (float)msg->data[2]/(float)MIDIMSG_DATA_BYTE_MAX,
            *note);
}

//<td> N1 Fade control </td>
//<td> Adjust the fade just as the GAIN knob does with the FADE/GAIN/FBK switch in the upward position. </td>
/* note */
static void
synth_midi_cc_fbk_rate_control(void *data, MIDIMsg *msg)
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
static void
synth_midi_cc_event_delta_control(void *data, MIDIMsg *msg)
{
    int *note = data;
    synth_control_set_eventDelta_free(
            (float)msg->data[2]/(float)MIDIMSG_DATA_BYTE_MAX,
            *note);
}

//<td> N1 Number of repeats control </td>
//<td> Adjust the number of repeats (the number of notes in the arpeggio) just as the TMPO/REP knob does. * </td>
/* note */
static void
synth_midi_cc_num_reps_control(void *data, MIDIMsg *msg)
{
    int *note = data,
         nreps = msg->data[2];
    if (nreps < 0) { nreps = 0; }
    if (nreps > SYNTH_CONTROL_MAX_NUM_REPEATS) { nreps = SYNTH_CONTROL_MAX_NUM_REPEATS; }
    synth_control_set_numRepeats(nreps, *note);
}

//<td> N1 Stride state </td>
// If 0, reset the note stride accumulator
// If 1, reset note stride
// If 2, reset position stride
// If >= 2, do everything
/* note */
static void
synth_midi_cc_stride_reset(void *data, MIDIMsg *msg)
{
    int *note = data;
    /* map to the middle */
    int datum = msg->data[2];
    if (datum > 2) { goto all; }
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

//<td> N1 skip control </td>
//<td> Control how often the arpeggio plays just like the &#x394; knob does with the FREE/QUANT/SKIP switch in the downward position. </td>
/* note */
static void
synth_midi_cc_interm_control(void *data, MIDIMsg *msg)
{
    int *note = data,
        i = msg->data[2];
    synth_control_set_intermittency_idx(i, *note);
}

//<td> N1 swing control </td>
//<td> Control the swing factor for the time between repeats of notes.</td>
/* note */
static void
synth_midi_cc_swing_control(void *data, MIDIMsg *msg)
{
    int *note = data;
    synth_control_set_swing((float)msg->data[2]/(float)MIDIMSG_DATA_BYTE_MAX,
                            *note);
}

static void
synth_midi_cc_note_stride_control(void *data, MIDIMsg *msg)
{
    int *note = data;
    synth_control_set_noteStride(midi_util_map_midpoint_exact(msg->data[2],0,1),
                                 *note);
}

//<td> Coarse tempo control </td>
//<td> Control how often the sequence plays in beats per minute (BPM) from 11 BPM to 60 BPM. </td>
/* global */
static void
synth_midi_cc_tempo_coarse_control(void *data, MIDIMsg *msg)
{
    synth_control_set_tempo_coarse_norm(midi_util_map_midpoint_exact(msg->data[2],0,1));
}

//<td> Fine tempo control </td>
//<td> Control how often the sequence plays by adding to the current tempo -10 to +10 BPM. </td>
/* global */
static void
synth_midi_cc_tempo_fine_control(void *data, MIDIMsg *msg)
{
    synth_control_set_tempo_fine_norm(midi_util_map_midpoint_exact(msg->data[2],0,1));
}

//<td> Tempo scaling </td>
//<td> Scale the tempo like the &#x394; knob with N1/N2/N3 in the upward position and FREE/QUANT/SKIP in the middle position. </td>
/* global */
static void
synth_midi_cc_tempo_scale_control(void *data, MIDIMsg *msg)
{
    synth_control_set_tempo_scale_norm((float)msg->data[2] / (float)MIDIMSG_DATA_BYTE_MAX);
}

//<td> Tempo nudge </td>
//<td> Slightly adjust the tempo like the TMPO/REP knob with N1/N2/N3 in the upward position and FREE/R=B/AREC in the middle position. </td>
/* global */
static void
synth_midi_cc_tempo_nudge_control(void *data, MIDIMsg *msg)
{
    synth_control_tempoNudge((float)msg->data[2] / (float)MIDIMSG_DATA_BYTE_MAX);
}

//<td> Preset store </td>
//<td> Store the current settings in a specified register. The message value of 0 stores in the PRE1 register, a value of 1 stores in the PRE2 register and a value of 2 stores in the PRE3 register. If greater than 2, stores in the PRE3 register. </td>
/* global */
static void
synth_midi_cc_preset_store_control(void *data, MIDIMsg *msg)
{
    sc_presets_store(msg->data[2]);
}

//<td> Preset recall </td>
//<td> Recall the current settings from a specified register. The message value of 0 recalls from the PRE1 register, a value of 1 recalls from the PRE2 register and a value of 2 recalls from the PRE3 register.  If greater than 2, recalls from the PRE3 register. </td>
/* global */
static void
synth_midi_cc_preset_recall_control(void *data, MIDIMsg *msg)
{
    sc_presets_recall(msg->data[2]);
}

//<td> Record enable/disable </td>
//<td> Start/stop recording. A message value of 0 stops recording. A message value greater than 0 starts or restarts recording depending on whether or not recording is already going on. </td>
/* global */
static void
synth_midi_cc_rec_control(void *data, MIDIMsg *msg)
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
static void
synth_midi_cc_play_control(void *data, MIDIMsg *msg)
{
    if (msg->data[2] > 0) {
        synth_control_schedulerState_on();
    } else {
        synth_control_schedulerState_off();
    }
}

//<td> Record mode </td>
//<td> Sets the record mode just like the FREE/R=B/AREC switch. A message value of 0 sets the FREE record mode, a value of 1 sets the R=B mode and a value of 2 sets the AREC mode. A value greater than 2 sets the AREC mode. </td>

static void
synth_midi_cc_rec_mode_control(void *data, MIDIMsg *msg)
{
    /* The last_recMode_param is NULL because we want to force setting this recording mode. */
    synth_control_set_recMode_onChange((SynthControlRecMode)msg->data[2],
                                       NULL);
}

static synth_midi_control_params_t *
synth_midi_cc_rec_mode_control_t_init(
        MIDI_Router_Standard *router,
        int midi_channel,
        unsigned int *cc)
{
    synth_midi_control_params_t *callback_storage = alloc_synth_midi_control_params_t();
    *callback_storage = (synth_midi_control_params_t) { .func = synth_midi_cc_rec_mode_control };
    if (!callback_storage) { goto fail; }
    MIDI_CC_CB_Router_addCB(&router->cbRouters[midi_channel],
            *cc,
            synth_midi_cc_rec_mode_control,
            NULL);
    *cc = *cc + 1;
    return callback_storage;
fail:
    return NULL;
}

//<td> Feedback state </td>
//<td> Sets the feedback state. Data byte of 0 Means off, 1 just N1 fedback, 2 all notes,
//otherwise does nothing </td>
/* global */
static void
synth_midi_cc_fbk_state_control(void *data, MIDIMsg *msg)
{
    synth_control_feedback_control(msg->data[2]);
}

static void
synth_midi_note_on_control(void *data, MIDIMsg *msg)
{
    float pitch, amplitude;
    pitch = msg->data[1] - SYNTH_CONTROL_PITCH_UNISON;
    amplitude = (float)msg->data[2] / (float)MIDIMSG_DATA_BYTE_MAX;
    amplitude = SYNTH_CONTROL_MIN_GAIN 
        + (SYNTH_CONTROL_MAX_GAIN - SYNTH_CONTROL_MIN_GAIN)*amplitude;
    amplitude = powf(10.,amplitude/20.);
    synth_control_one_shot(pitch,amplitude);
}

static void
synth_midi_note_on_init(
        MIDI_Router_Standard *router,
        int midi_channel)
{
    MIDI_Router_addCB(&router->router,
            MIDIMSG_NOTE_ON,
            midi_channel,
            synth_midi_note_on_control,
            NULL);
}

static void
synth_midi_syscom_control(void *data,
                          MIDIMsg *msg)
{
    if (((msg->data[0]) & 0x0f) == 0x08) {
        scheduler_incTimeAndDoEvents_midiclock();
    }
}

static void
synth_midi_syscom_control_init(
        MIDI_Router_Standard *router,
        int midi_channel)
{
    MIDI_Router_addCB(&router->router, 
            MIDIMSG_SYS_COMMON,
            midi_channel,
            synth_midi_syscom_control,
            NULL);
}

/*
initializes a MIDI control function that doesn't need any extra parameters,
so returns void
increments cc number
*/
static synth_midi_control_params_t *
synth_midi_cc_global_func_init(
        MIDI_Router_Standard *router,
        int midi_channel,
        void (*func)(void*,MIDIMsg*),
        unsigned int *cc)
{
    /* returns dummy address to indicate it worked */
    synth_midi_control_params_t *callback_storage = alloc_synth_midi_control_params_t();
    if (!callback_storage) { goto fail; }
    *callback_storage = (synth_midi_control_params_t) { .func = func };
    MIDI_CC_CB_Router_addCB(&router->cbRouters[midi_channel],
            /* cc number */
            *cc,
            func,
            NULL);
    *cc = *cc + 1;
    return callback_storage;
fail:
    return NULL;
}


/* initializes a function that takes a note as its parameter 
increments cc number
*/
static synth_midi_control_params_t *
synth_midi_cc_note_func_init(
        MIDI_Router_Standard *router,
        int midi_channel,
        void (*func)(void*,MIDIMsg*),
        unsigned int *cc,
        int note)
{
    synth_midi_cc_note_control_t *note_ctl = alloc_synth_midi_cc_note_control_t();
    if (!note_ctl) { goto fail; }
    *note_ctl = (synth_midi_cc_note_control_t) {
        .super = (synth_midi_control_params_t) { .func = func },
        .note = note
    };
    MIDI_CC_CB_Router_addCB(&router->cbRouters[midi_channel],
            /* cc number */
            *cc,
            func,
            &note_ctl->note);
    *cc = *cc + 1;
    return (synth_midi_control_params_t*)note_ctl;
fail:
    return NULL;
}

/*
initializes a function that takes a note and pitch as its parameter 
increments cc number
*/
static synth_midi_control_params_t *
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
        .super = (synth_midi_control_params_t) { .func = func },
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
    return (synth_midi_control_params_t*)p_note_pitch;
fail:
    return NULL;
}

static int midi_channel_was_oob = 0;
static const synth_midi_control_params_t **p_midi_cc_controls;
static synth_midi_control_params_t midi_controls_end;

const synth_midi_control_params_t **
synth_midi_control_get_midi_cc_controls() { return p_midi_cc_controls; }

synth_midi_control_params_t *
synth_midi_control_get_p_midi_contols_end() { return &midi_controls_end; }

void
synth_midi_control_setup(int midi_channel)
{
    /* This shouldn't happen but if midi_channel is out of range, we put it in range */
    if ((midi_channel < 0) || (midi_channel >= 16)) {
        midi_channel = 0;
        midi_channel_was_oob = 1;
    }
    /* note: midiRouter from inc/midi_setup.h */
    static unsigned int cc = 0;
    /* We store the callbacks and related information here for debugging purposes. */
    const synth_midi_control_params_t *midi_cc_controls[] = {
        /* Note 1 group */
        // 0 
        // N1 Pitch 1 control (fine) 
        synth_midi_cc_note_pitch_func_init(&midiRouter, midi_channel, synth_midi_cc_pitch_fine_control, &cc, 0, 0),
        // 1 
        // N1 Pitch 2 control (fine) 
        synth_midi_cc_note_pitch_func_init(&midiRouter, midi_channel, synth_midi_cc_pitch_fine_control, &cc, 0, 1),
        // 2 
        // N1 Pitch 3 control (fine) 
        synth_midi_cc_note_pitch_func_init(&midiRouter, midi_channel, synth_midi_cc_pitch_fine_control, &cc, 0, 2),
        // 3 
        // N1 Envelope control 
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_env_control, &cc, 0),
        // 4 
        // N1 Length control 
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_sus_control, &cc, 0),
        // 5 
        // N1 Pitch 1 control 
        synth_midi_cc_note_pitch_func_init(&midiRouter, midi_channel, synth_midi_cc_pitch_control, &cc, 0,0),
        // 6 
        // N1 Pitch 2 control 
        synth_midi_cc_note_pitch_func_init(&midiRouter, midi_channel, synth_midi_cc_pitch_control, &cc, 0,1),
        // 7 
        // N1 Pitch 3 control 
        synth_midi_cc_note_pitch_func_init(&midiRouter, midi_channel, synth_midi_cc_pitch_control, &cc, 0,2),
        // 8 
        // N1 Gain control 
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_gain_control, &cc, 0),
        // 9 
        // N1 Position control 
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_pos_control, &cc, 0),
        // 10 
        // N1 Stride control 
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_stride_control, &cc, 0),
        // 11 
        // N1 Offset control 
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_offset_control, &cc, 0),
        // 12 
        // N1 Fade control 
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_fbk_rate_control, &cc, 0),
        // 13 
        // N1 Free &#x394; control 
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_event_delta_control, &cc, 0),
        // 14 
        // N1 Number of repeats control 
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_num_reps_control, &cc, 0),
        // 15 
        // N1 Stride reset 
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_stride_reset, &cc, 0),
        // 16 
        // N1 skip control 
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_interm_control, &cc, 0),
        // 17 
        // N1 swing control 
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_swing_control, &cc, 0),

        /* Note 2 group */
        // 18 
        // N2 Pitch 1 control (fine) 
        synth_midi_cc_note_pitch_func_init(&midiRouter, midi_channel, synth_midi_cc_pitch_fine_control, &cc, 1, 0),
        // 19 
        // N2 Pitch 2 control (fine) 
        synth_midi_cc_note_pitch_func_init(&midiRouter, midi_channel, synth_midi_cc_pitch_fine_control, &cc, 1, 1),
        // 20
        // N2 Pitch 3 control (fine) 
        synth_midi_cc_note_pitch_func_init(&midiRouter, midi_channel, synth_midi_cc_pitch_fine_control, &cc, 1, 2),
        // 21 
        // N2 Envelope control 
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_env_control, &cc, 1),
        // 22 
        // N2 Length control 
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_sus_control, &cc, 1),
        // 23 
        // N2 Pitch 1 control 
        synth_midi_cc_note_pitch_func_init(&midiRouter, midi_channel, synth_midi_cc_pitch_control, &cc, 1,0),
        // 24 
        // N2 Pitch 2 control 
        synth_midi_cc_note_pitch_func_init(&midiRouter, midi_channel, synth_midi_cc_pitch_control, &cc, 1,1),
        // 25 
        // N2 Pitch 3 control 
        synth_midi_cc_note_pitch_func_init(&midiRouter, midi_channel, synth_midi_cc_pitch_control, &cc, 1,2),
        // 26 
        // N2 Gain control 
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_gain_control, &cc, 1),
        // 27 
        // N2 Position control 
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_pos_control, &cc, 1),
        // 28 
        // N2 Stride control 
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_stride_control, &cc, 1),
        // 29 
        // N2 Offset control 
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_offset_control, &cc, 1),
        // 30
        // N2 Fade control 
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_fbk_rate_control, &cc, 1),
        // 31 
        // N2 Free &#x394; control 
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_event_delta_control, &cc, 1),
        // 32 
        // N2 Number of repeats control 
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_num_reps_control, &cc, 1),
        // 33 
        // N2 Stride reset 
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_stride_reset, &cc, 1),
        // 34 
        // N2 skip control 
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_interm_control, &cc, 1),
        // 35 
        // N2 swing control 
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_swing_control, &cc, 1),

        /* Note 3 group */
        // 36 
        // N3 Pitch 1 control (fine) 
        synth_midi_cc_note_pitch_func_init(&midiRouter, midi_channel, synth_midi_cc_pitch_fine_control, &cc, 2, 0),
        // 37 
        // N3 Pitch 2 control (fine) 
        synth_midi_cc_note_pitch_func_init(&midiRouter, midi_channel, synth_midi_cc_pitch_fine_control, &cc, 2, 1),
        // 38 
        // N3 Pitch 3 control (fine) 
        synth_midi_cc_note_pitch_func_init(&midiRouter, midi_channel, synth_midi_cc_pitch_fine_control, &cc, 2, 2),
        // 39 
        // N3 Envelope control 
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_env_control, &cc, 2),
        // 40 
        // N3 Length control 
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_sus_control, &cc, 2),
        // 41 
        // N3 Pitch 1 control 
        synth_midi_cc_note_pitch_func_init(&midiRouter, midi_channel, synth_midi_cc_pitch_control, &cc, 2,0),
        // 42 
        // N3 Pitch 2 control 
        synth_midi_cc_note_pitch_func_init(&midiRouter, midi_channel, synth_midi_cc_pitch_control, &cc, 2,1),
        // 43 
        // N3 Pitch 3 control 
        synth_midi_cc_note_pitch_func_init(&midiRouter, midi_channel, synth_midi_cc_pitch_control, &cc, 2,2),
        // 44 
        // N3 Gain control 
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_gain_control, &cc, 2),
        // 45 
        // N3 Position control 
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_pos_control, &cc, 2),
        // 46 
        // N3 Stride control 
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_stride_control, &cc, 2),
        // 47 
        // N3 Offset control 
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_offset_control, &cc, 2),
        // 48 
        // N3 Fade control 
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_fbk_rate_control, &cc, 2),
        // 49 
        // N3 Free &#x394; control 
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_event_delta_control, &cc, 2),
        // 50 
        // N3 Number of repeats control 
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_num_reps_control, &cc, 2),
        // 51 
        // N3 Stride reset 
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_stride_reset, &cc, 2),
        // 52 
        // N3 skip control 
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_interm_control, &cc, 2),
        // 53 
        // N3 swing control 
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_swing_control, &cc, 2),
        // 54
        // Coarse tempo control 
        synth_midi_cc_global_func_init(&midiRouter, midi_channel,synth_midi_cc_tempo_coarse_control,&cc),
        // 55 
        // Fine tempo control 
        synth_midi_cc_global_func_init(&midiRouter, midi_channel,synth_midi_cc_tempo_fine_control,&cc),
        // 56 
        // Tempo scaling 
        synth_midi_cc_global_func_init(&midiRouter, midi_channel,synth_midi_cc_tempo_scale_control,&cc),
        // 57 
        // Tempo nudge 
        synth_midi_cc_global_func_init(&midiRouter, midi_channel,synth_midi_cc_tempo_nudge_control,&cc),
        // 58 
        // Preset store 
        synth_midi_cc_global_func_init(&midiRouter, midi_channel,synth_midi_cc_preset_store_control,&cc),
        // 59 
        // Preset recall 
        synth_midi_cc_global_func_init(&midiRouter, midi_channel,synth_midi_cc_preset_recall_control,&cc),
        // 60 
        // Record enable/disable 
        synth_midi_cc_global_func_init(&midiRouter, midi_channel,synth_midi_cc_rec_control,&cc),
        // 61 
        // Play start 
        synth_midi_cc_global_func_init(&midiRouter, midi_channel,synth_midi_cc_play_control,&cc),
        // 62 
        // Record mode 
        synth_midi_cc_rec_mode_control_t_init(&midiRouter, midi_channel, &cc),
        // 63 
        // Feedback state 
        synth_midi_cc_global_func_init(&midiRouter, midi_channel, synth_midi_cc_fbk_state_control, &cc),
        // 64
        // Note stride
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_note_stride_control, &cc, 0),
        // 65
        // Note stride
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_note_stride_control, &cc, 1),
        // 66
        // Note stride
        synth_midi_cc_note_func_init(&midiRouter, midi_channel, synth_midi_cc_note_stride_control, &cc, 2),
        &midi_controls_end
    };
    p_midi_cc_controls = midi_cc_controls;
    synth_midi_note_on_init(&midiRouter, midi_channel);
    synth_midi_syscom_control_init(&midiRouter, midi_channel);
}

