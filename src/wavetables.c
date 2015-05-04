#include <stddef.h>
#include <string.h>
#include <math.h> 
#include "audio_setup.h" 
#include "wavetables.h"

#include <stdlib.h> 
#include <stdio.h> 
#include <error.h> 

MMWavTab WaveTable;
static MMSample waveTableData[WAVTABLE_LENGTH_SAMPLES];

MMWavTab soundSample;
MMWavTab   *theSound;

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
        memset(((MMArray*)&soundSample)->data,0,
                sizeof(MMSample)*(((MMArray*)&soundSample)->length - rem));
    }
    fclose(f);
    theSound = &soundSample;
}
