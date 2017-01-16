/* Copyright (c) 2016 Nicholas Esterer. All rights reserved. */

#include "scheduling.h" 
#include "audio_setup.h" 
#include "synth_control.h" 
#include "wavetables.h" 
#include "poly_management.h" 
#include "signal_chain.h" 
#include "mm_common_calcs.h" 
#include <math.h> 

#ifdef DEBUG
#include <assert.h>
#endif

/* The scheduler is set up so that the 32 LSB of the time are fractional part 
 * and the 32 MSB are the integer part of a beat index. So that this works, the
 * scheduler time is incremented 0xffffffff*tempoBPM beats per minute and events
 * are scheduled 0xffffffff ticks apart. */

struct __NoteOnEvent {
    MMEvent head;
    NoteOnEventListNode *parent;
    int active; /* 1 if active, 0 if not */
    int parameterSet; /* Which set of parameters to use */
    int numRepeats;   /* The number of times to repeat (reschedule) after
                         playing this event */
    int repeatIndex;  /* 0 means the first time this has been played, 1 means
                         the first repeat, etc. */
    MMSample currentFade; /* a value of 1 means no fade, and this decreases
                             every time if the parameterSet's fadeRate value < 1
                             */
    MMSample currentPosition; /* a value between 0 and 1 determining the
                                 starting point within the valid range of the
                                 buffer */
    MMSample currentPitch;   /* A pitch value s.t. 0 means no transposition, 7
                               means transpose up a 5th etc. */
    MMSample pitchOffset;
};

/* Event that schedules other notes to play. */
struct __NoteSchedEvent {
    MMEvent head;
    NoteSchedEventListNode *parent;
    int active;
    int one_shot;
    MMSample pitch_offset;
    MMSample amplitude_scalar;
};

/* Event that turns off LED indicating measure pulse */
struct __MeasureLEDOffEvent {
    MMEvent head;
    MeasureLEDOffEventListNode *parent;
    int active;
};

MMSeq *sequence;
NoteOnEventListNode noteOnEventListHead[NUM_NOTE_PARAM_SETS];
NoteSchedEventListNode noteSchedEventListHead;
MeasureLEDOffEventListNode measureLEDOffEventListHead;

static void NoteOnEvent_happen(MMEvent *event);
static void NoteSchedEvent_happen(MMEvent *event);
static void MeasureLEDOffEvent_happen(MMEvent *event);

void scheduler_setup(void)
{
    sequence = MMSeq_new();
    MMSeq_init(sequence, 0);
    int n;
    for (n = 0; n < NUM_NOTE_PARAM_SETS; n++) {
        noteOnEventCount[n] = 0;
        /* Initialize first list member as if it were a list item because the
         * program should never call on its child. Nodes whose children are
         * called are appended to this list node. */
        MMDLList_init(&noteOnEventListHead[n]);
    }
    MMDLList_init(&noteSchedEventListHead);
    MMDLList_init(&measureLEDOffEventListHead);
}

NoteOnEvent *NoteOnEvent_new(int active,
        int parameterSet,
        int numRepeats,
        int repeatIndex,
        MMSample currentFade,
        MMSample currentPosition,
        MMSample currentPitch,
        MMSample pitchOffset)
{
    NoteOnEvent *ev = (NoteOnEvent*)malloc(sizeof(NoteOnEvent));
    if (!ev) {
        return NULL;
    }
    ((MMEvent*)ev)->happen = NoteOnEvent_happen;
    ev->active = active;
    ev->parameterSet = parameterSet;
    ev->numRepeats = numRepeats;
    ev->repeatIndex = repeatIndex;
    ev->currentFade = currentFade;
    ev->currentPosition = currentPosition;
    ev->currentPitch = currentPitch;
    ev->pitchOffset = pitchOffset;
    ev->parent = NULL;
    return ev;
}

NoteSchedEvent *NoteSchedEvent_new(int active)
{
    NoteSchedEvent *ev = (NoteSchedEvent*)malloc(sizeof(NoteSchedEvent));
    if (!ev) {
        return NULL;
    }
    ((MMEvent*)ev)->happen = NoteSchedEvent_happen;
    ev->active = active;
    ev->parent = NULL;
    ev->one_shot = 0;
    ev->pitch_offset = 0.;
    ev->amplitude_scalar = 1.;
    return ev;
}

void NoteSchedEvent_set_pitch_offset(NoteSchedEvent *nse, MMSample pitch)
{
    nse->pitch_offset = pitch;
}

void NoteSchedEvent_set_amplitude_scalar(NoteSchedEvent *nse, MMSample amp)
{
    nse->amplitude_scalar = amp;
}

void NoteSchedEvent_set_one_shot(NoteSchedEvent *nse, int one_shot)
{
    nse->one_shot = one_shot;
}

MeasureLEDOffEvent *MeasureLEDOffEvent_new(int active)
{
    MeasureLEDOffEvent *ev = (MeasureLEDOffEvent*)malloc(sizeof(MeasureLEDOffEvent));
    if (!ev) {
        return NULL;
    }
    ((MMEvent*)ev)->happen = MeasureLEDOffEvent_happen;
    ev->active = active;
    ev->parent = NULL;
    return ev;
}

void schedule_measureLEDOff_event(uint64_t timeFromNow, MeasureLEDOffEvent *ev)
{
    if (!ev) {
        return;
    }
    ev->parent = (MeasureLEDOffEventListNode*)malloc(sizeof(MeasureLEDOffEventListNode));
    if (!ev->parent) {
        free(ev);
        return;
    }
    MMDLList_init(ev->parent);
    ev->parent->child = ev;
    MMDLList_insertAfter((MMDLList*)&measureLEDOffEventListHead,
            (MMDLList*)ev->parent);
    MMSeq_scheduleEvent(sequence, (MMEvent*)ev,
            MMSeq_getCurrentTime(sequence) + timeFromNow);
}

void schedule_noteOn_event(uint64_t timeFromNow, NoteOnEvent *ev)
{
    if (!ev) {
        return;
    }
    ev->parent = (NoteOnEventListNode*)malloc(sizeof(NoteOnEventListNode));
    if (!ev->parent) {
        free(ev);
        return;
    }
    MMDLList_init(ev->parent);
    ev->parent->child = ev;
    MMDLList_insertAfter((MMDLList*)&noteOnEventListHead[ev->parameterSet],
            (MMDLList*)ev->parent);
    MMSeq_scheduleEvent(sequence, (MMEvent*)ev,
            MMSeq_getCurrentTime(sequence) + timeFromNow);
}

void schedule_noteSched_event(uint64_t timeFromNow, NoteSchedEvent *ev)
{
    if (!ev) {
        return;
    }
    ev->parent = (NoteSchedEventListNode*)malloc(sizeof(NoteSchedEventListNode));
    if (!ev->parent) {
        free(ev);
        return;
    }
    MMDLList_init(ev->parent);
    ev->parent->child = ev;
    MMDLList_insertAfter((MMDLList*)&noteSchedEventListHead,(MMDLList*)ev->parent);
    MMSeq_scheduleEvent(sequence, (MMEvent*)ev,
            MMSeq_getCurrentTime(sequence) + timeFromNow);
}

static void NoteOnEvent_happen(MMEvent *event)
{
    /* only play if event is active */
    if (((NoteOnEvent*)event)->active == 1) {
        /* If numRepeats greater than 0, schedule the note to occur again. The
         * number of repeats should never be greater than 0 for an event of note
         * 0 */
        MMSample _next_pitch;
        int _next_repeat_idx, _cur_param_set, _next_pitch_idx;
        SynthControlPosMode _posMode;
        _cur_param_set = ((NoteOnEvent*)event)->parameterSet;
        _next_repeat_idx = ((NoteOnEvent*)event)->repeatIndex + 1;
        switch (pitchMode) {
            case SynthControlPitchMode_ABSOLUTE:
                _next_pitch_idx = _next_repeat_idx % SYNTH_CONTROL_PITCH_TABLE_SIZE;
                _next_pitch =  noteParamSets[_cur_param_set].pitches[_next_pitch_idx]
                    + noteParamSets[_cur_param_set].fine_pitches[_next_pitch_idx];
                break;
            case SynthControlPitchMode_RELATIVE:
                _next_pitch_idx = _next_repeat_idx % SYNTH_CONTROL_PITCH_TABLE_SIZE;
                _next_pitch =  ((NoteOnEvent*)event)->currentPitch 
                    + (noteParamSets[_cur_param_set].pitches[_next_pitch_idx]
                        + noteParamSets[_cur_param_set].fine_pitches[_next_pitch_idx]);
                break;
        }
        _posMode = noteParamSets[_cur_param_set].posMode;
        if (((NoteOnEvent*)event)->numRepeats > 0) {
            schedule_noteOn_event(
                    noteParamSets[((NoteOnEvent*)event)->parameterSet].eventDeltaBeats
                    * 0xffffffffULL,
                    NoteOnEvent_new(1,
                        ((NoteOnEvent*)event)->parameterSet,
                        ((NoteOnEvent*)event)->numRepeats - 1,
                        _next_repeat_idx,
                        ((NoteOnEvent*)event)->currentFade 
                            * noteParamSets[((NoteOnEvent*)event)->parameterSet].fadeRate,
                        /* If stride enabled, increment the previous current
                         * position by the stride amount, wrapping between 0 and
                         * 1, otherwise just put the position as dictated by the
                         * parameter set */
                        (_posMode == SynthControlPosMode_STRIDE) ?
                            MM_fwrap(((NoteOnEvent*)event)->currentPosition
                                + noteParamSets[((NoteOnEvent*)event)->parameterSet].positionStride,
                                0,1) :
                            noteParamSets[((NoteOnEvent*)event)->parameterSet].startPoint,
                         _next_pitch,
                         ((NoteOnEvent*)event)->pitchOffset));
        }
        MMSample voiceNum = pm_get_next_free_voice_number();
        if (voiceNum != -1 && 
                ((noteParamSets[((NoteOnEvent*)event)->parameterSet].fademin
                    * ((NoteOnEvent*)event)->currentFade) > SCHEDULING_AMP_FLOOR)) { 
            /* there is a voice free */
            pm_claim_params_from_allocator((void*)&voiceAllocator,
                    (void*)&voiceNum);
            ((MMEnvedSamplePlayer*)&spsps[(int)voiceNum])->onDone =
                autorelease_on_done;
            MMTrapEnvedSamplePlayer_noteOnStruct no;
            no.note = voiceNum;
            no.amplitude =
                (noteParamSets[((NoteOnEvent*)event)->parameterSet].fademin
                    * ((NoteOnEvent*)event)->currentFade) > 1. ? 
                    1. :
                    (noteParamSets[((NoteOnEvent*)event)->parameterSet].fademin
                            * ((NoteOnEvent*)event)->currentFade);
            no.index = ((NoteOnEvent*)event)->currentPosition
                        * MMArray_get_length(theSound->wavtab);
            /* sustainTime is the length of the audio, times
             * noteParamSets[parameterSet].sustainTime *
             * length_of_sound_seconds * (1 -
             * noteParamSets[parameterSet].attackTime -
             * noteParamSets[parametersSet].releaseTime) */
            no.sustainTime =
                noteParamSets[((NoteOnEvent*)event)->parameterSet].sustainTime
                * (MMSample)MMArray_get_length(theSound->wavtab)
                / (MMSample)audio_hw_get_sample_rate(NULL)
                * (1.
                        - noteParamSets[((NoteOnEvent*)event)->parameterSet].attackTime
                        - noteParamSets[((NoteOnEvent*)event)->parameterSet].releaseTime);
#ifdef SIG_CHAIN_FILL_BUF_ONES
            no.attackTime = 0;
            no.releaseTime = 0;
#else
            no.attackTime = 
                noteParamSets[((NoteOnEvent*)event)->parameterSet].sustainTime
                * (MMSample)MMArray_get_length(theSound->wavtab)
                / (MMSample)audio_hw_get_sample_rate(NULL)
                * noteParamSets[((NoteOnEvent*)event)->parameterSet].attackTime;
            no.releaseTime = 
                noteParamSets[((NoteOnEvent*)event)->parameterSet].sustainTime
                * (MMSample)MMArray_get_length(theSound->wavtab)
                / (MMSample)audio_hw_get_sample_rate(NULL)
                * noteParamSets[((NoteOnEvent*)event)->parameterSet].releaseTime;
#endif
            no.samples = theSound->wavtab;
            MMWavTab_inc_n_players(theSound->wavtab);
            /* 69 is added because MMCC_et12_rate considers pitch 69 to be a note of no
             * transposition. In this we consider 0 to be a note of no
             * transposition, so we add 69 */
            no.rate = MMCC_et12_rate(
                    synth_control_clip_valid_pitch(
                        ((NoteOnEvent*)event)->currentPitch
                        + 69
                        + ((NoteOnEvent*)event)->pitchOffset));
            MMTrapEnvedSamplePlayer_noteOn_Rate(
                    &spsps[(int)voiceNum], &no);
        }
    }
    MMDLList_remove((MMDLList*)((NoteOnEvent*)event)->parent);
    free(((NoteOnEvent*)event)->parent);
    free(event);
}

static void NoteSchedEvent_happen(MMEvent *event)
{
    NoteSchedEvent *nse = (NoteSchedEvent*)event;
    if (nse->active == 1) {
        /* Schedule notes */
        int n;
        for (n = 0; n < NUM_NOTE_PARAM_SETS; n++) {
            if ((noteOnEventCount[n]
                    >= noteParamSets[n].intermittency) 
                || (nse->one_shot == 1)) {
                noteOnEventCount[n] = 0;
                schedule_noteOn_event(
                        noteParamSets[n].offsetBeats
                        * 0xffffffffULL,
                        NoteOnEvent_new(1,
                            n,
                            noteParamSets[n].numRepeats,
                            0,
                            nse->amplitude_scalar,
                            noteParamSets[n].startPoint,
                            noteParamSets[n].pitches[0]
                                + noteParamSets[n].fine_pitches[0],
                            nse->pitch_offset));
            } else {
                noteOnEventCount[n] += 1;
            }
        }
        if (nse->one_shot == 0) {
            schedule_noteSched_event(SYNTH_CONTROL_DEFAULT_EVENTDELTABEATS
                    * 0xffffffffULL,
                    NoteSchedEvent_new(1));
            /* Turn on LED */
            MEASURE_LED_SET();
            /* Schedule measure LED off event to turn off the measure indicating LED
             * */
            schedule_measureLEDOff_event(SYNTH_CONTROL_DEFAULT_EVENTDELTABEATS
                    * 0xffffffffULL / MEASURE_LED_LENGTH_SCALAR,
                    MeasureLEDOffEvent_new(1));
            /* If scheduled recording enabled, stop the previous recording and start
             * a new one. It is always okay to stop the recording, because the
             * scheduleRecording flag is set when recording is turned off in the
             * SynthControlRecMode_REC_LEN_1_BEAT_REC_SCHED state, and actually not
             * turned off at that point (as it is with SynthControlRecMode_REC_LEN_1_BEAT)
             * If scheduleRecording is 2, then this means the recording was just
             * scheduled by the user, if 1 it means the sequencer scheduled it.
             * It is also okay to stop recording, even if not currently
             * recording, in this case, the request is ignored (see
             * synth_contol_record_stop_helper*/
            if (scheduleRecording > 0) {
                if (scheduleRecording == 2) {
                    scheduleRecording = 1;
                    synth_control_record_stop_helper(scrsh_source_USER);
                } else if (scheduleRecording == 1) {
                    synth_control_record_stop_helper(scrsh_source_SCHEDULER);
                }
                synth_control_record_start_helper();
            }
        }
    }
    MMDLList_remove((MMDLList*)((NoteSchedEvent*)event)->parent);
    free(((NoteSchedEvent*)event)->parent);
    free(event);
}

static void MeasureLEDOffEvent_happen(MMEvent *event)
{
    if (((MeasureLEDOffEvent*)event)->active == 1) {
       MEASURE_LED_RESET();
    } 
    MMDLList_remove((MMDLList*)((MeasureLEDOffEvent*)event)->parent);
    free(((MeasureLEDOffEvent*)event)->parent);
    free(event);
}

void scheduler_incTimeAndDoEvents(void)
{
#ifdef DEBUG
    assert(sequence);
#endif
    MMSeq_incTime(sequence,(synth_control_get_tempoBPM() / 60.) 
            / ((MMSample)audio_hw_get_sample_rate(NULL) 
                / (MMSample)audio_hw_get_block_size(NULL)) * 0xffffffffULL);
    MMSeq_doAllCurrentEvents(sequence);
}

/* It could be that head is a sentinel and contains no child (as is the case
 * with noteOnEventListHead). In that case, be sure to pass the next in the
 * list, e.g., set_noteOnEvents_active(noteOnEventListHead.next) */
void set_noteOnEvents_active(NoteOnEventListNode *head)
{
    while (head) {
        head->child->active = 1;
        head = (NoteOnEventListNode*)((MMDLList*)head)->next;
    }
}

/* See set_noteOnEvents_active for tips. */
void set_noteOnEvents_inactive(NoteOnEventListNode *head)
{
    while (head) {
        head->child->active = 0;
        head = (NoteOnEventListNode*)((MMDLList*)head)->next;
    }
}

/* It could be that head is a sentinel and contains no child (as is the case
 * with noteSchedEventListHead). In that case, be sure to pass the next in the
 * list, e.g., set_noteSchedEvents_active(noteSchedEventListHead.next) */
void set_noteSchedEvents_active(NoteSchedEventListNode *head)
{
    while (head) {
        head->child->active = 1;
        head = (NoteSchedEventListNode*)((MMDLList*)head)->next;
    }
}

/* See set_noteSchedEvents_active for tips. */
void set_noteSchedEvents_inactive(NoteSchedEventListNode *head)
{
    while (head) {
        head->child->active = 0;
        head = (NoteSchedEventListNode*)((MMDLList*)head)->next;
    }
}

/* See set_noteSchedEvents_active for tips. */
void set_measureLEDOffEvents_inactive(MeasureLEDOffEventListNode *head)
{
    while (head) {
        head->child->active = 0;
        head = (MeasureLEDOffEventListNode*)((MMDLList*)head)->next;
    }
}
