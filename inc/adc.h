#ifndef ADC_H
#define ADC_H 
#include <stdint.h> 
#define ADC_MAX 4095
/* With an audio sampling rate of 32000 Hz, and a block size of 128 samples, you
 * can get ~16 ADC samples in one block tick on one ADC channel of a group
 * of 8 channels running at sampling rate of 5680 Hz */
#define ADC_AVG_SIZE 16
/* There are 4 ADC channels per ADC */
#define NUM_CHANNELS_PER_ADC 4 
#define TOTAL_NUM_ADC_CHANNELS 8
#define NUM_ADC_VALUES (ADC_AVG_SIZE*TOTAL_NUM_ADC_CHANNELS)
#define ADC1_DMA_NUM_VALS_TRANS (NUM_CHANNELS_PER_ADC*ADC_AVG_SIZE)
#define ADC3_DMA_NUM_VALS_TRANS (NUM_CHANNELS_PER_ADC*ADC_AVG_SIZE) 
extern uint16_t volatile *adc_data_starts[];
void adc_setup_dma_scan(void);
#endif /* ADC_H */
