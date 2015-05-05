#include "midi_setup.h" 
#include "poly_management.h" 
#include "wavetables.h" 
#include "signal_chain.h" 
#include <math.h> 

static MMSample attackTime       = 0.01;
static MMSample shortReleaseTime = 0.025;
static MMSample releaseTime      = 0.01;
static MMSample sustainTime      = 1.;

#define NOTE_STEALING MMPolyManagerSteal_TRUE

void MIDI_synth_note_off_do(void *data, MIDIMsg *msg)
{
    MMPvtespParams *params = MMPvtespParams_new();
    ((MMPolyVoiceParams*)params)->steal = NOTE_STEALING;
    params->paramType = MMPvtespParamType_NOTEOFF;
    params->note = (MMSample)msg->data[1];
    params->amplitude = (MMSample)msg->data[2] / 127.;
    params->releaseTime = releaseTime;
    MMPolyManager_noteOff(pvm, (void*)params);
    MIDIMsg_free(msg);
}

/* Callback to trigger synth with note on */
void MIDI_synth_note_on_do(void *data, MIDIMsg *msg)
{
    if (msg->data[2] > 0) {
        MMPvtespParams *params = MMPvtespParams_new();
        ((MMPolyVoiceParams*)params)->steal = NOTE_STEALING;
        params->paramType = MMPvtespParamType_NOTEON;
        params->note = (MMSample)msg->data[1];
        params->amplitude = (MMSample)msg->data[2] / 127.;
        params->interpolation = MMInterpMethod_CUBIC;
        params->index = 0;
        params->attackTime = attackTime;
        /* this is the time a note that is stolen will take to decay */
        params->releaseTime = shortReleaseTime; 
        params->samples = theSound;
        params->loop = 1;
        params->rate = pow(2.,((MMSample)msg->data[1]-60)/12.);
        params->rateSource = MMPvtespRateSource_RATE;
        MMPolyManager_noteOn(pvm, (void*)params);
        MIDIMsg_free(msg);
    } else {
        MIDI_synth_note_off_do(data,msg);
    }
}

static void autorelease_on_done(MMEnvedSamplePlayer * esp)
{
    pm_yield_params_to_allocator((void*)&voiceAllocator,
            (void *)&(MMEnvedSamplePlayer_getSamplePlayerSigProc(esp).note));
}

void MIDI_note_on_autorelease_do(void *data, MIDIMsg *msg)
{
    MMSample voiceNum = pm_get_next_free_voice_number();
    if ((voiceNum == -1) || (msg->data[2] <= 0)) { 
        /* No more voices free or actually received a cheap note off (note on
         * with velocity 0). */
        MIDIMsg_free(msg);
        return;
    }
    pm_claim_params_from_allocator((void*)&voiceAllocator,(void*)&voiceNum);
    ((MMEnvedSamplePlayer*)&spsps[(int)voiceNum])->onDone = autorelease_on_done;
    MMTrapEnvedSamplePlayer_noteOn_Rate(
            &spsps[(int)voiceNum],
            voiceNum,
            msg->data[2]/127.,
            MMInterpMethod_CUBIC,
            0,
            attackTime,
            releaseTime,
            sustainTime,
            theSound, 
            1,
            pow(2.,(msg->data[1]-60)/12.));

}

void MIDI_synth_cc_attackTime_control(void *data, MIDIMsg *msg)
{
    *((MMSample*)data) = (exp(pow(msg->data[2]/127.,2.))-1)/(M_E - 1.)*5.;
}

void MIDI_synth_cc_releaseTime_control(void *data, MIDIMsg *msg)
{
    *((MMSample*)data) = (exp(pow(msg->data[2]/127.,2.))-1)/(M_E - 1.)*10.;
}

void synth_control_setup(void)
{
    MIDI_Router_addCB(&midiRouter.router, MIDIMSG_NOTE_ON, 1, 
            MIDI_note_on_autorelease_do, spsps);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[1],0x0e,
            MIDI_synth_cc_attackTime_control,&attackTime);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[1],0x0f,
            MIDI_synth_cc_releaseTime_control,&releaseTime);
}

