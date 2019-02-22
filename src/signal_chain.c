/* Copyright (c) 2016 Nicholas Esterer. All rights reserved. */

#include <string.h> 
#include "signal_chain.h" 
#include "wavetables.h" 
#include "audio_setup.h" 
#include "mm_busproc.h"
#include "limiter_ir_af.h"
#include "_gend_fwir_header.h"

MMBus *inBus, *outBus, *fbBus;
MMSigChain sigChain;
MMTrapEnvedSamplePlayer spsps[NUM_NOTES];
MMWavTabRecorder wtr;
MMBusSplitter fbBusSplitter;
/* Insert the fbBusSplitter after this node to turn it on */
MMSigProc *fbOnNode;

#ifdef SIG_CHAIN_FILL_BUF_ONES
/* Instead of recording what comes in the input, just send 1s to the recorder. */
MMSigConst fillOnesSigConst;
#endif  

static void audio_limiter_fun(MMBus *bus, void *aux_)
{
    struct limiter_ir_af *aux = aux_;
    if ((aux != NULL) && (bus->channels == 1)) {
        /* we can limit the output nicely */
        limiter_ir_af_tick(aux,bus->data);
    } 
}

struct limiter_ir_af *audio_limiter = NULL;
struct filter_w_ir *audio_limiter_fwir = NULL;
struct limiter_ir_af_init liai;

static void audio_limiter_setup(void)
{
    audio_limiter_fwir = filter_w_ir_new(&gen_fwir_header_filter_w_ir_init);

    liai = (struct limiter_ir_af_init) {
        .fwir = audio_limiter_fwir,
        .buffer_size = BUFFER_SIZE,
        .threshold = .9
    };

    audio_limiter = limiter_ir_af_new(&liai);
}

static MMBusMerger fbBusMerger;

void signal_chain_setup(void)
{
    /* Allocate space for the busses */
    /* The bus the signal chain is reading */
    inBus = MMBus_new(audio_hw_get_block_size(NULL),1);
    /* The bus the signal chain is writing */
    outBus = MMBus_new(audio_hw_get_block_size(NULL),1);
    /* A bus to feed the output back to the input so that it can be recorded */
    fbBus = MMBus_new(audio_hw_get_block_size(NULL),1);
    /* Set the bus to contain all 0s initially */
    memset(fbBus->data,0,sizeof(MMSample)*fbBus->size*fbBus->channels);
    /* Initializes the signal chain that signal processors are put in to */
    MMSigChain_init(&sigChain);
    /* Initialize audio limiter */
    audio_limiter_setup();
    MMBusProc *audio_limiter_bus_proc = MMBusProc_new(outBus,audio_limiter_fun,audio_limiter);
    int i;
    /* The last (NUM_NOTES-1) sample players sum into the bus, the first one
     * coming right at the top of the signal chain writes straight into the bus
     * */
    MMTrapEnvedSamplePlayerInitStruct tespinit;
    ((MMEnvedSamplePlayerInitStruct*)&tespinit)->outBus
        = outBus;
    ((MMEnvedSamplePlayerInitStruct*)&tespinit)->interp
        = MMInterpMethod_CUBIC;
    ((MMEnvedSamplePlayerInitStruct*)&tespinit)->tickType
        = MMEnvedSamplePlayerTickType_SUM;
    ((MMEnvedSamplePlayerInitStruct*)&tespinit)->internalBusSize
        = audio_hw_get_block_size(NULL); 
    tespinit.tickPeriod = 1. / (MMSample)audio_hw_get_sample_rate(NULL);
    for (i = 0; i < (NUM_NOTES-1); i++) {
        MMTrapEnvedSamplePlayer_init(&spsps[i],&tespinit);
        /* insert in signal chain after sig const*/
        MMSigProc_insertAfter(&sigChain.sigProcs, &spsps[i]);
    }
    /* Initialize sample player */
    ((MMEnvedSamplePlayerInitStruct*)&tespinit)->tickType
        = MMEnvedSamplePlayerTickType_NOSUM;
    MMTrapEnvedSamplePlayer_init(&spsps[i],&tespinit);
    /* insert in signal chain after sig const*/
    MMSigProc_insertAfter(&sigChain.sigProcs, &spsps[i]);
    /* The first spsps is the last one in the chain and so this is where you
     * want to put the limiter*/
    MMSigProc_insertAfter((MMSigProc*)&spsps[0],audio_limiter_bus_proc);
    /* We write to the feedback bus after limiting, so this is where it is
    placed after when feedback is on. */
    fbOnNode = (MMSigProc*)audio_limiter_bus_proc;
    /* Send the contents of the outBus to the fbBus. When feedback is on, the
     * bus splitter  will go after the last sampleplayer. When feedback is off,
     * it goes after the sig const and therefore only receives 0s */
    MMBusSplitter_init(&fbBusSplitter, outBus, fbBus);
    /* Initially, 0s are written to the fbBus (it is not inserted into the
     * signal chain) */
    /* Make a recorder */
    MMWavTabRecorder_init(&wtr);
    wtr.buffer = recordingSound->wavtab;
    wtr.inputBus = inBus;
    wtr.currentIndex = 0;
    wtr.state = MMWavTabRecorderState_STOPPED;
    /* Insert at the top of the signal chain */
    MMSigProc_insertAfter(&sigChain.sigProcs,&wtr);
    /* Merge the contents of the fbBus with the inBus. When feedback is off,
     * this leaves the inBus unaffected by adding only 0s to it */
    MMBusMerger_init(&fbBusMerger, fbBus, inBus);
    MMSigProc_insertBefore(&wtr, &fbBusMerger);
#ifdef SIG_CHAIN_FILL_BUF_ONES
    MMSigConst_init(&fillOnesSigConst,inBus,1,MMSigConst_doSum_FALSE);
    MMSigProc_insertBefore(&wtr,&fillOnesSigConst);
#endif  
}
