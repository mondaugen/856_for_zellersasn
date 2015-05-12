#ifndef SIGNAL_CHAIN_H
#define SIGNAL_CHAIN_H 
#include "mm_bus.h"
#include "mm_trapenvedsampleplayer.h" 
#include "mm_sigchain.h" 

#define NUM_NOTES 8 

extern MMBus *inBus, *outBus;
extern MMSigChain sigChain;
extern MMTrapEnvedSamplePlayer spsps[NUM_NOTES];

void signal_chain_setup(void);

#endif /* SIGNAL_CHAIN_H */
