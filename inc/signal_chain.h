#ifndef SIGNAL_CHAIN_H
#define SIGNAL_CHAIN_H 
#include "mm_bus.h"
#include "mm_trapenvedsampleplayer.h" 
#include "mm_sigchain.h" 

#define NUM_NOTES 12 

extern MMBus *inBus, *outBus;
extern MMSigChain sigChain;
extern MMTrapEnvedSamplePlayer spsps[NUM_NOTES];

#endif /* SIGNAL_CHAIN_H */
