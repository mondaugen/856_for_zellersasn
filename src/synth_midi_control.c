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

void synth_midi_cc_stride_state_control(void *data, MIDIMsg *msg)
{
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
} 
