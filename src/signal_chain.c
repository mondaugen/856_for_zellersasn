/* Copyright (c) 2016 Nicholas Esterer. All rights reserved. */

#include <string.h> 
#include "signal_chain.h" 
#include "wavetables.h" 
#include "audio_setup.h" 
#include "mm_busproc.h"
#include "limiter_ir_af.h"
#include "_gend_fwir_header.h"
#include "dc_notch_filter.h"
#include "signal_gate.h"
#include "mm_busmult.h"
#include "mm_sigconst.h"

MMBus *inBus, *outBus, *fbBus, *fbScaleBus, *n1fbBus;
MMSigChain sigChain;
MMTrapEnvedSamplePlayer spsps[NUM_NOTES];
/* We wrap every spsps in one of these so that we can output to multiple busses */
MMEnvedSamplePlayerTwoBus spsps_2bus_wrappers[NUM_NOTES];
MMWavTabRecorder wtr;
MMBusSplitter fbBusSplitter;
/* Insert the fbBusSplitter after this node to turn it on */
MMSigProc *fbOnNode;
MMBusMult fbBusMult, n1fbBusMult;
MMSigConst fbScaleBusConst, n1fbBusConst;
static MMBusMerger fbBusMerger, n1fbBusMerger;
/* The amount feedback is faded out */
static const float fbScaleBusConst_val = .99;

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

struct dc_notch_filter_1_pole *dc_blocker = NULL;
struct dc_notch_filter_1_pole_init dc_blocker_init;

static void dc_blocker_fun(MMBus *bus, void *aux_)
{
    struct dc_notch_filter_1_pole *aux = aux_;
    if ((aux != NULL) && (bus->channels == 1)) {
        /* we can block dc */
        dc_notch_filter_1_pole_tick(aux,bus->data);
    } 
}

static void dc_blocker_setup(void)
{
    dc_blocker_init = (struct dc_notch_filter_1_pole_init) {
        .r = .999, // gives pretty low 3dB point on highpass
        .buffer_size = BUFFER_SIZE
    };
    dc_blocker = dc_notch_filter_1_pole_new(&dc_blocker_init);
}

struct signal_gate *fbk_signal_gate = NULL;
struct signal_gate *n1_fbk_signal_gate = NULL;
struct signal_gate_init fbk_signal_gate_init;

static void fbk_signal_gate_fun(MMBus *bus, void *aux_)
{
    struct signal_gate *aux = aux_;
    /* if aux is NULL or bus->channels != 1 the whole system is not working properly */
    if ((aux != NULL) && (bus->channels == 1)) {
        signal_gate_tick(aux,bus->data);
    }
}

static void fbk_signal_gate_setup(void)
{
    fbk_signal_gate_init = (struct signal_gate_init) {
        .buffer_size = BUFFER_SIZE,
        /* Gives a nice little ramp */
        .ramp_time = 1000
    };
    fbk_signal_gate = signal_gate_new(&fbk_signal_gate_init);
    n1_fbk_signal_gate = signal_gate_new(&fbk_signal_gate_init);
}

void fbk_signal_gate_pass(void)
{
    signal_gate_set_state(fbk_signal_gate,1);
}

void fbk_signal_gate_block(void)
{
    signal_gate_set_state(fbk_signal_gate,0);
}

void n1_fbk_signal_gate_pass(void)
{
    signal_gate_set_state(n1_fbk_signal_gate,1);
}

void n1_fbk_signal_gate_block(void)
{
    signal_gate_set_state(n1_fbk_signal_gate,0);
}

__attribute__((optimize("-O0")))
void signal_chain_setup(void)
{
    /* Allocate space for the busses */
    /* The bus the signal chain is reading */
    inBus = MMBus_new(audio_hw_get_block_size(NULL),1);
    /* The bus the signal chain is writing */
    outBus = MMBus_new(audio_hw_get_block_size(NULL),1);
    /* A bus to feed the output back to the input so that it can be recorded */
    fbBus = MMBus_new(audio_hw_get_block_size(NULL),1);
    /* A bus to hold a scalar for the feeback */
    fbScaleBus = MMBus_new(audio_hw_get_block_size(NULL),1);
    /* A bus to feed back only the output of N1 */
    n1fbBus = MMBus_new(audio_hw_get_block_size(NULL),1);
    /* Set the bus to contain all 0s initially */
    memset(fbBus->data,0,sizeof(MMSample)*fbBus->size*fbBus->channels);
    /* Initializes the signal chain that signal processors are put in to */
    MMSigChain_init(&sigChain);
    /* Initialize audio limiter */
    audio_limiter_setup();
    MMBusProc *audio_limiter_bus_proc = MMBusProc_new(outBus,audio_limiter_fun,audio_limiter);
    /* Initialize DC blocker */
    dc_blocker_setup();
    MMBusProc *dc_blocker_bus_proc = MMBusProc_new(outBus,dc_blocker_fun,dc_blocker);
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
        MMEnvedSamplePlayerTwoBusInitStruct esp2bi = {
            .esp = (MMEnvedSamplePlayer*)&spsps[i]
        };
        MMEnvedSamplePlayerTwoBus_init(&spsps_2bus_wrappers[i],&esp2bi);
        /* insert in signal chain after sig const*/
        MMSigProc_insertAfter(&sigChain.sigProcs, &spsps[i]);
    }
    /* Initialize sample player */
    ((MMEnvedSamplePlayerInitStruct*)&tespinit)->tickType
        = MMEnvedSamplePlayerTickType_NOSUM;
    MMTrapEnvedSamplePlayer_init(&spsps[i],&tespinit);
    MMEnvedSamplePlayerTwoBusInitStruct esp2bi = {
        .esp = (MMEnvedSamplePlayer*)&spsps[i]
    };
    MMEnvedSamplePlayerTwoBus_init(&spsps_2bus_wrappers[i],&esp2bi);
    /* insert in signal chain at the beginning */
    MMSigProc_insertAfter(&sigChain.sigProcs, &spsps[i]);
    /*
    The first thing to do is zero the n1fbBus so we put n1fbBusConst at the
    beginning
    */
    MMSigConst_init(&n1fbBusConst,n1fbBus,0,MMSigConst_doSum_FALSE);
    MMSigProc_insertAfter(&sigChain.sigProcs,&n1fbBusConst);
    /*
    The first spsps is the last one in the chain and so this is where you
    want to put the dc blocker and limiter
    first we block DC.
    */
    MMSigProc_insertAfter((MMSigProc*)&spsps[0],dc_blocker_bus_proc);
    /* Then we limit */
    MMSigProc_insertAfter((MMSigProc*)dc_blocker_bus_proc,audio_limiter_bus_proc);
    /* We write to the feedback bus after limiting, so this is where it is
    placed after when feedback is on. */
    fbOnNode = (MMSigProc*)audio_limiter_bus_proc;
    /* Send the contents of the outBus to the fbBus. */
    MMBusSplitter_init(&fbBusSplitter, outBus, fbBus);
    /* Put the splitter after the last in the playback chain (the limiter) */
    MMSigProc_insertAfter(fbOnNode,&fbBusSplitter);
    /* Scale the contents of the feedback bus so they slowly fade out if feedback is left on forever */
    MMBusMult_init(&fbBusMult,fbBus,fbScaleBus);
    /* Scale the contents of the N1 feedback bus so they slowly fade out if feedback is left on forever */
    MMBusMult_init(&n1fbBusMult,n1fbBus,fbScaleBus);
    /* Initialize scalar */
    MMSigConst_init(&fbScaleBusConst,fbScaleBus,fbScaleBusConst_val,MMSigConst_doSum_FALSE);
    /* Put a signal gate that optionally zeros the feedback bus */
    fbk_signal_gate_setup();
    MMBusProc *fbk_signal_gate_bus_proc = MMBusProc_new(fbBus,fbk_signal_gate_fun,fbk_signal_gate);
    MMSigProc_insertAfter(&fbBusSplitter,fbk_signal_gate_bus_proc);
    MMBusProc *n1_fbk_signal_gate_bus_proc = MMBusProc_new(
        n1fbBus,fbk_signal_gate_fun,n1_fbk_signal_gate);
    /* Put BusMult after gate  */
    MMSigProc_insertAfter(fbk_signal_gate_bus_proc,&fbScaleBusConst);
    MMSigProc_insertAfter(&fbScaleBusConst,&fbBusMult);
    MMSigProc *fbBusEnd = (MMSigProc*)&fbBusMult;
    /* Merge the contents of the fbBus with the inBus. When feedback is off,
     * this leaves the inBus unaffected by adding only 0s to it */
    MMBusMerger_init(&fbBusMerger, fbBus, inBus);
    MMBusMerger_init(&n1fbBusMerger, n1fbBus, inBus);
    MMSigProc_insertAfter(fbBusEnd, &fbBusMerger);
    MMSigProc_insertAfter(&fbBusMerger,&n1fbBusMerger);
    MMSigProc_insertBefore(&fbBusMerger,n1_fbk_signal_gate_bus_proc);
    MMSigProc_insertBefore(n1_fbk_signal_gate_bus_proc,&n1fbBusMult);
    /* Make a recorder */
    MMWavTabRecorder_init(&wtr);
    wtr.buffer = recordingSound->wavtab;
    wtr.inputBus = inBus;
    wtr.currentIndex = 0;
    wtr.state = MMWavTabRecorderState_STOPPED;
    /* Insert at the end */
    MMSigProc *sig_chain_end = (MMSigProc *)MMDLList_getTail((MMDLList*)&sigChain.sigProcs);
    MMSigProc_insertAfter(sig_chain_end,&wtr);
#ifdef SIG_CHAIN_FILL_BUF_ONES
    MMSigConst_init(&fillOnesSigConst,inBus,1,MMSigConst_doSum_FALSE);
    MMSigProc_insertBefore(&wtr,&fillOnesSigConst);
#endif  
}

MMBus *
signal_chain_get_n1fbBus(void)
{
    return n1fbBus;
}
