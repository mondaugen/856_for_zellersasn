#include "pots.h" 


typedef struct __adc_channel {
    uint16_t cur_val;
    uint16_t prev_val;
    uint16_t[ADC_CHANNEL_RAW_VALUE_STRIDE] *raw_vals;
    uint32_t n_raw_vals;
    /* The function that is used to calculate cur_val from raw_vals */
    void update (struct __adc_channel *); 
} adc_channel_t;

static adc_channel_do_set_t *_do_sets;

void adc_channel_do_set_add(adc_channel_do_set_t *set)
{
    set->next = _do_sets;
    _do_sets = set;
}

void adc_channels_update(adc_channel_t *chans, uint32_t nchans)
{
    while (nchans--) {
        chans->prev_val = chans->cur_val;
        chans->update(chans);
        chans++;
    }
}

void adc_channel_do_all_sets(void)
{
    adc_channel_do_set_t *set = _do_sets;
    while (set) {
        switch (set->data->style) {
            case adc_channel_do_style_ALWAYS:
                adc_channel_do(set->chan,set->data,set->func);
                break;
            case adc_channel_do_style_CHANGED:
                adc_channel_do_if_changed(set->chan,set->data,set->func);
                break;
        }
        set = set->next;
    }
}

void adc_channel_update_avg(adc_channel_t *chan)
{
    uint32_t n, result = 0;
    for (n = 0; n < chan->n_raw_vals; n++) {
        result += chan->raw_vals[n];
    }
    chan->cur_val = (uint16_t)(result / chan->n_raw_vals);
}

/* Inititalizes adc_channel struct. By default given the averaging update
 * function. */
void adc_channel_init(adc_channel_t *chan,
                      uint16_t[ADC_CHANNEL_RAW_VALUE_STRIDE] *raw_vals,
                      uint32_t nraw_vals)
{
    chan->cur_val = 0;
    chan->prev_val = 0;
    chan->raw_vals = raw_vals;
    chan->n_raw_vals = n_raw_vals;
    chan->update = adc_channel_update_avg;
}

/* TODO: 
 *
 * Implement an array or list of adc_channels for this application. If the
 * array or list is allocated by the user, you will need to provide the struct's
 * fields (not forward-declare the struct).
 *
 * Make some sets with function that are called on the values read from the
 * ADCs.
 *
 */
