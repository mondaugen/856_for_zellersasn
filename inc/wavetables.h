#ifndef WAVETABLES_H
#define WAVETABLES_H 
#include "mm_wavtab.h" 

#define WAVTABLE_LENGTH_SAMPLES 8192
#define SAMPLE_TABLE_LENGTH_SEC 2 

#define WAVTABLE_NUM_PARTIALS   8

extern MMWavTab WaveTable;
extern MMWavTab sampleTable;

void WaveTable_init(void);
void SampleTable_init(void);

#endif /* WAVETABLES_H */
