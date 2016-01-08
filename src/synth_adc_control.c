#include "adc_channel.h" 
#include "synth_adc_control.h" 
#include "synth_control.h"

static float synth_adc_scale_thresh(float x);

#define SYNTH_ADC_SETUP(name,data_start_idx,func)\
    static void synth_adc_ ## name ## _control_setup(void)\
    {\
        static adc_channel_t channel;\
        static adc_channel_do_set_t do_set;\
        static adc_channel_do_data_t channel_data;\
        adc_channel_do_data_init(&channel_data,\
                                 adc_channel_do_style_CHANGED_INIT,\
                                 SYNTH_ADC_THRESHOLD,\
                                 0);\
        adc_channel_init(&channel,\
                         adc_data_starts[data_start_idx],\
                         NUM_CHANNELS_PER_ADC,\
                         ADC_AVG_SIZE);\
        adc_channel_do_set_init(&do_set,\
                                &channel,\
                                func,\
                                &channel_data);\
        adc_channel_do_set_add(&do_set);\
    }

#define SYNTH_ADC_CONTROL_FLOAT(name)\
   static void synth_adc_ ## name ## _control(adc_channel_t *chan,\
                                               adc_channel_do_data_t *data)\
    {\
         synth_control_set_ ## name (synth_adc_scale_thresh(chan->cur_val));\
    }


SYNTH_ADC_CONTROL_FLOAT(envelopeTime_curParams);
SYNTH_ADC_SETUP(envelopeTime_curParams,SYNTH_ADC_ENV_IDX,synth_adc_envelopeTime_curParams_control);
SYNTH_ADC_CONTROL_FLOAT(sustainTime_curParams);
SYNTH_ADC_SETUP(sustainTime_curParams,SYNTH_ADC_SUS_IDX,synth_adc_sustainTime_curParams_control);
SYNTH_ADC_CONTROL_FLOAT(offset_curParams);
SYNTH_ADC_SETUP(offset_curParams,SYNTH_ADC_OFFSET_IDX,synth_adc_offset_curParams_control);

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
        case SynthControlPosMode_UNKNOWN:
        case SynthControlPosMode_STRIDE:
            synth_control_set_positionStride_curParams(
                    synth_adc_scale_thresh(chan->cur_val));
            break;
    }
}

SYNTH_ADC_SETUP(pos_curParams,SYNTH_ADC_POS_IDX,synth_adc_pos_curParams_control);

static void synth_adc_eventDelta_curParams_control(adc_channel_t *chan,
                                         adc_channel_do_data_t *data)
{
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

SYNTH_ADC_SETUP(eventDelta_curParams,SYNTH_ADC_EVENTDELTA_IDX,
        synth_adc_eventDelta_curParams_control);

static void synth_adc_pitch_curParams_control(adc_channel_t *chan,
                                    adc_channel_do_data_t *data)
{
    synth_control_set_pitch_chrom_quant_curParams(
            synth_adc_scale_thresh(chan->cur_val));
}

SYNTH_ADC_SETUP(pitch_curParams,SYNTH_ADC_PITCH_IDX,synth_adc_pitch_curParams_control);

static void synth_adc_gain_curParams_control(adc_channel_t *chan,
                                   adc_channel_do_data_t *data)
{
    switch (synth_control_get_gainMode()) {
        case SynthControlGainMode_FADE:
            synth_control_set_ampLastEcho_curParams(
                    synth_adc_scale_thresh(chan->cur_val));
            break;
        case SynthControlGainMode_WET:
            synth_control_set_wet_curParams(
                    synth_adc_scale_thresh(chan->cur_val));
            break;
    }
}

SYNTH_ADC_SETUP(gain_curParams,SYNTH_ADC_GAIN_IDX,synth_adc_gain_curParams_control);

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

SYNTH_ADC_SETUP(tempo,SYNTH_ADC_TEMPO_IDX,synth_adc_tempo_control);

void synth_adc_control_setup(void)
{
    adc_channel_setup();
    synth_adc_envelopeTime_curParams_control_setup();
    synth_adc_sustainTime_curParams_control_setup();
    synth_adc_offset_curParams_control_setup();
    synth_adc_pos_curParams_control_setup();
    synth_adc_eventDelta_curParams_control_setup();
    synth_adc_pitch_curParams_control_setup();
    synth_adc_gain_curParams_control_setup();
    synth_adc_tempo_control_setup();
}

