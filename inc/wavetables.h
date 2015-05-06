#ifndef WAVETABLES_H
#define WAVETABLES_H 
#include "mm_wavtab.h" 

#define WAVTABLE_LENGTH_SAMPLES (44100*40)

#define WAVTABLE_NUM_PARTIALS  1

/* number of periods in the wavetable of the fundamental */
#define WAVTABLE_NUM_PERIODS (400*40)

extern MMWavTab WaveTable;

void WaveTable_init(void);

#endif /* WAVETABLES_H */
