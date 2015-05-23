#include "signal_chain.h" 
#include "wavetables.h" 
#include "audio_setup.h" 

MMBus *inBus, *outBus, *fbBus;
MMSigChain sigChain;
MMTrapEnvedSamplePlayer spsps[NUM_NOTES];
MMWavTabRecorder wtr;
MMBusSplitter fbBusSplitter;
/* Insert the fbBusSplitter after this node to turn it off */
MMSigProc *fbOffNode; 
/* Insert the fbBusSplitter after this node to turn it on */
MMSigProc *fbOnNode;

static MMBusMerger fbBusMerger;
static MMSigConst  sigConstTop;

void signal_chain_setup(void)
{
    /* Allocate space for the busses */
    /* The bus the signal chain is reading */
    inBus = MMBus_new(audio_hw_get_block_size(NULL), 
            audio_hw_get_num_input_channels(NULL));
    /* The bus the signal chain is writing */
    outBus = MMBus_new(audio_hw_get_block_size(NULL), 
            audio_hw_get_num_output_channels(NULL));
    /* A bus to feed the output back to the input so that it can be recorded */
    fbBus = MMBus_new(audio_hw_get_block_size(NULL), 
            audio_hw_get_num_output_channels(NULL));
    /* Initializes the signal chain that signal processors are put in to */
    MMSigChain_init(&sigChain);
    /* A constant that zeros the bus each iteration */
    MMSigConst_init(&sigConstTop,outBus,0,MMSigConst_doSum_FALSE);
    /* put sig constant at the top of the sig chain */
    MMSigProc_insertAfter(&sigChain.sigProcs,&sigConstTop);
    fbOffNode = (MMSigProc*)&sigConstTop;
    int i;
    for (i = 0; i < NUM_NOTES; i++) {
        /* Initialize sample player */
        MMTrapEnvedSamplePlayer_init(&spsps[i], outBus, 
                audio_hw_get_block_size(NULL), 
                1. / (MMSample)audio_hw_get_sample_rate(NULL));
        /* insert in signal chain after sig const*/
        MMSigProc_insertAfter(&sigConstTop, &spsps[i]);
    }
    /* The first spsps is the last one in the chain and so this is where you
     * want to write to the feedback bus */
    fbOnNode = (MMSigProc*)&spsps[0];
    /* Send the contents of the outBus to the fbBus. When feedback is on, the
     * bus splitter  will go after the last sampleplayer. When feedback is off,
     * it goes after the sig const and therefore only receives 0s */
    MMBusSplitter_init(&fbBusSplitter, outBus, fbBus);
    /* Initially, 0s are written to the fbBus (it is off) */
    MMSigProc_insertAfter(fbOffNode, &fbBusSplitter);
    /* Make a recorder */
    MMWavTabRecorder_init(&wtr);
    wtr.buffer = recordingSound;
    wtr.inputBus = inBus;
    wtr.currentIndex = 0;
    wtr.state = MMWavTabRecorderState_STOPPED;
    /* Insert at the top of the signal chain */
    MMSigProc_insertAfter(&sigChain.sigProcs,&wtr);
    /* Merge the contents of the fbBus with the inBus. When feedback is off,
     * this leaves the inBus unaffected by adding only 0s to it */
    MMBusMerger_init(&fbBusMerger, fbBus, inBus);
    MMSigProc_insertBefore(&wtr, &fbBusMerger);
}
