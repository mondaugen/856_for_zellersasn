/* Copyright (c) 2016 Nicholas Esterer. All rights reserved. */
#ifndef ADC_H
#define ADC_H 

#include <stdint.h> 

#if ((!defined(BOARD_V1)) & (!defined(BOARD_V2)))
 #error("Please define board type.")
#endif

#define ADC_MAX 4095
/* With an audio sampling rate of 32000 Hz, and a block size of 128 samples, you
 * can get ~16 ADC samples in one block tick on one ADC channel of a group
 * of 8 channels running at sampling rate of 5680 Hz */
#define ADC_AVG_SIZE 128

#if defined(BOARD_V1)
 /* There are 4 ADC channels per ADC */
 #define NUM_CHANNELS_PER_ADC 4 
 #define TOTAL_NUM_ADC_CHANNELS 8
 #define ADC1_DMA_NUM_VALS_TRANS (NUM_CHANNELS_PER_ADC*ADC_AVG_SIZE)
 #define ADC3_DMA_NUM_VALS_TRANS (NUM_CHANNELS_PER_ADC*ADC_AVG_SIZE) 
#elif defined(BOARD_V2)
 /* There are 4 ADC channels on ADC1, 5 on ADC3 */
 #define  NUM_CHANNELS_ADC1 4
 #define  NUM_CHANNELS_ADC3 5 
 #define TOTAL_NUM_ADC_CHANNELS 9
 #define ADC1_DMA_NUM_VALS_TRANS (NUM_CHANNELS_ADC1*ADC_AVG_SIZE)
 #define ADC3_DMA_NUM_VALS_TRANS (NUM_CHANNELS_ADC3*ADC_AVG_SIZE) 
#endif

#define NUM_ADC_VALUES (ADC_AVG_SIZE*TOTAL_NUM_ADC_CHANNELS)
#define ADC1_READY_BIT 0
#define ADC3_READY_BIT 1
#define ADC_NUM_ADCS 2
#define ADC_READY_MASK ((1 << ADC1_READY_BIT) | (1 << ADC3_READY_BIT)) 

typedef enum {
    /* In continuous mode, the adc is always converting. The application can
     * check the values whenever, but the conversion will always be running. */
    adc_mode_CONT,
    /* In one shot mode, the adc sets a flag when it is done converting, and
     * pauses. The application must restart conversion. This is so the
     * application can check the values knowing they will not change, and then
     * request new values when it is done with them. */
    adc_mode_1SHOT
} adc_mode_t;

extern uint16_t volatile *adc_data_starts[];
/* Array of length TOTAL_NUM_ADC_CHANNELS where the nth index contains the
 * number of channels on the ADC port which adc_data_starts refers to.
 * This is required so that averaging functions know how many values to skip
 * before reading the next value on the same channel.
 */
extern uint32_t adc_raw_value_strides[];
void adc_setup_dma_scan(adc_mode_t mode);
void adc_start_conversion(void);
int  adc_get_adc_ready(void);
void adc_clear_adc_ready(void);

#endif /* ADC_H */
