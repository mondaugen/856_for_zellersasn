#ifndef ADC_H
#define ADC_H 
#include <stdint.h> 
#define NUM_ADC_VALUES 8 
#define ADC1_DMA_NUM_VALS_TRANS 4
#define ADC3_DMA_NUM_VALS_TRANS 4 
extern volatile uint16_t adc_values[];
void adc_setup_dma_scan(void);
#endif /* ADC_H */
