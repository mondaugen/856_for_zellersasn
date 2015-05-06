#include <stddef.h>
#include <string.h>
#include <math.h> 
#include "audio_setup.h" 
#include "wavetables.h"

MMWavTab WaveTable;
static MMSample waveTableData[WAVTABLE_LENGTH_SAMPLES];

void WaveTable_init(void)
{
    ((MMArray*)&WaveTable)->data = waveTableData;
    ((MMArray*)&WaveTable)->length = WAVTABLE_LENGTH_SAMPLES;
    WaveTable.samplerate = audio_hw_get_sample_rate(NULL);
    size_t i,j;
    memset(waveTableData,0,sizeof(MMSample) * WAVTABLE_LENGTH_SAMPLES);
    for (i = 0; i < WAVTABLE_LENGTH_SAMPLES; i++) {
        for (j = 0; j < WAVTABLE_NUM_PARTIALS; j++) {
            waveTableData[i] += sin(WAVTABLE_NUM_PERIODS
                    * (MMSample)i / (MMSample)WAVTABLE_LENGTH_SAMPLES 
                    * (j + 1) * M_PI * 2.) / (MMSample)(j + 1);
        }
    }
}

