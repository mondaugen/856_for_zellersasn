#ifndef AUDIO_SETUP_H
#define AUDIO_SETUP_H 
#include "alsa_lowlevel.h"
#include "audio_hw.h" 

#define audio_start() audio_hw_start(NULL) 
int audio_setup(void *data);
extern int audio_ready;

#endif /* AUDIO_SETUP_H */
