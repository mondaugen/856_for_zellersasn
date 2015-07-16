#include <stddef.h>
#include <string.h>
#include <math.h> 
#include "audio_setup.h" 
#include "wavetables.h"
#include <stdlib.h> 
#include "mm_windows.h" 
#include "fmc.h"
#include "err.h" 

MMWavTab WaveTable;
MMWavTab soundSample;

/* Areas in memory where samples are recorded */
MMSample *sampleTableAreas[NUM_SAMPLE_TABLES];
MMWavTab    sampleTable[NUM_SAMPLE_TABLES];
size_t      soundSampleMaxLength;
WavTabAreaPair theSound;
WavTabAreaPair recordingSound;
MMSample     *hannWindowTable;
size_t       hannWindowTableLength;
size_t       zeroxSearchMaxLength;

void SampleTable_init(void)
{
    int n;
    for (n = 0; n < NUM_SAMPLE_TABLES; n++) {
        sampleTable[n].samplerate = audio_hw_get_sample_rate(NULL);
        ((MMArray*)&sampleTable[n])->length = SAMPLE_TABLE_LENGTH_SEC 
            * sampleTable[n].samplerate;
        sampleTableAreas[n] = ((MMSample*)SDRAM_BANK_ADDR)
            + ((MMArray*)&sampleTable[n])->length*n;
        if (((uint32_t)(sampleTableAreas[n]
                + ((MMArray*)&sampleTable[n])->length))
            > (SDRAM_BANK_ADDR + SDRAM_LENGTH)) {
            THROW_ERR("Sample tables do not fit in SDRAM.");
        }
        ((MMArray*)&sampleTable[n])->data = sampleTableAreas[n];
        memset(((MMArray*)&sampleTable[n])->data,0,
                sizeof(MMSample) * ((MMArray*)&sampleTable[n])->length);
    }
    theSound.wavtab = &sampleTable[0];
    theSound.area = sampleTableAreas[0];
    recordingSound.wavtab = &sampleTable[1];
    recordingSound.area   = sampleTableAreas[1];
    soundSampleMaxLength = SAMPLE_TABLE_LENGTH_SEC 
        * audio_hw_get_sample_rate(NULL);
}

void HannWindowTable_init(MMSample len_sec)
{
    size_t N = (MMSample)audio_hw_get_sample_rate(NULL) * len_sec;
    hannWindowTable = (MMSample*)malloc(sizeof(MMSample) * N);
    MM_hann_fill(hannWindowTable,N);
    hannWindowTableLength = N;
}

void ZeroxSearch_init(MMSample len_sec)
{
    zeroxSearchMaxLength = (MMSample)audio_hw_get_sample_rate(NULL) * len_sec;
}
