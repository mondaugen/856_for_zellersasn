/* Copyright (c) 2016 Nicholas Esterer. All rights reserved. */

#include "adc_channel.h" 
#include "synth_adc_control.h" 
#include "synth_control.h"
#include "switches.h"

static float synth_adc_scale_thresh(float x);

#define SYNTH_ADC_CONTROL_FLOAT(name)\
    static void synth_adc_ ## name ## _control(adc_channel_t *chan,\
                                               adc_channel_do_data_t *data)\
    {\
         synth_control_set_ ## name (synth_adc_scale_thresh(chan->cur_val));\
    }

typedef struct synth_adc_channel_do_data_t synth_adc_channel_do_data_t;
struct synth_adc_channel_do_data_t {
    adc_channel_do_data_t super;
    /* The function to call normally (when FBK is not held down) */
    adc_channel_do_func_t normal_func;
};

struct synth_adc_obj_t {
    adc_channel_t channel;
    adc_channel_do_set_t do_set;
    synth_adc_channel_do_data_t channel_data;
};

/* Check if FBK switch is down. If it is, when you turn a knob it sets that as
   the parameter the expression pedal controls, otherwise it just calls the
   normal_func contained in synth_adc_channel_do_data_t. */
static void synth_adc_check_fbk_and_call(adc_channel_t *chan,
                                  adc_channel_do_data_t *data);

/* Check if expression pedal plugged in and if it is, call the function it
   controls, otherwise do nothing */
static void synth_adc_check_expr_and_call(adc_channel_t *chan,
                                  adc_channel_do_data_t *data)
{
    synth_adc_channel_do_data_t *sadc = (synth_adc_channel_do_data_t*)data;
    /* check if expression plugged in  */
    if (expsw_get_state()) {
        sadc->normal_func(chan,data);
    }
    /* Otherwise do nothing */
}

void
synth_adc_obj_init(struct synth_adc_obj_t *sao,
        size_t data_start_idx,
        adc_channel_do_func_t func,
        adc_channel_do_func_t normal_func)
{
    adc_channel_do_data_init((adc_channel_do_data_t*)&sao->channel_data,
            adc_channel_do_style_CHANGED_INIT,
            SYNTH_ADC_THRESHOLD,
            0);
        adc_channel_init(&sao->channel,
                adc_data_starts[data_start_idx],
                adc_raw_value_strides[data_start_idx],
                ADC_AVG_SIZE);
        adc_channel_do_set_init(&sao->do_set,
                &sao->channel,
                func,
                (adc_channel_do_data_t*)&sao->channel_data);
        adc_channel_do_set_add(&sao->do_set);
        sao->channel_data.normal_func = normal_func;
}

SYNTH_ADC_CONTROL_FLOAT(envelopeTime_curParams);
SYNTH_ADC_CONTROL_FLOAT(sustainTime_curParams);
SYNTH_ADC_CONTROL_FLOAT(offset_curParams);

static float synth_adc_scale_thresh(float x)
{
    float result;
    result = (x/((float)(ADC_MAX)) - SYNTH_ADC_LOWER_THRESH);
    result /= (SYNTH_ADC_UPPER_THRESH - SYNTH_ADC_LOWER_THRESH);
    if (result < 0.) {
        return 0.;
    }
    if (result > 1.) {
        return 1.;
    }
    return result;
}

static void synth_adc_pos_curParams_control(adc_channel_t *chan,
                                  adc_channel_do_data_t *data)
{
    switch (synth_control_get_posMode_curParams()) {
        case SynthControlPosMode_ABSOLUTE:
            synth_control_set_startPoint_curParams(synth_adc_scale_thresh(chan->cur_val));
            break;
        case SynthControlPosMode_UNI:
            synth_control_set_positionStride_curParams(
                    synth_adc_scale_thresh(chan->cur_val));
            /* prevent pitches from resetting */
            synth_control_set_uni_stuff_changed();
            break;
        case SynthControlPosMode_STRIDE:
            synth_control_set_noteStride_curParams(
                    synth_adc_scale_thresh(chan->cur_val));
            break;
    }
}

static void synth_adc_eventDelta_curParams_control(adc_channel_t *chan,
                                         adc_channel_do_data_t *data)
{
    if (synth_control_get_posMode_curParams() == SynthControlPosMode_UNI) {
        /* Only do something on note 2 and 3 */
        if (synth_control_get_editingWhichParams() != 0) {
            synth_control_set_uni_stuff_changed();
            synth_control_set_swing_curParams(
                    synth_adc_scale_thresh(chan->cur_val));
        }
    } else {
        switch (synth_control_get_deltaButtonMode()) {
            case SynthControlDeltaButtonMode_EVENT_DELTA_QUANT:
                if (synth_control_get_editingWhichParams() == 0) {
                    /* If editing 0th params, note delta controls tempo scaling */
                    synth_control_set_tempo_scale_norm(
                        synth_adc_scale_thresh(chan->cur_val));
                } else {
                    /* Otherwise it controls the event delta in a quantized way */
                    synth_control_set_eventDelta_quant_curParams(
                        synth_adc_scale_thresh(chan->cur_val));
                }
                break;
            case SynthControlDeltaButtonMode_EVENT_DELTA_FREE:
                if (synth_control_get_editingWhichParams() == 0) {
                    /* Controls fine tempo control */
                    synth_control_set_tempo_fine_norm(
                        synth_adc_scale_thresh(chan->cur_val));
                } else {
                    /* Otherwise freely controls event delta (no quantization) */
                    synth_control_set_eventDelta_free_curParams(
                            synth_adc_scale_thresh(chan->cur_val));
                }
                break;
            case SynthControlDeltaButtonMode_INTERMITTENCY:
                synth_control_set_intermittency_curParams(
                        synth_adc_scale_thresh(chan->cur_val));
                break;
        }
    }
}


static void synth_adc_pitch_curParams_control(adc_channel_t *chan,
                                    adc_channel_do_data_t *data)
{
    if (synth_control_get_posMode_curParams() == SynthControlPosMode_UNI) {
        synth_control_set_uni_stuff_changed();
        int n;
        for (n = 0; n < SYNTH_CONTROL_PITCH_TABLE_SIZE; n++) {
            synth_control_set_pitch_chrom_quant(
                    synth_adc_scale_thresh(chan->cur_val),
                    n,
                    synth_control_get_editingWhichParams());
        }
    } else {
        if (synth_control_get_editingWhichParams() == 0) {
            synth_control_set_pitch_chrom_quant(
                    synth_adc_scale_thresh(chan->cur_val),0,0);
        } else {
            synth_control_set_pitch_chrom_quant_curParams(
                    synth_adc_scale_thresh(chan->cur_val));
        }
    }
}


static void synth_adc_gain_curParams_control(adc_channel_t *chan,
                                   adc_channel_do_data_t *data)
{
    if (synth_control_get_editingWhichParams() == 0) {
        synth_control_set_wet(synth_adc_scale_thresh(chan->cur_val),0);
    } else {
        switch (synth_control_get_gainMode()) {
            case SynthControlGainMode_FADE:
                synth_control_set_ampLastEcho_curParams(
                        synth_adc_scale_thresh(chan->cur_val));
                break;
            case SynthControlGainMode_WET:
                synth_control_set_wet_curParams(
                        synth_adc_scale_thresh(chan->cur_val));
                break;
            default:
                break;
        }
    }
}


static void synth_adc_tempo_control(adc_channel_t *chan,
                                   adc_channel_do_data_t *data)
{
    if (synth_control_get_editingWhichParams() == 0) {
        SynthControlRecMode _recMode = synth_control_get_recMode();
        if ((_recMode == SynthControlRecMode_REC_LEN_1_BEAT_REC_SCHED)
                || (_recMode == SynthControlRecMode_REC_LEN_1_BEAT)) {
            synth_control_tempoNudge(synth_adc_scale_thresh(chan->cur_val));
        } else {
            synth_control_set_tempo_coarse_norm(
                    synth_adc_scale_thresh(chan->cur_val));
        }
    } else {
        synth_control_set_repeats(synth_adc_scale_thresh(chan->cur_val));
    }
}


#if defined(BOARD_V2)

static void __attribute__((optimize("O0"))) synth_adc_expr_control(adc_channel_t *chan,
                                                                   adc_channel_do_data_t *data)
{
    /* Just to test */
   adc_channel_datatype_t cur_val;
   cur_val = chan->cur_val;
}

#endif

static struct synth_adc_obj_t envelopeTime_curParams_sao;
static struct synth_adc_obj_t sustainTime_curParams_sao;
static struct synth_adc_obj_t offset_curParams_sao;
static struct synth_adc_obj_t pos_curParams_sao;
static struct synth_adc_obj_t eventDelta_curParams_sao;
static struct synth_adc_obj_t pitch_curParams_sao;
static struct synth_adc_obj_t gain_curParams_sao;
static struct synth_adc_obj_t tempo_sao;
static struct synth_adc_obj_t expr_sao;

/* Check if FBK switch is down. If it is, when you turn a knob it sets that as
   the parameter the expression pedal controls, otherwise it just calls the
   normal_func contained in synth_adc_channel_do_data_t. */
static void synth_adc_check_fbk_and_call(adc_channel_t *chan,
                                  adc_channel_do_data_t *data)
{
    synth_adc_channel_do_data_t *sadc = (synth_adc_channel_do_data_t*)data;
    if (synth_control_get_gainMode() == SynthControlGainMode_FBKHOLD) {
        expr_sao.channel_data.normal_func = sadc->normal_func;
        /* Inform that we indeed chose what to control with the expression pedal */
        synth_control_expr_ctl_chosen_set();
    } else {
        /* Otherwise just call the normal function */
        sadc->normal_func(chan,data);
    }
}

void
synth_adc_control_setup (void)
{
  synth_adc_obj_init (&envelopeTime_curParams_sao, SYNTH_ADC_ENV_IDX,
                      synth_adc_check_fbk_and_call,
                      synth_adc_envelopeTime_curParams_control);
  synth_adc_obj_init (&sustainTime_curParams_sao, SYNTH_ADC_SUS_IDX,
                      synth_adc_check_fbk_and_call,
                      synth_adc_sustainTime_curParams_control);
  synth_adc_obj_init (&offset_curParams_sao, SYNTH_ADC_OFFSET_IDX,
                      synth_adc_check_fbk_and_call,
                      synth_adc_offset_curParams_control);
  synth_adc_obj_init (&pos_curParams_sao, SYNTH_ADC_POS_IDX,
                      synth_adc_check_fbk_and_call,
                      synth_adc_pos_curParams_control);
  synth_adc_obj_init (&eventDelta_curParams_sao, SYNTH_ADC_EVENTDELTA_IDX,
                      synth_adc_check_fbk_and_call,
                      synth_adc_eventDelta_curParams_control);
  synth_adc_obj_init (&pitch_curParams_sao, SYNTH_ADC_PITCH_IDX,
                      synth_adc_check_fbk_and_call,
                      synth_adc_pitch_curParams_control);
  synth_adc_obj_init (&gain_curParams_sao, SYNTH_ADC_GAIN_IDX,
                      synth_adc_check_fbk_and_call,
                      synth_adc_gain_curParams_control);
  synth_adc_obj_init (&tempo_sao, SYNTH_ADC_TEMPO_IDX,
                      synth_adc_check_fbk_and_call, synth_adc_tempo_control);
#if defined(BOARD_V2)
  synth_adc_obj_init (&expr_sao, SYNTH_ADC_EXPR_IDX,
                      synth_adc_check_expr_and_call, 
                      /* This function does nothing, which is initially what the
                         expression pedal does. */
                      synth_adc_expr_control);
#endif

}

