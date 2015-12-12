#include "midi_setup.h"
#include "synth_control.h" 

#define SYNTH_MIDI_NUM_NOTE_PARAMS 15
typedef enum {
    synth_midi_cc_type_t_PITCH_FINE = 0,
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

typedef struct __synth_midi_cc_pitch_fine_control_t {
    int note;
    int pitch;
} synth_midi_cc_pitch_fine_control_t;


void synth_midi_cc_pitch_fine_control(void *data, MIDIMsg *msg)
{
    synth_midi_cc_pitch_fine_control_t *params =
        (synth_midi_cc_pitch_fine_control_t*)data;
    synth_control_set_pitch_fine_quant((float)msg->data[2] 
            / (float)MIDI_MSG_DATA_BYTE_MAX,
            params->note,
            params->pitch);
}

static void synth_midi_cc_pitch_fine_control_t_init(
        MIDI_Router_Standard *router,
        int midi_channel,
        synth_midi_cc_pitch_fine_control_t *controls,
        size_t num_params,
        size_t num_pitches)
{
    size_t n, m;
    for (n = 0; n < num_params; n++) {
        for (m = 0; m < num_pitches; m++) {
            controls[n*num_pitches + m].note = n;
            controls[n*num_pitches + m].pitch = m;
            MIDI_CC_CB_Router_addCB(&router->cbRouters[midi_channel],
                    /* cc number */
                    n * SYNTH_MIDI_NUM_NOTE_PARAMS 
                        + synth_midi_cc_type_t_PITCH1
                        + m,
                    synth_midi_cc_pitch_fine_control,
                    &controls[n*num_pitches + m]);
        }
    }
}

void synth_midi_cc_env_control(void *data, MIDIMsg *msg)
{
}

void synth_midi_cc_sus_control(void *data, MIDIMsg *msg)
{
}

void synth_midi_cc_pitch1_control(void *data, MIDIMsg *msg)
{
}

void synth_midi_cc_pitch2_control(void *data, MIDIMsg *msg)
{
}

void synth_midi_cc_pitch3_control(void *data, MIDIMsg *msg)
{
}

void synth_midi_cc_gain_control(void *data, MIDIMsg *msg)
{
}

void synth_midi_cc_pos_control(void *data, MIDIMsg *msg)
{
}

void synth_midi_cc_stride_control(void *data, MIDIMsg *msg)
{
}

void synth_midi_cc_offset_control(void *data, MIDIMsg *msg)
{
}

void synth_midi_cc_fbk_rate_control(void *data, MIDIMsg *msg)
{
}

void synth_midi_cc_event_delta_control(void *data, MIDIMsg *msg)
{
}

void synth_midi_cc_num_reps_control(void *data, MIDIMsg *msg)
{
}

typedef struct __synth_midi_cc_stride_state_control {
    int note;
    SynthControlPosMode last_state;
} synth_midi_cc_stride_state_control_t;

void synth_midi_cc_stride_state_control(void *data, MIDIMsg *msg)
{
    synth_midi_cc_stride_state_control_t _params;
    _params = (synth_midi_cc_stride_state_control_t*)data;
    SynthControlPosMode _posMode = (msg->data[2] > 0) 
        ? SynthControlPosMode_STRIDE
        : SynthControlPosMode_ABSOLUTE;
    synth_control_set_posMode_onChange(_posMode,
            &_params->last_state,
            _params->note);
}

synth_midi_cc_stride_state_control_t_init(
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
}

void synth_midi_cc_tempo_coarse_control(void *data, MIDIMsg *msg)
{
}

void synth_midi_cc_tempo_fine_control(void *data, MIDIMsg *msg)
{
}

void synth_midi_cc_tempo_scale_control(void *data, MIDIMsg *msg)
{
}

void synth_midi_cc_tempo_nudge_control(void *data, MIDIMsg *msg)
{
}

void synth_midi_cc_preset_store_control(void *data, MIDIMsg *msg)
{
}

void synth_midi_cc_preset_recall_control(void *data, MIDIMsg *msg)
{
}

void synth_midi_cc_rec_control(void *data, MIDIMsg *msg)
{
}

void synth_midi_cc_play_control(void *data, MIDIMsg *msg)
{
}

void synth_midi_cc_rec_mode_control(void *data, MIDIMsg *msg)
{
}

void synth_midi_cc_fbk_state_control(void *data, MIDIMsg *msg)
{
}

void synth_midi_control_setup(void)
{
    int midi_channel;
    midi_channel = SYNTH_MIDI_CONTROL_DEFAULT_CHANNEL;
    /* could set custom channel here ... */
    static synth_midi_cc_pitch_fine_control_t 
        fine_pitch_params[NUM_NOTE_PARAM_SETS 
            * SYNTH_CONTROL_PITCH_TABLE_SIZE];
    synth_midi_cc_pitch_fine_control_t_init(
            &midiRouter,
            midi_channel,
            fine_pitch_params,
            NUM_NOTE_PARAM_SETS,
            SYNTH_CONTROL_PITCH_TABLE_SIZE);
    static synth_midi_cc_stride_state_control_t
        stride_state_params[NUM_NOTE_PARAM_SETS];
    synth_midi_cc_stride_state_control_t_init(
            &midiRouter,
            midi_channel,
            stride_state_params,
            NUM_NOTE_PARAM_SETS);
}
