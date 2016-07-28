/* Copyright (c) 2016 Nicholas Esterer. All rights reserved. */

#ifndef SYNTH_ADC_CONTROL_H
#define SYNTH_ADC_CONTROL_H 


#define SYNTH_ADC_ENV_IDX           0
#define SYNTH_ADC_PITCH_IDX         1
#define SYNTH_ADC_SUS_IDX           2
#define SYNTH_ADC_POS_IDX           3
#define SYNTH_ADC_TEMPO_IDX         4
#define SYNTH_ADC_OFFSET_IDX        5
#define SYNTH_ADC_EVENTDELTA_IDX    6
#define SYNTH_ADC_GAIN_IDX          7

#define SYNTH_ADC_LOWER_THRESH      0.
#define SYNTH_ADC_UPPER_THRESH      1.

#define SYNTH_ADC_THRESHOLD         32 

void synth_adc_control_setup(void);
#endif /* SYNTH_ADC_CONTROL_H */
