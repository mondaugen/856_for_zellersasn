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
    MMTrapEnvedSamplePlayer_noteOnStruct no;
    no.note = voiceNum;
    no.amplitude = msg->data[2]/127.;
    no.index = noteParamSets[0].startPoint * MMArray_get_length(theSound.wavtab);
    no.attackTime = noteParamSets[0].attackTime;
    no.releaseTime = noteParamSets[0].releaseTime;
    no.sustainTime = ((noteParamSets[0].sustainTime *
        (MMSample)MMArray_get_length(theSound.wavtab) 
          / (MMSample)audio_hw_get_sample_rate(NULL) 
          - noteParamSets[0].attackTime 
          - noteParamSets[0].releaseTime) < 0) ? 
            0 :
            (noteParamSets[0].sustainTime *
             (MMSample)MMArray_get_length(theSound.wavtab) /
             (MMSample)audio_hw_get_sample_rate(NULL)
             - noteParamSets[0].attackTime - noteParamSets[0].releaseTime);
    no.samples = theSound.wavtab;
    /* 9 is added because MMCC_et12_rate considers pitch 69 to be a note of no
     * transposition. In this we consider middle C to be a note of no
     * transposition, so we add 9 (middle C is note 60) */
    no.rate = MMCC_et12_rate(msg->data[1] + 9);
    MMTrapEnvedSamplePlayer_noteOn_Rate(
            &spsps[(int)voiceNum], &no);
    MIDIMsg_free(msg);
}

void MIDI_synth_cc_envelopeTime_control(void *data, MIDIMsg *msg)
{
    env_map_attack_release_f(
            &((NoteParamSet*)data)[editingWhichParams].attackTime,
            &((NoteParamSet*)data)[editingWhichParams].releaseTime,
            msg->data[2]/127.,
            SYNTH_CONTROL_MIN_ATTACK_TIME,
            SYNTH_CONTROL_MAX_ATTACK_TIME,
            SYNTH_CONTROL_MIN_RELEASE_TIME,
            SYNTH_CONTROL_MAX_RELEASE_TIME);
    MIDIMsg_free(msg);
}

void MIDI_synth_cc_sustainTime_control(void *data, MIDIMsg *msg)
{
    /* Sustain time is relative to length of recording, so here just 0-1.
     * It is scaled this way so that the length selection is more precise for
     * short lengths and less precise for longer ones */
    ((NoteParamSet*)data)[editingWhichParams].sustainTime
        = powf(2.,-7.*(1 - msg->data[2]/127.));
    MIDIMsg_free(msg);
}

void MIDI_synth_cc_tempoBPM_control(void *data, MIDIMsg *msg)
{
    if (editingWhichParams == 0) {
        if (noteDeltaFromBuffer == 1) {
            /* the tempo is calculated so that 1 buffer * K is played per 1 beat
             * where K is some scalar. So if K > 1, the tempo is slower and K < 1
             * the tempo is faster */
            MMSample K = (msg->data[2] / 127.) * 0.1 + 0.95;
            *((MMSample*)data) = 60. * (MMSample)audio_hw_get_sample_rate(NULL) 
                / ((MMSample)((MMArray*)theSound.wavtab)->length * K);
        } else {
            *((MMSample*)data) = 40. + (240. - 40.)*msg->data[2] / 127.;
        }
    } else {
        noteParamSets[editingWhichParams].numRepeats = 
            (int)(16. * msg->data[2] / 128.);        
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
    switch (posMode) {
        case SynthControlPosMode_ABSOLUTE:
            ((NoteParamSet*)data)[editingWhichParams].startPoint
                = msg->data[2]/127.;
            break;
        case SynthControlPosMode_STRIDE:
            ((NoteParamSet*)data)[editingWhichParams].positionStride
                = msg->data[2] / 127. * 0.2 - 0.1;
            break;
    }
    MIDIMsg_free(msg);
}

void MIDI_synth_cc_eventDeltaBeats_control(void *data, MIDIMsg *msg)
{
    switch (deltaButtonMode) {
        case SynthControlDeltaButtonMode_EVENT_DELTA:
            if (editingWhichParams == 0) {
                /* The first set is quantized */
                ((NoteParamSet*)data)[editingWhichParams].eventDeltaBeats
                    = (MMSample)(1 + (int)(3. * (MMSample)msg->data[2]/127.));
            } else {
                /* Other sets are free */
                ((NoteParamSet*)data)[editingWhichParams].eventDeltaBeats
                    = powf(2.,-6.*(1 - msg->data[2]/127.));
                /*
                ((NoteParamSet*)data)[editingWhichParams].eventDeltaBeats
                    = 2. * (msg->data[2]+1.)/128.;
                */
            }
            break;
        case SynthControlDeltaButtonMode_INTERMITTENCY:
            ((NoteParamSet*)data)[editingWhichParams].intermittency =
                0 + (int)(3. * msg->data[2] / 127.);
            break;
    }
    MIDIMsg_free(msg);
}

void MIDI_synth_cc_offsetBeats_control(void *data, MIDIMsg *msg)
{
    if (editingWhichParams == 0) {
        ((NoteParamSet*)data)[editingWhichParams].offsetBeats
            = (msg->data[2] + 1.)/128.;
    } else {
        /* The offset is relative to the total event delta of the note event
         * with parameterSet 0 */
        ((NoteParamSet*)data)[editingWhichParams].offsetBeats
            = ((NoteParamSet*)data)[0].eventDeltaBeats * msg->data[2]/127.;
    }
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
        /* Only allow recording if recording is not scheduled */
        if (scheduleRecording == 0) {
            MIDI_synth_record_start_helper(data);
        }
    } else {
        MIDI_synth_record_stop_helper(data);
    }
    MIDIMsg_free(msg);
}

void MIDI_synth_cc_feedback_control(void *data, MIDIMsg *msg)
{
    if (msg->data[2] > 0) {
        /* Move fbBusSplitter to onNode */
        MMSigProc_insertAfter(fbOnNode,data);
        feedbackState = 1;
    } else {
        /* Move fbBusSplitter to offNode */
        MMSigProc_remove(data);
        /* Zero the feedback bus */
        memset(((MMBusSplitter*)data)->destBus->data,0,
                sizeof(MMSample)
                *((MMBusSplitter*)data)->destBus->size
                *((MMBusSplitter*)data)->destBus->channels);
        feedbackState = 0;
    }
    MIDIMsg_free(msg);
}

void MIDI_synth_cc_dryGain_control(void *data, MIDIMsg *msg)
{
    *((int16_t*)data) = msg->data[2];
    MIDIMsg_free(msg);
}

static void free_playing_spsp_voice(void *voice_number)
{
    MMEnvelope_startRelease(
            ((MMEnvedSamplePlayer*)&spsps[*((int*)voice_number)])->envelope);
}

/* Pass a pointer to the first NoteOnEventListNode */
static void schedulerState_off_helper(void *data)
{
    int n;
    for (n = 0; n < NUM_NOTE_PARAM_SETS; n++) {
        /* Disactivate all events of all parameter sets */
        set_noteOnEvents_inactive(
                (NoteOnEventListNode*)MMDLList_getNext(
                    &(((NoteOnEventListNode*)data)[n])));
        /* Reset the note on event counts */
        noteOnEventCount[n] = 0;
    }
    /* Disactivate the noteSchedEvents */
    set_noteSchedEvents_inactive(
            (NoteSchedEventListNode*)MMDLList_getNext(&noteSchedEventListHead));
    /* Turn off all playing notes */
    pm_do_for_each_busy_voice(&voiceAllocator,free_playing_spsp_voice);
    if (scheduleRecording == 1) {
        /* Discard what was last recorded */
        wtr.state = MMWavTabRecorderState_STOPPED;
    }
    schedulerState = 0;
}

static void schedulerState_on_helper(void)
{
    /* schedule 1st event which is initially active */
    schedule_noteSched_event(0, NoteSchedEvent_new(1));
    schedulerState = 1;
}

void MIDI_synth_cc_schedulerState_control(void *data, MIDIMsg *msg)
{
    if (msg->data[2] > 0) {
        schedulerState_on_helper();
    } else {
        schedulerState_off_helper(data);
    }
    MIDIMsg_free(msg);
}

void MIDI_synth_cc_editingWhichParams_control(void *data, MIDIMsg *msg)
{
    *((int*)data) = (int)(((MMSample)(NUM_NOTE_PARAM_SETS - 1))
                        * msg->data[2] / 127.);
    MIDIMsg_free(msg);
}

void MIDI_synth_cc_deltaButtonMode_control(void *data, MIDIMsg *msg)
{
    if (msg->data[2]) {
        *((SynthControlDeltaButtonMode*)data) = 
            SynthControlDeltaButtonMode_INTERMITTENCY;
    } else {
        *((SynthControlDeltaButtonMode*)data) =
            SynthControlDeltaButtonMode_EVENT_DELTA;
    }
    MIDIMsg_free(msg);
}

void MIDI_synth_cc_recordScheduling_control(void *data, MIDIMsg *msg)
{
    if (msg->data[2]) {
        /* If recording in progress, stop it */
        if (wtr.state == MMWavTabRecorderState_RECORDING) {
            wtr.state = MMWavTabRecorderState_STOPPED;
/*             MIDI_synth_record_stop_helper((void*)&wtr); */
        }
        *((int*)data) = 1;
        /* Set first scheduled recording to true so that when the first
         * scheduled recording happens, the buffers aren't swapped. This is
         * because the buffer it swaps with might contain garbage. */
        firstScheduledRecording = 1;
    } else {
        *((int*)data) = 0;
        /* Stop recording (it will most likely be in progress) but don't swap
         * the recording and playing sounds. We discard the most recent
         * recording to give the user time to flip the switch if they like the
         * previous recording */
        wtr.state = MMWavTabRecorderState_STOPPED;
    }
    MIDIMsg_free(msg);
}

void MIDI_synth_cc_gainMode_control(void *data, MIDIMsg *msg)
{
    if (msg->data[2]) {
        *((SynthControlGainMode*)data) = SynthControlGainMode_FADE;
    } else {
        *((SynthControlGainMode*)data) = SynthControlGainMode_WET;
    }

    MIDIMsg_free(msg);
}

void MIDI_synth_cc_gain_control(void *data, MIDIMsg *msg)
{
    switch (gainMode) {
        case SynthControlGainMode_WET:
            /* Don't do anything for now */
            break;
        case SynthControlGainMode_FADE:
            ((NoteParamSet*)data)[editingWhichParams].fadeRate
                = msg->data[2] / 127. * 2.;
    }
    MIDIMsg_free(msg);
}

void MIDI_synth_cc_posMode_control(void *data, MIDIMsg *msg)
{
    if (msg->data[2] > 0) {
        *((SynthControlPosMode*)data) = SynthControlPosMode_STRIDE;
    } else {
        *((SynthControlPosMode*)data) = SynthControlPosMode_ABSOLUTE;
    }
    MIDIMsg_free(msg);
}

void MIDI_synth_cc_presetNumber_control(void *data, MIDIMsg *msg)
{
    *((int*)data) = (int)(((MMSample)(NUM_SYNTH_CONTROL_PRESETS - 1))
                        * msg->data[2] / 127.);
    MIDIMsg_free(msg);
}

void MIDI_synth_cc_presetStore_control(void *data, MIDIMsg *msg)
{
    sc_presets_store(*((int*)data));
    MIDIMsg_free(msg);
}

void MIDI_synth_cc_presetRecall_control(void *data, MIDIMsg *msg)
{
    sc_presets_recall(*((int*)data));
    MIDIMsg_free(msg);
}

void MIDI_led1_toggle(void *data, MIDIMsg *msg)
{
    if (msg->data[2] > 0) {
        led1_set();
    } else {
        led1_reset();
    }
}

void MIDI_leds_cycle(void *data, MIDIMsg *msg)
{
    if (msg->data[2] & (1 << 3)) {
        led1_set();
    } else {
        led1_reset();
    }
    if (msg->data[2] & (1 << 2)) {
        led3_set();
    } else {
        led3_reset();
    }
    if (msg->data[2] & (1 << 1)) {
        led5_set();
    } else {
        led5_reset();
    }
    if (msg->data[2] & (1 << 0)) {
        led7_set();
    } else {
        led7_reset();
    }
}

void synth_control_setup(void)
{
    noteParamSets[0] = (NoteParamSet) {
        .attackTime = 0.01,     /* attackTime */
        .sustainTime  = 1,      /* sustainTime */
        .releaseTime = 0.01,    /* releaseTime */
        .eventDeltaBeats = 1,   /* eventDeltaBeats */
        .pitch = 60,            /* pitch */
        .amplitude = .5,        /* amplitude */
        .startPoint = 0,        /* startPoint */
        .numRepeats = 0,        /* The number of times repeated */
        .offsetBeats = 0,       /* The amount of beats offset from the beginning
                                   of the bar */
        .intermittency = 0,      /* Canonically the number of repeats that are
                                    ignored */
        .fadeRate      = 0,      /* Fade rate doesn't apply to the first
                                    parameter set */
        .positionStride = 0      /* The amount the starting point in the sample
                                    is advanced each time it is scheduled (if
                                    stride enabled) */
    };
    int n;
    for (n = 1; n < NUM_NOTE_PARAM_SETS; n++) {
        noteParamSets[n] = (NoteParamSet) {
            .attackTime = 0.01,     /* attackTime */
            .sustainTime  = 1,      /* sustainTime */
            .releaseTime = 0.01,    /* releaseTime */
            .eventDeltaBeats = 1,   /* eventDeltaBeats */
            .pitch = 60,            /* pitch */
            .amplitude = 0,         /* amplitude */
            .startPoint = 0,        /* startPoint */
            .numRepeats = 0,        /* The number of times repeated */
            .offsetBeats = 0,       /* The amount of beats offset from the
                                       beginning of the bar */
            .intermittency = 0,      /* Canonically the number of repeats that
                                        are ignored */
            .fadeRate      = 1,      /* Default fade rate of 1 means no fade */
            .positionStride = 0      /* The amount the starting point in the
                                        sample is advanced each time it is
                                        scheduled (if stride enabled) */
        };
    }
    noteDeltaFromBuffer = 0;
    dryGain             = 0;
    editingWhichParams  = 0;
    tempoBPM            = 120;
    posMode             = SynthControlPosMode_ABSOLUTE;
    deltaButtonMode     = SynthControlDeltaButtonMode_EVENT_DELTA;
    feedbackState       = 0;
    scheduleRecording   = 0;
    schedulerState      = 0;
    MIDI_Router_addCB(&midiRouter.router, MIDIMSG_NOTE_ON, 1, 
            MIDI_note_on_autorelease_do, spsps);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x0e,
            MIDI_synth_cc_envelopeTime_control,noteParamSets);
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
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x16,
            MIDI_synth_cc_offsetBeats_control,noteParamSets);
    /* The recorder trigger requires the zero crossing search be initialized */
    HannWindowTable_init(REC_LOOP_FADE_TIME_S * 2.);
//    ZeroxSearch_init(REC_LOOP_FADE_TIME_S * 2.);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x17,
            MIDI_synth_cc_record_trig, &wtr);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x18,
            MIDI_synth_cc_feedback_control, &fbBusSplitter);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x19,
            MIDI_synth_cc_noteDeltaFromBuffer_control,&noteDeltaFromBuffer);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x0D,
            MIDI_synth_cc_dryGain_control,&dryGain);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x21,
            MIDI_synth_cc_schedulerState_control,noteOnEventListHead);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x03,
            MIDI_synth_cc_editingWhichParams_control,&editingWhichParams);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x1c,
            MIDI_synth_cc_deltaButtonMode_control,&deltaButtonMode);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x22,
            MIDI_synth_cc_recordScheduling_control,&scheduleRecording);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x29,
            MIDI_synth_cc_gainMode_control,&gainMode);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x0c,
            MIDI_synth_cc_gain_control,noteParamSets);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x1d,
            MIDI_synth_cc_posMode_control,&posMode);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x04,
            MIDI_synth_cc_presetNumber_control,&presetNumber);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x25,
            MIDI_synth_cc_presetStore_control,&presetNumber);
    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x26,
            MIDI_synth_cc_presetRecall_control,&presetNumber);

    MIDI_CC_CB_Router_addCB(&midiRouter.cbRouters[0],0x0f,
            MIDI_leds_cycle,LED1_PORT);
}
