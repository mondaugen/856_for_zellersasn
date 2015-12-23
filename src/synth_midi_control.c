#include "synth_midi_control.h" 
#include "midi_setup.h"
#include "synth_control.h" 

#define SYNTH_MIDI_NUM_NOTE_PARAMS 17
typedef enum {
    synth_midi_cc_type_t_PITCH_FINE1 = 0,
    synth_midi_cc_type_t_PITCH_FINE2,
    synth_midi_cc_type_t_PITCH_FINE3,
    synth_midi_cc_type_t_ENV,
    synth_midi_cc_type_t_SUS,
    synth_midi_cc_type_t_PITCH1,
    synth_midi_cc_type_t_PITCH2,
    synth_midi_cc_type_t_PITCH3,
    synth_midi_cc_type_t_GAIN,
    synth_midi_cc_type_t_POS,
    synth_midi_cc_type_t_STRIDE,
    synth_midi_cc_type_t_OFFSET,
    synth_midi_cc_type_t_FBK_RATE,
    synth_midi_cc_type_t_EVENT_DELTA,
    synth_midi_cc_type_t_NUM_REPS,
    synth_midi_cc_type_t_STRIDE_STATE,
    synth_midi_cc_type_t_INTERM,
    synth_midi_cc_type_t_TEMPO_COARSE 
        = (SYNTH_MIDI_NUM_NOTE_PARAMS * NUM_NOTE_PARAM_SETS),
    synth_midi_cc_type_t_TEMPO_FINE,
    synth_midi_cc_type_t_TEMPO_SCALE,
    synth_midi_cc_type_t_TEMPO_NUDGE,
    synth_midi_cc_type_t_PRESET_STORE,
    synth_midi_cc_type_t_PRESET_RECALL,
    synth_midi_cc_type_t_REC,
    synth_midi_cc_type_t_PLAY,
    synth_midi_cc_type_t_REC_MODE,
    synth_midi_cc_type_t_FBK_STATE
} synth_midi_cc_type_t;

static void synth_midi_note_param_indices_init(int *indices, size_t len)
{
    while (len--) {
        indices[len] = len;
    }
}

typedef struct __synth_midi_cc_pitch_control_t {
    int note;
    int pitch;
} synth_midi_cc_pitch_control_t;


void synth_midi_cc_pitch_fine_control(void *data, MIDIMsg *msg)
{
    synth_midi_cc_pitch_control_t *params =
        (synth_midi_cc_pitch_control_t*)data;
    synth_control_set_pitch_fine_quant((float)msg->data[2] 
            / (float)MIDIMSG_DATA_BYTE_MAX,
            params->note,
            params->pitch);
}

static void synth_midi_cc_pitch_control_t_init(
        MIDI_Router_Standard *router,
        int midi_channel,
        synth_midi_cc_pitch_control_t *controls,
        size_t num_params,
        size_t num_pitches,
        void (**funcs)(void*,MIDIMsg*),
        synth_midi_cc_type_t *types)
{
    size_t n, m;
    for (n = 0; n < num_params; n++) {
        for (m = 0; m < num_pitches; m++) {
            controls[n*num_pitches + m].note = n;
            controls[n*num_pitches + m].pitch = m;
            while (*funcs != NULL) {
                MIDI_CC_CB_Router_addCB(&router->cbRouters[midi_channel],
                        /* cc number */
                        n * SYNTH_MIDI_NUM_NOTE_PARAMS 
                        + *types
                        + m,
                        *funcs,
                        &controls[n*num_pitches + m]);
                funcs++;
                types++;
            }
        }
    }
}

void synth_midi_cc_env_control(void *data, MIDIMsg *msg)
{
    int *note = (int*)data;
    synth_control_set_envelopeTime(
            (float)msg->data[2]/(float)MIDIMSG_DATA_BYTE_MAX,
            *note);
}

void synth_midi_cc_sus_control(void *data, MIDIMsg *msg)
{
    int *note = (int*)data;
    synth_control_set_sustainTime(
            (float)msg->data[2]/(float)MIDIMSG_DATA_BYTE_MAX,
            *note);
}

void synth_midi_cc_pitch_control(void *data, MIDIMsg *msg)
{
}

void synth_midi_cc_gain_control(void *data, MIDIMsg *msg)
{
    int *note = (int*)data;
    synth_control_set_wet(
            (float)msg->data[2]/(float)MIDIMSG_DATA_BYTE_MAX,
            *note);
}

void synth_midi_cc_pos_control(void *data, MIDIMsg *msg)
{
    int *note = (int*)data;
    synth_control_set_startPoint(
            (float)msg->data[2]/(float)MIDIMSG_DATA_BYTE_MAX,
            *note);
}

void synth_midi_cc_stride_control(void *data, MIDIMsg *msg)
{
    int *note = (int*)data;
    synth_control_set_positionStride(
            (float)msg->data[2]/(float)MIDIMSG_DATA_BYTE_MAX,
            *note);
}

void synth_midi_cc_offset_control(void *data, MIDIMsg *msg)
{
    int *note = (int*)data;
    synth_control_set_offset(
            (float)msg->data[2]/(float)MIDIMSG_DATA_BYTE_MAX,
            *note);
}

void synth_midi_cc_fbk_rate_control(void *data, MIDIMsg *msg)
{
    int *note = (int*)data;
    synth_control_set_ampLastEcho(
            (float)msg->data[2]/(float)MIDIMSG_DATA_BYTE_MAX,
            *note);
}

void synth_midi_cc_event_delta_control(void *data, MIDIMsg *msg)
{
    int *note = (int*)data;
    synth_control_set_eventDelta_free(
            (float)msg->data[2]/(float)MIDIMSG_DATA_BYTE_MAX,
            *note);
}

void synth_midi_cc_num_reps_control(void *data, MIDIMsg *msg)
{
    int *note = (int*)data;
    synth_control_set_numRepeats(
            (float)msg->data[2]/(float)MIDIMSG_DATA_BYTE_MAX,
            *note);
}

typedef struct __synth_midi_cc_stride_state_control {
    int note;
    SynthControlPosMode last_state;
} synth_midi_cc_stride_state_control_t;

void synth_midi_cc_stride_state_control(void *data, MIDIMsg *msg)
{
    synth_midi_cc_stride_state_control_t *_params;
    _params = (synth_midi_cc_stride_state_control_t*)data;
    SynthControlPosMode _posMode = (msg->data[2] > 0) 
        ? SynthControlPosMode_STRIDE
        : SynthControlPosMode_ABSOLUTE;
    synth_control_set_posMode_onChange(_posMode,
            &_params->last_state,
            _params->note);
}

void synth_midi_cc_stride_state_control_t_init(
        MIDI_Router_Standard *router,
        int midi_channel,
        synth_midi_cc_stride_state_control_t *controls,
        int num_params)
{
    int n;
    for (n = 0; n < num_params; n++) {
        controls[n].note = n;
        controls[n].last_state = SynthControlPosMode_ABSOLUTE;
        MIDI_CC_CB_Router_addCB(&router->cbRouters[midi_channel],
                /* cc number */
                n * SYNTH_MIDI_NUM_NOTE_PARAMS 
                    + synth_midi_cc_type_t_STRIDE_STATE,
                synth_midi_cc_stride_state_control,
                &controls[n]);
    }
}

void synth_midi_cc_interm_control(void *data, MIDIMsg *msg)
{
    int *note = (int*)data;
    synth_control_set_intermittency(
            (float)msg->data[2]/(float)MIDIMSG_DATA_BYTE_MAX,
            *note);
}

void synth_midi_cc_tempo_coarse_control(void *data, MIDIMsg *msg)
{
    synth_control_set_tempo_coarse_norm((float)msg->data[2] / (float)MIDIMSG_DATA_BYTE_MAX);
}

void synth_midi_cc_tempo_fine_control(void *data, MIDIMsg *msg)
{
    synth_control_set_tempo_fine_norm((float)msg->data[2] / (float)MIDIMSG_DATA_BYTE_MAX);
}

void synth_midi_cc_tempo_scale_control(void *data, MIDIMsg *msg)
{
    synth_control_set_tempo_scale_norm((float)msg->data[2] / (float)MIDIMSG_DATA_BYTE_MAX);
}

void synth_midi_cc_tempo_nudge_control(void *data, MIDIMsg *msg)
{
    synth_control_tempoNudge((float)msg->data[2] / (float)MIDIMSG_DATA_BYTE_MAX);
}

void synth_midi_cc_preset_store_control(void *data, MIDIMsg *msg)
{
    sc_presets_store(msg->data[2]);
}

void synth_midi_cc_preset_recall_control(void *data, MIDIMsg *msg)
{
    sc_presets_recall(msg->data[2]);
}

void synth_midi_cc_rec_control(void *data, MIDIMsg *msg)
{
    if (msg->data[2] > 0) {
        synth_control_record_start();
    } else {
        synth_control_record_stop();
    }
}

void synth_midi_cc_play_control(void *data, MIDIMsg *msg)
{
    if (msg->data[2] > 0) {
        synth_control_schedulerState_on();
    } else {
        synth_control_schedulerState_off();
    }
}

typedef SynthControlRecMode synth_midi_cc_rec_mode_control_t;

void synth_midi_cc_rec_mode_control(void *data, MIDIMsg *msg)
{
    synth_midi_cc_rec_mode_control_t *last_recMode_param = 
        (synth_midi_cc_rec_mode_control_t*)data;
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

void synth_midi_cc_fbk_state_control(void *data, MIDIMsg *msg)
{
    synth_control_feedback_control(msg->data[2]);
}

/* Parameter set chosen by channel */
void synth_midi_note_on_control(void *data, MIDIMsg *msg)
{
    int *base_channel = (int*)data;
    float pitch, amplitude;
    pitch = msg->data[1] - SYNTH_CONTROL_PITCH_UNISON;
    /* TODO: use dB scale for more "natural" amplitude variation */
    amplitude = (float)msg->data[2] / (float)MIDIMSG_DATA_BYTE_MAX;
    synth_control_note_on(MIDIMSG_GET_CHANNEL(msg->data[0]) - *base_channel,
                          pitch,
                          amplitude);
}

static void synth_midi_cc_note_funcs_init(
        MIDI_Router_Standard *router,
        int midi_channel,
        void (**funcs)(void*,MIDIMsg*),
        synth_midi_cc_type_t *types,
        int *param_set_indices,
        int num_param_sets)
{
    int n;
    while (*funcs != NULL) {
        for (n = 0; n < num_param_sets; n++) {
            MIDI_CC_CB_Router_addCB(&router->cbRouters[midi_channel],
                    /* cc number */
                    n * SYNTH_MIDI_NUM_NOTE_PARAMS 
                        + *types,
                    *funcs,
                    &param_set_indices[n]);
        }
        funcs++;
        types++;
    }
}

static void synth_midi_cc_global_funcs_init(
        MIDI_Router_Standard *router,
        int midi_channel,
        void (**funcs)(void*,MIDIMsg*),
        synth_midi_cc_type_t *types)
{
    while (*funcs != NULL) {
        MIDI_CC_CB_Router_addCB(&router->cbRouters[midi_channel],
                /* cc number */
                *types,
                *funcs,
                NULL);
        funcs++;
        types++;
    }
}

/* Assigns to 3 channels, above and including midi_channel, wrapping around if
 * the channel exceeds the max MIDI channel */
static void synth_midi_note_on_init(
        MIDI_Router_Standard *router,
        int midi_channel,
        void (*func)(void*,MIDIMsg*),
        int *base_channel)
{
    int n;
    for (n = 0; n < NUM_NOTE_PARAM_SETS; n++) {
        MIDI_Router_addCB(&router->router,
                MIDIMSG_NOTE_ON,
                (midi_channel + n) % MIDIMSG_NCHANNELS,
                func,
                base_channel);
    }
}

void synth_midi_control_setup(void)
{
    int midi_channel = SYNTH_MIDI_CONTROL_DEFAULT_CHANNEL;
    static int base_channel = SYNTH_MIDI_CONTROL_DEFAULT_CHANNEL;
    /* could set custom channel here ... */
    void (*midi_cc_pitch_funcs[])(void*,MIDIMsg*) = {
        synth_midi_cc_pitch_control,
        synth_midi_cc_pitch_fine_control,
        NULL
    };
    synth_midi_cc_type_t midi_cc_pitch_types[] = {
        synth_midi_cc_type_t_PITCH1,
        synth_midi_cc_type_t_PITCH_FINE1
    };
    static synth_midi_cc_pitch_control_t 
        pitch_params[NUM_NOTE_PARAM_SETS 
            * SYNTH_CONTROL_PITCH_TABLE_SIZE];
    synth_midi_cc_pitch_control_t_init(
            &midiRouter,
            midi_channel,
            pitch_params,
            NUM_NOTE_PARAM_SETS,
            SYNTH_CONTROL_PITCH_TABLE_SIZE,
            midi_cc_pitch_funcs,
            midi_cc_pitch_types);
    static synth_midi_cc_stride_state_control_t
        stride_state_params[NUM_NOTE_PARAM_SETS];
    synth_midi_cc_stride_state_control_t_init(
            &midiRouter,
            midi_channel,
            stride_state_params,
            NUM_NOTE_PARAM_SETS);
    static synth_midi_cc_rec_mode_control_t
        rec_mode_param;
    synth_midi_cc_rec_mode_control_t_init(
            &midiRouter,
            midi_channel,
            &rec_mode_param);
    static int note_param_indices[NUM_NOTE_PARAM_SETS];
    synth_midi_note_param_indices_init(note_param_indices,
            NUM_NOTE_PARAM_SETS);
    void (*midi_cc_note_funcs[])(void*,MIDIMsg*) = {
            synth_midi_cc_env_control,
            synth_midi_cc_sus_control,
            synth_midi_cc_gain_control,
            synth_midi_cc_pos_control,
            synth_midi_cc_stride_control,
            synth_midi_cc_offset_control,
            synth_midi_cc_fbk_rate_control,
            synth_midi_cc_event_delta_control,
            synth_midi_cc_num_reps_control,
            synth_midi_cc_interm_control,
            NULL
    };
    synth_midi_cc_type_t midi_cc_note_types[] = {
            synth_midi_cc_type_t_ENV,
            synth_midi_cc_type_t_SUS,
            synth_midi_cc_type_t_GAIN,
            synth_midi_cc_type_t_POS,
            synth_midi_cc_type_t_STRIDE,
            synth_midi_cc_type_t_OFFSET,
            synth_midi_cc_type_t_FBK_RATE,
            synth_midi_cc_type_t_EVENT_DELTA,
            synth_midi_cc_type_t_NUM_REPS,
            synth_midi_cc_type_t_INTERM,
    };
    synth_midi_cc_note_funcs_init(
        &midiRouter,
        midi_channel,
        midi_cc_note_funcs,
        midi_cc_note_types,
        note_param_indices, 
        NUM_NOTE_PARAM_SETS);
    void (*midi_cc_funcs[])(void*,MIDIMsg*) = {
        synth_midi_cc_tempo_coarse_control,
        synth_midi_cc_tempo_fine_control,
        synth_midi_cc_tempo_scale_control,
        synth_midi_cc_tempo_nudge_control,
        synth_midi_cc_preset_store_control,
        synth_midi_cc_preset_recall_control,
        synth_midi_cc_rec_control,
        synth_midi_cc_play_control,
        synth_midi_cc_fbk_state_control,
        NULL
    };
    synth_midi_cc_type_t midi_cc_types[] = {
        synth_midi_cc_type_t_TEMPO_COARSE,
        synth_midi_cc_type_t_TEMPO_FINE,
        synth_midi_cc_type_t_TEMPO_SCALE,
        synth_midi_cc_type_t_TEMPO_NUDGE,
        synth_midi_cc_type_t_PRESET_STORE,
        synth_midi_cc_type_t_PRESET_RECALL,
        synth_midi_cc_type_t_REC,
        synth_midi_cc_type_t_PLAY,
        synth_midi_cc_type_t_FBK_STATE
    };
    synth_midi_cc_global_funcs_init(
        &midiRouter,
        midi_channel,
        midi_cc_funcs,
        midi_cc_types);
    synth_midi_note_on_init(
        &midiRouter,
        midi_channel,
        synth_midi_note_on_control,
        &base_channel);
}
