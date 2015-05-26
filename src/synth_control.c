#include "synth_control.h"
#include "midi_setup.h" 
#include "poly_management.h" 
#include "wavetables.h" 
#include "mm_time.h" 
#include <math.h> 
#include "audio_setup.h" 
#include "scheduling.h" 

NoteParamSet noteParamSets[NUM_NOTE_PARAM_SETS];

/* The amount the scheduler is incremented each block */
MMSample tempoBPM; 
int noteDeltaFromBuffer;
int16_t  dryGain;
int editingWhichParams;

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
            noteParamSets[0].startPoint * MMArray_get_length(theSound),
            noteParamSets[0].attackTime,
            noteParamSets[0].releaseTime,
            ((noteParamSets[0].sustainTime * (MMSample)MMArray_get_length(theSound) 
                / (MMSample)audio_hw_get_sample_rate(NULL) 
                - noteParamSets[0].attackTime 
                - noteParamSets[0].releaseTime) < 0) ? 
                0 :
                (noteParamSets[0].sustainTime *
                (MMSample)MMArray_get_length(theSound) /
                (MMSample)audio_hw_get_sample_rate(NULL)
                    - noteParamSets[0].attackTime - noteParamSets[0].releaseTime),
            theSound, 
            1,
            pow(2.,(msg->data[1]-60)/12.));
    MIDIMsg_free(msg);
}

void MIDI_synth_cc_attackTime_control(void *data, MIDIMsg *msg)
{
    ((NoteParamSet*)data)[editingWhichParams].attackTime =
        (exp(pow(msg->data[2]/127.,2.))-1)/(M_E - 1.)*5.;
    MIDIMsg_free(msg);
}

void MIDI_synth_cc_releaseTime_control(void *data, MIDIMsg *msg)
{
    ((NoteParamSet*)data)[editingWhichParams].releaseTime =
        (exp(pow(msg->data[2]/127.,2.))-1)/(M_E - 1.)*10.;
    MIDIMsg_free(msg);
}

void MIDI_synth_cc_sustainTime_control(void *data, MIDIMsg *msg)
{
    /* Sustain time is relative to length of recording, so here just 0-1 */
    ((NoteParamSet*)data)[editingWhichParams].sustainTime
        = msg->data[2]/127.;
    MIDIMsg_free(msg);
}

void MIDI_synth_cc_tempoBPM_control(void *data, MIDIMsg *msg)
{
    if (noteDeltaFromBuffer == 1) {
        /* the tempo is calculated so that 1 buffer * K is played per 1 beat
         * where K is some scalar. So if K > 1, the tempo is slower and K < 1
         * the tempo is faster */
        MMSample K = (msg->data[2] / 127.) * 0.1 + 0.95;
        *((MMSample*)data) = 60. * (MMSample)audio_hw_get_sample_rate(NULL) 
            / ((MMSample)((MMArray*)theSound)->length * K);
    } else {
        *((MMSample*)data) = 40. + (240. - 40.)*msg->data[2] / 127.;
    }
    MIDIMsg_free(msg);
}

void MIDI_synth_cc_pitch_control(void *data, MIDIMsg *msg)
{
    ((NoteParamSet*)data)[editingWhichParams].pitch
        = 48. + (72. - 48.) * msg->data[2]/127.;
    MIDIMsg_free(msg);
}

void MIDI_synth_cc_amplitude_control(void *data, MIDIMsg *msg)
{
    ((NoteParamSet*)data)[editingWhichParams].amplitude
        = msg->data[2]/127.;
    MIDIMsg_free(msg);
}

void MIDI_synth_cc_startPoint_control(void *data, MIDIMsg *msg)
{
    ((NoteParamSet*)data)[editingWhichParams].startPoint =
        *((MMSample*)data) = msg->data[2]/127.;
    MIDIMsg_free(msg);
}

void MIDI_synth_cc_eventDeltaBeats_control(void *data, MIDIMsg *msg)
{
    ((NoteParamSet*)data)[editingWhichParams].eventDeltaBeats =
        (msg->data[2]+1.)/128.;
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
        if (((MMWavTabRecorder*)data)->currentIndex >= hannWindowTableLength) {
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
            if (noteDeltaFromBuffer == 0) {
                /* Set the length to the index the recorder got to minus half the
                 * hannWindowTableLength */
                ((MMArray*)recordingSound)->length =
                    ((MMWavTabRecorder*)data)->currentIndex
                    - hannWindowTableLength/2;
            } else {
                /* Set the length to the index the recorder got to so the loop
                 * is more precise */
                ((MMArray*)recordingSound)->length =
                    ((MMWavTabRecorder*)data)->currentIndex;
            }
        }
        /* If the noteDeltaFromBuffer flag is set, compute the tempo from
         * the buffer length, set the playback rate to 1 and set the eventDelta
         * to 1 beat so that the recording plays once per beat and the tempo is
         * one beat per length of recording */
        if (noteDeltaFromBuffer == 1) {
            /* Only the 0th noteParamSet's parameters are set as this is the
             * master and the other notes are slaves to its timing, buffer, etc.
             * */
            noteParamSets[0].eventDeltaBeats = 1;
            tempoBPM = 60. * (MMSample)audio_hw_get_sample_rate(NULL) 
                / (MMSample)((MMArray*)recordingSound)->length;
            noteParamSets[0].pitch = 60.;
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
        /* schedule 1st event which is active and uses the 0th parameter set */
        schedule_event(0, NoteOnEvent_new(1,0));
    } else {
        set_noteOnEvents_inactive((NoteOnEventListNode*)((MMDLList*)data)->next);
    }
    MIDIMsg_free(msg);
}

void MIDI_synth_cc_editingWhichParams_control(void *data, MIDIMsg *msg)
{
    if (msg->data[2]) {
        *((int*)data) = 1;
    } else {
        *((int*)data) = 0;
    }
    MIDIMsg_free(msg);
}

void synth_control_setup(void)
{
    int n;
    for (n = 0; n < NUM_NOTE_PARAM_SETS; n++) {
        noteParamSets[n] = (NoteParamSet) {
            .attackTime = 0.01,     /* attackTime */
            .sustainTime  = 1,      /* sustainTime */
            .releaseTime = 0.01,    /* releaseTime */
            .eventDeltaBeats = 1,   /* eventDeltaBeats */
            .pitch = 60,            /* pitch */
            .amplitude = .1,        /* amplitude */
            .startPoint = 0         /* startPoint */
        };
    }
    noteDeltaFromBuffer = 0;
    dryGain             = 0;
    editingWhichParams  = 0;
    tempoBPM            = 120;
    MIDI_Router_addCB(&midiRouter.router, MIDIMSG_NOTE_ON, 1, 
            MIDI_note_on_autorelease_do, spsps);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x0e,
            MIDI_synth_cc_attackTime_control,noteParamSets);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x0f,
            MIDI_synth_cc_releaseTime_control,noteParamSets);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x10,
            MIDI_synth_cc_sustainTime_control,noteParamSets);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x11,
            MIDI_synth_cc_tempoBPM_control,&tempoBPM);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x12,
            MIDI_synth_cc_pitch_control,noteParamSets);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x13,
            MIDI_synth_cc_amplitude_control,noteParamSets);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x14,
            MIDI_synth_cc_startPoint_control,noteParamSets);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x15,
            MIDI_synth_cc_eventDeltaBeats_control,noteParamSets);
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
            MIDI_synth_cc_schedulerState_control,&noteOnEventListHead);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x03,
            MIDI_synth_cc_editingWhichParams_control,&editingWhichParams);
}
