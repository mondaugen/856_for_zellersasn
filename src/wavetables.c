#include <stddef.h>
#include <string.h>
#include <math.h> 
#include "audio_setup.h" 
#include "wavetables.h"
#include <stdlib.h> 

MMWavTab WaveTable;
static MMSample waveTableData[WAVTABLE_LENGTH_SAMPLES];
MMWavTab sampleTable;

void WaveTable_init(void)
{
    ((MMArray*)&WaveTable)->data = waveTableData;
    ((MMArray*)&WaveTable)->length = WAVTABLE_LENGTH_SAMPLES;
    WaveTable.samplerate = audio_hw_get_sample_rate(NULL);
    size_t i,j;
    memset(waveTableData,0,sizeof(MMSample) * WAVTABLE_LENGTH_SAMPLES);
    for (i = 0; i < WAVTABLE_LENGTH_SAMPLES; i++) {
        for (j = 0; j < WAVTABLE_NUM_PARTIALS; j++) {
            waveTableData[i] += sin((MMSample)i / (MMSample)WAVTABLE_LENGTH_SAMPLES 
                    * (j + 1) * M_PI * 2.) / (MMSample)(j + 1);
        }
    }
}

void SampleTable_init(void)
{
    sampleTable.samplerate = audio_hw_get_sample_rate(NULL);
    ((MMArray*)&sampleTable)->length = 22050; // SAMPLE_TABLE_LENGTH_SEC 
//                                        * sampleTable.samplerate;
    ((MMArray*)&sampleTable)->data = 
        (MMSample*)malloc(((MMArray*)&sampleTable)->length*sizeof(MMSample));
    memset(((MMArray*)&sampleTable)->data,0,
            sizeof(MMSample) * ((MMArray*)&sampleTable)->length);

}
