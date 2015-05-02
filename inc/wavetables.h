#ifndef WAVETABLES_H
#define WAVETABLES_H 
#include "mm_wavtab.h" 

#define WAVTABLE_LENGTH_SAMPLES 8192

#define WAVTABLE_NUM_PARTIALS   4

extern MMWavTab WaveTable;

void WaveTable_init(void);

#endif /* WAVETABLES_H */
