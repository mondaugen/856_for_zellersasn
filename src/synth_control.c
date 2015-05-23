#include "synth_control.h"
#include "midi_setup.h" 
#include "poly_management.h" 
#include "wavetables.h" 
#include "mm_time.h" 
#include <math.h> 
#include "audio_setup.h" 
#include "scheduling.h" 

MMSample attackTime         = 0.01;
MMSample shortReleaseTime   = 0.025;
MMSample releaseTime        = 0.01;
MMSample sustainTime        = 0.5;
/* The amount the scheduler is incremented each block */
MMSample tempoBPM           = 120; 
/* The time between two scheduled events */
MMSample eventDeltaBeats    = 1; 
MMSample pitch              = 60;
MMSample amplitude          = .1;
MMSample startPoint         = 0; /* between 0 and 1 */
int noteDeltaFromBuffer     = 0;
int16_t  dryGain            = 0;
int      schedulerState     = 0;

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
        params->rate = pow(2.,(msg->data[1]-60)/12.);
        params->rateSource = MMPvtespRateSource_RATE;
        MMPolyManager_noteOn(pvm, (void*)params);
        MIDIMsg_free(msg);
    } else {
        MIDI_synth_note_off_do(data,msg);
    }
}

void autorelease_on_done(MMEnvedSamplePlayer * esp)
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
    MIDIMsg_free(msg);
}

void MIDI_synth_cc_attackTime_control(void *data, MIDIMsg *msg)
{
    *((MMSample*)data) = (exp(pow(msg->data[2]/127.,2.))-1)/(M_E - 1.)*5.;

    MIDIMsg_free(msg);
}

void MIDI_synth_cc_releaseTime_control(void *data, MIDIMsg *msg)
{
    *((MMSample*)data) = (exp(pow(msg->data[2]/127.,2.))-1)/(M_E - 1.)*10.;
    MIDIMsg_free(msg);
}

void MIDI_synth_cc_sustainTime_control(void *data, MIDIMsg *msg)
{
    *((MMSample*)data) = (msg->data[2]+1)/128.*2.;
    MIDIMsg_free(msg);
}

void MIDI_synth_cc_tempoBPM_control(void *data, MIDIMsg *msg)
{
    *((MMSample*)data) = 40. + (240. - 40.)*msg->data[2] / 127.;
    MIDIMsg_free(msg);
}

void MIDI_synth_cc_pitch_control(void *data, MIDIMsg *msg)
{
    *((MMSample*)data) = 48. + (72. - 48.) * msg->data[2]/127.;
    MIDIMsg_free(msg);
}

void MIDI_synth_cc_amplitude_control(void *data, MIDIMsg *msg)
{
    *((MMSample*)data) = msg->data[2]/127.;
    MIDIMsg_free(msg);
}

void MIDI_synth_cc_startPoint_control(void *data, MIDIMsg *msg)
{
    *((MMSample*)data) = msg->data[2]/127.;
    MIDIMsg_free(msg);
}

void MIDI_synth_cc_eventDeltaBeats_control(void *data, MIDIMsg *msg)
{
    *((MMSample*)data) = (msg->data[2]+1.)/128.;
    MIDIMsg_free(msg);
}

void MIDI_synth_cc_noteDeltaFromBuffer_control(void *data, MIDIMsg *msg)
{
    if (msg->data[2] > 0) {
        *((int*)data) = 1;
    } else  {
        *((int*)data) = 0;
    }
    MIDIMsg_free(msg);
}

/* Start recording with non-zero control change value. Stop with value of 0. */
void MIDI_synth_cc_record_trig(void *data, MIDIMsg *msg)
{
    if (msg->data[2] > 0) {
        /* Set to max length so it would be possible to record all the way to
         * the end of allocated space */
        ((MMArray*)recordingSound)->length = soundSampleMaxLength;
        ((MMWavTabRecorder*)data)->buffer = recordingSound;
        ((MMWavTabRecorder*)data)->currentIndex = 0;
        ((MMWavTabRecorder*)data)->state = MMWavTabRecorderState_RECORDING;
    } else {
        ((MMWavTabRecorder*)data)->state = MMWavTabRecorderState_STOPPED;
        /* If the index the recorder got to is greater than 2 times the fade
         * time in samples, do the fade by adding the fade time's worth of
         * samples to the beginning where the end and the beginning are weighted
         * by a window */
        if ((((MMWavTabRecorder*)data)->currentIndex >= hannWindowTableLength)
                && (noteDeltaFromBuffer == 0)) {
            int _n;
            for (_n = 0; _n < hannWindowTableLength/2; _n++) {
                MMWavTab_get(((MMWavTabRecorder*)data)->buffer,_n) =
                    MMWavTab_get(((MMWavTabRecorder*)data)->buffer,_n)
                        * hannWindowTable[_n]
                    + MMWavTab_get(((MMWavTabRecorder*)data)->buffer,
                           ((MMWavTabRecorder*)data)->currentIndex 
                            - hannWindowTableLength/2 + _n)
                        * hannWindowTable[hannWindowTableLength/2 + _n];
            }
            /* Set the length to the index the recorder got to minus half the
             * hannWindowTableLength */
            ((MMArray*)recordingSound)->length =
                ((MMWavTabRecorder*)data)->currentIndex
                    - hannWindowTableLength/2;
        } else {
            /* Set the length to the index the recorder got to and don't do any
             * windowing of the end points */
            ((MMArray*)recordingSound)->length =
                ((MMWavTabRecorder*)data)->currentIndex;
        }
        /* If the noteDeltaFromBuffer flag is set, compute the eventDelta from
         * the buffer length and set the playback rate to 1 */
        if (noteDeltaFromBuffer == 1) {
            eventDeltaBeats = ((MMArray*)recordingSound)->length 
                / (MMSample)audio_hw_get_sample_rate(NULL) * tempoBPM / 60.;
            pitch = 60.;
        }
        /* Swap the playing and the recording sounds */
        MMWavTab *tmp = recordingSound;
        recordingSound = theSound;
        theSound = tmp;
    }
    MIDIMsg_free(msg);
}

void MIDI_synth_cc_feedback_control(void *data, MIDIMsg *msg)
{
    if (msg->data[2] > 0) {
        /* Move fbBusBusSplitter to onNode */
        MMSigProc_remove(data);
        MMSigProc_insertAfter(fbOnNode,data);
    } else {
        /* Move fbBusBusSplitter to offNode */
        MMSigProc_remove(data);
        MMSigProc_insertAfter(fbOffNode,data);
    }
    MIDIMsg_free(msg);
}

void MIDI_synth_cc_dryGain_control(void *data, MIDIMsg *msg)
{
    *((int16_t*)data) = msg->data[2];
    MIDIMsg_free(msg);
}

void MIDI_synth_cc_schedulerState_control(void *data, MIDIMsg *msg)
{
    if (msg->data[2] > 0) {
        *((int*)data) = 1;
        /* schedule 1st event */
        schedule_event(0);
    } else {
        *((int*)data) = 0;
    }
    MIDIMsg_free(msg);
}

void synth_control_setup(void)
{
    MIDI_Router_addCB(&midiRouter.router, MIDIMSG_NOTE_ON, 1, 
            MIDI_note_on_autorelease_do, spsps);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x0e,
            MIDI_synth_cc_attackTime_control,&attackTime);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x0f,
            MIDI_synth_cc_releaseTime_control,&releaseTime);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x10,
            MIDI_synth_cc_sustainTime_control,&sustainTime);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x11,
            MIDI_synth_cc_tempoBPM_control,&tempoBPM);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x12,
            MIDI_synth_cc_pitch_control,&pitch);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x13,
            MIDI_synth_cc_amplitude_control,&amplitude);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x14,
            MIDI_synth_cc_startPoint_control,&startPoint);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x15,
            MIDI_synth_cc_eventDeltaBeats_control,&eventDeltaBeats);
    /* The recorder trigger requires the Hann window wavetable, initialize it
     * first. */
    HannWindowTable_init(REC_LOOP_FADE_TIME_S * 2.);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x17,
            MIDI_synth_cc_record_trig, &wtr);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x18,
            MIDI_synth_cc_feedback_control, &fbBusSplitter);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x19,
            MIDI_synth_cc_noteDeltaFromBuffer_control,&noteDeltaFromBuffer);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x0D,
            MIDI_synth_cc_dryGain_control,&dryGain);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x21,
            MIDI_synth_cc_schedulerState_control,&schedulerState);
}
