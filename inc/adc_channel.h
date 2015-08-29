#ifndef ADC_CHANNEL_H
#define ADC_CHANNEL_H 
#include <stdint.h> 
#include <stdlib.h> 

#define NUM_ADC_CHANNELS 8 
/* DMA can be set up to convert values from many ADCs, scanning the inputs like
 * so
 * ADC_CH_1, ADC_CH_2, ..., ADC_CH_N.
 * To smooth the values you can take an average. To get an average, you will
 * need the M last conversions of channel n. DMA in circular mode will continue
 * cycling through the array, continuing back at the beginning when the end is
 * reached. To make it easy to take an average, a stride is defined meaning the
 * number of conversions (including the current conversion) to skip over before
 * getting a sample of the same channel. This is that value N above,
 * corresponding to the number of channels converted.
 */
#define ADC_CHANNEL_RAW_VALUE_STRIDE NUM_ADC_CHANNELS

typedef struct __adc_channel adc_channel_t;

typedef enum {
    /* Always calls the function on the adc value */
    adc_channel_do_style_ALWAYS,
    /* Calls the function on the adc value if the current value is different
     * enough from the last value (according to the threshold parameter in the
     * data). */
    adc_channel_do_style_CHANGED
} adc_channel_do_style_t;

/* Implementations can subclass this to pass more data to the adc_channel_do*
 * functions. */
typedef struct __adc_channel_do_data adc_channel_do_data_t {
    adc_channel_do_style_t style;
    /* The difference between the current value and the last value that can be
     * used to optionally do something if surpassed. */
    uint32_t threshold;
}

typedef void (*adc_channel_do_func)(adc_channel_t *,adc_channel_do_data_t *);

typedef struct __adc_channel_do_set {
    adc_channel_t *chan;
    adc_channel_do_func func;
    adc_channel_do_data_t *data;
    struct __adc_channel_do_set_t *next;
} adc_channel_do_set_t;

void adc_channel_do_set_add(adc_channel_do_set_t *set);

#define adc_channel_do(chan,data,what) (what)(chan,data)

#define adc_channel_do_if_changed(chan,data,what)\
    abs(chan->cur_val - chan->prev_val) > data->threshold ?\
    adc_channel_do(chan,data,what)\
    : NULL

#endif /* ADC_CHANNEL_H */
