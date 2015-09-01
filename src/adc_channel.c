#include "adc_channel.h" 

static adc_channel_do_set_t *_do_sets;

void adc_channel_do_set_init(adc_channel_do_set_t *set,
                             adc_channel_t *chan,
                             adc_channel_do_func func,
                             adc_channel_do_data_t *do_data)
{
    set->chan = chan;
    set->func = func;
    set->data = do_data;
    set->next = NULL;
}

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
        result += chan->raw_vals[n*chan->raw_val_stride];
    }
    chan->cur_val = (uint16_t)(result / chan->n_raw_vals);
}

/* Inititalizes adc_channel struct. By default given the averaging update
 * function. */
void adc_channel_init(adc_channel_t *chan,
                      uint16_t *raw_vals,
                      uint32_t raw_val_stride,
                      uint32_t nraw_vals)
{
    chan->cur_val = 0;
    chan->prev_val = 0;
    chan->raw_vals = raw_vals;
    chan->raw_val_stride = raw_val_stride;
    chan->n_raw_vals = nraw_vals;
    chan->update = adc_channel_update_avg;
}

void adc_channel_setup(void)
{
    _do_sets = NULL;
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
