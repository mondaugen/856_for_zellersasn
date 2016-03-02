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
WavTabAreaPair *theSound;
WavTabAreaPair *recordingSound;
MMSample     *hannWindowTable;
size_t       hannWindowTableLength;
size_t       zeroxSearchMaxLength;
static WavTabAreaPair wtaps[NUM_SAMPLE_TABLES];
#ifdef WAVETABLES_IN_SRAM
 #define SRAM_WAVETABLE_SIZE 32000/4
    MMSample sramSampleTableData[NUM_SAMPLE_TABLES*SRAM_WAVETABLE_SIZE]
        __attribute__((section(".big_data")));
#endif /* WAVETABLES_IN_SRAM */


void SampleTable_init(void)
{
    int n;
    for (n = 0; n < NUM_SAMPLE_TABLES; n++) {
        sampleTable[n].samplerate = audio_hw_get_sample_rate(NULL);
        sampleTable[n].n_players  = 0;
#ifdef WAVETABLES_IN_SRAM
        ((MMArray*)&sampleTable[n])->length = SRAM_WAVETABLE_SIZE;
        sampleTableAreas[n] = ((MMSample*)sramSampleTableData)
            + ((MMArray*)&sampleTable[n])->length*n;
#else
        ((MMArray*)&sampleTable[n])->length = SAMPLE_TABLE_LENGTH_SEC 
            * sampleTable[n].samplerate;
        sampleTableAreas[n] = ((MMSample*)SDRAM_BANK_ADDR)
            + ((MMArray*)&sampleTable[n])->length*n;
        if (((uint32_t)(sampleTableAreas[n]
                + ((MMArray*)&sampleTable[n])->length))
            > (SDRAM_BANK_ADDR + SDRAM_LENGTH)) {
            THROW_ERR("Sample tables do not fit in SDRAM.");
        }
#endif /* WAVETABLES_IN_SRAM */
        ((MMArray*)&sampleTable[n])->data = sampleTableAreas[n];
        memset(((MMArray*)&sampleTable[n])->data,0,
                sizeof(MMSample) * ((MMArray*)&sampleTable[n])->length);
        /* Make ring of WavTabAreaPairs */
        wtaps[n].wavtab = &sampleTable[n];
        wtaps[n].area   = sampleTableAreas[n];
        if (n < (NUM_SAMPLE_TABLES-1)) {
            wtaps[n].next = &wtaps[n+1];
        } else {
            wtaps[n].next = &wtaps[0];
        }
    }
    theSound = &wtaps[0];
    recordingSound = wtaps[0].next;
#ifdef WAVETABLES_IN_SRAM
    soundSampleMaxLength = SRAM_WAVETABLE_SIZE;
#else
    soundSampleMaxLength = SAMPLE_TABLE_LENGTH_SEC 
        * audio_hw_get_sample_rate(NULL);
#endif /* WAVETABLES_IN_SRAM */
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
