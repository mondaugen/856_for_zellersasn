#include "signal_chain.h" 
#include "wavetables.h" 

MMBus *inBus, *outBus;
MMSigChain sigChain;
MMTrapEnvedSamplePlayer spsps[NUM_NOTES];
MMWavTabRecorder wtr;

static MMSigConst sigConst;

void signal_chain_setup(void)
{
    /* Allocate space for the busses */
    /* The bus the signal chain is reading */
    inBus = MMBus_new(audio_hw_get_block_size(NULL), 
            audio_hw_get_num_input_channels(NULL));
    /* The bus the signal chain is writing */
    outBus = MMBus_new(audio_hw_get_block_size(NULL), 
            audio_hw_get_num_output_channels(NULL));
    /* Initializes the signal chain that signal processors are put in to */
    MMSigChain_init(&sigChain);
    /* A constant that zeros the bus each iteration */
    MMSigConst_init(&sigConst,outBus,0,MMSigConst_doSum_FALSE);
    /* put sig constant at the top of the sig chain */
    MMSigProc_insertAfter(&sigChain.sigProcs,&sigConst);
    int i;
    for (i = 0; i < NUM_NOTES; i++) {
        /* Initialize sample player */
        MMTrapEnvedSamplePlayer_init(&spsps[i], outBus, 
                audio_hw_get_block_size(NULL), 
                1. / (MMSample)audio_hw_get_sample_rate(NULL));
        /* insert in signal chain after sig const*/
        MMSigProc_insertAfter(&sigConst, &spsps[i]);
    }
    /* Make a recorder */
    MMWavTabRecorder_init(&wtr);
    wtr.buffer = &sampleTable;
    wtr.inputBus = inBus;
    wtr.currentIndex = 0;
    wtr.state = MMWavTabRecorderState_STOPPED;
    /* Insert at the top of the signal chain */
    MMSigProc_insertAfter(&sigChain.sigProcs,&wtr);
}






