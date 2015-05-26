#include <stddef.h>
#include <string.h>
#include <math.h> 
#include "audio_setup.h" 
#include "wavetables.h"
#include <stdlib.h> 
#include <stdio.h> 
#include <error.h> 
#include "mm_windows.h" 

MMWavTab WaveTable;
MMWavTab soundSample;
static MMSample waveTableData[WAVTABLE_LENGTH_SAMPLES];

MMWavTab    sampleTable[NUM_SAMPLE_TABLES];
size_t      soundSampleMaxLength;
MMWavTab   *theSound;
MMWavTab   *recordingSound;
MMSample     *hannWindowTable;
size_t       hannWindowTableLength;

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
    theSound = &WaveTable;
}

/* Read a sound from path. Sound must be mono and have the sample sample data
 * type as MMSample */
void SoundSample_init(char *path)
{
    ((MMArray*)&soundSample)->length = audio_hw_get_sample_rate(NULL)
                                        * SOUND_SAMPLE_LENGTH_SECONDS;
    ((MMArray*)&soundSample)->data = (float*)malloc(sizeof(MMSample)
                                    *((MMArray*)&soundSample)->length);
    soundSample.samplerate = audio_hw_get_sample_rate(NULL);
    FILE *f = fopen(path,"r");
    if (!f) {
        perror("Cannot open file.");
        exit(-1);
    }
    size_t rem;
    if ((rem = fread(((MMArray*)&soundSample)->data,sizeof(MMSample),
                ((MMArray*)&soundSample)->length,f))
                    < ((MMArray*)&soundSample)->length) {
        if (rem < 0) {
            perror("Reading file.");
            exit(-1);
        }
        /* Didn't fill the memory, fill with 0s. */
        memset(&((MMSample*)((MMArray*)&soundSample)->data)[rem],0,
                sizeof(MMSample)*(((MMArray*)&soundSample)->length - rem));
    }
    fclose(f);
    theSound = &soundSample;
}

void SampleTable_init(void)
{
    int n;
    for (n = 0; n < NUM_SAMPLE_TABLES; n++) {
        sampleTable[n].samplerate = audio_hw_get_sample_rate(NULL);
        ((MMArray*)&sampleTable[n])->length = SAMPLE_TABLE_LENGTH_SEC 
            * sampleTable[n].samplerate;
        ((MMArray*)&sampleTable[n])->data = 
            (MMSample*)malloc(((MMArray*)&sampleTable[n])->length*sizeof(MMSample));
        memset(((MMArray*)&sampleTable[n])->data,0,
                sizeof(MMSample) * ((MMArray*)&sampleTable[n])->length);
    }
    theSound = &sampleTable[0];
    recordingSound = &sampleTable[1];
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
