#include "poly_management.h"
#include "signal_chain.h" 

MMPolyManager *pvm;

void poly_management_setup(void)
{
    /* Make poly voice manager */
    pvm = MMPolyManager_new(NUM_NOTES);
    int n;
    for (n = 0; n < NUM_NOTES; n++) {
        /* Make new poly voice and add it to the poly voice manager */
        MMPolyManager_addVoice(pvm, n, (MMPolyVoice*)MMPvtesp_new(&spsps[n])); 
    }
}

