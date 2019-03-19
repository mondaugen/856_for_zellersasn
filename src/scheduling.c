/* Copyright (c) 2016 Nicholas Esterer. All rights reserved. */

#include "scheduling.h" 
#include "audio_setup.h" 
#include "wavetables.h" 
#include "poly_management.h" 
#include "signal_chain.h" 
#include "mm_common_calcs.h" 
#include "mm_envedsampleplayer_twobus.h"
#include <math.h> 

#ifndef MAX
#define MAX(x,y) (((x)>(y))?(x):(y))
#endif

#ifndef MIN
#define MIN(x,y) (((x)<(y))?(x):(y))
#endif 

#ifdef DEBUG
#include <assert.h>
#endif

/* The scheduler is set up so that the 32 LSB of the time are fractional part 
 * and the 32 MSB are the integer part of a beat index. So that this works, the
 * scheduler time is incremented 0xffffffff*tempoBPM beats per minute and events
 * are scheduled 0xffffffff ticks apart. */
#define SCHED_BEAT_RES (0x10000000ULL*6ULL) 

struct __RecordStartEvent {
    MMEvent head;
};

typedef struct __RecordStartEvent RecordStartEvent;

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
    int pitch_idx;      /* The index in the pitch table. Used when busses specify the pitch. */
    int swing_idx;      /* The index in the swing table. */
    SynthControlPitchMode pitch_mode;
};

/* Event that schedules other notes to play. */
struct __NoteSchedEvent {
    MMEvent head;
    NoteSchedEventListNode *parent;
    int active;
    int one_shot;
    MMSample pitch_offset;
    MMSample amplitude_scalar;
    SynthControlPitchMode pitch_mode;
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
static void RecordStartEvent_happen(MMEvent *event);
static MMTime sched_time_one_frame(void);

static sched_advance_mode_t sched_advance_mode = sched_advance_mode_INTERNAL;

sched_advance_mode_t scheduler_get_advance_mode(void)
{
    return sched_advance_mode;
}

void scheduler_advance_mode_cycle(void)
{
    sched_advance_mode += 1;
    if (sched_advance_mode == sched_advance_mode_END) {
        sched_advance_mode = sched_advance_mode_INTERNAL;
    }
}

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
        MMSample pitchOffset,
        int pitch_idx,
        int swing_idx)
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
    ev->pitch_idx = pitch_idx;
    ev->swing_idx = swing_idx;
    ev->parent = NULL;
    /* Default pitch mode is to look at the bus. */
    ev->pitch_mode = SynthControlPitchMode_BUS;
    return ev;
}

RecordStartEvent *RecordStartEvent_new(void)
{
    RecordStartEvent *ev = malloc(sizeof(RecordStartEvent));
    if (!ev) { return NULL; }
    ((MMEvent*)ev)->happen = RecordStartEvent_happen;
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
    /* Default pitch mode is to look at the bus. */
    ev->pitch_mode = SynthControlPitchMode_BUS;
    return ev;
}

void NoteSchedEvent_set_pitch_offset(NoteSchedEvent *nse, MMSample pitch)
{
    nse->pitch_offset = pitch;
}

void NoteSchedEvent_set_pitch_mode(NoteSchedEvent *nse, SynthControlPitchMode pitch_mode)
{
    nse->pitch_mode = pitch_mode;
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

void schedule_RecordStartEvent(
    uint64_t timeFromNow,
    RecordStartEvent *ev)
{
    if (!ev) { return; }
    MMSeq_scheduleEvent(sequence,
                        (MMEvent*)ev,
                        MMSeq_getCurrentTime(sequence) + timeFromNow);
}

void schedule_RecordStartEvent_next_frame(RecordStartEvent *ev)
{
    /* Determine the amount of time of 1 frame */
    schedule_RecordStartEvent(sched_time_one_frame(),ev);
}

static int short_attack_allowed(NoteOnEvent *noe)
{
    return (noe->parameterSet == 0)
            && ((noteParamSets[noe->parameterSet].startPoint + noe->currentPosition) == 0);
}

static int short_release_allowed(NoteOnEvent *noe)
{
    return (noe->parameterSet == 0)
            && ((noteParamSets[noe->parameterSet].startPoint + noe->currentPosition) == 0)
            && (noteParamSets[noe->parameterSet].sustainTime == 1);
}

static void NoteOnEvent_happen(MMEvent *event)
{
    NoteOnEvent *noe = (NoteOnEvent*)event;
    /* only play if event is active */
    if (noe->active == 1) {
        /* If numRepeats greater than 0, schedule the note to occur again. The
         * number of repeats should never be greater than 0 for an event of note
         * 0 */
        MMSample _next_pitch;
        int _next_repeat_idx, _cur_param_set, _next_pitch_idx, _next_swing_idx;
        _cur_param_set = noe->parameterSet;
        _next_repeat_idx = noe->repeatIndex + 1;
        _next_swing_idx = (noe->swing_idx + 1) % SYNTH_CONTROL_SWING_TABLE_SIZE;
        switch (noe->pitch_mode) {
            case SynthControlPitchMode_ABSOLUTE:
                _next_pitch_idx = _next_repeat_idx % SYNTH_CONTROL_PITCH_TABLE_SIZE;
                _next_pitch =  noteParamSets[_cur_param_set].pitches[_next_pitch_idx]
                    + noteParamSets[_cur_param_set].fine_pitches[_next_pitch_idx];
                break;
            case SynthControlPitchMode_RELATIVE:
                _next_pitch_idx = _next_repeat_idx % SYNTH_CONTROL_PITCH_TABLE_SIZE;
                _next_pitch =  noe->currentPitch 
                    + (noteParamSets[_cur_param_set].pitches[_next_pitch_idx]
                        + noteParamSets[_cur_param_set].fine_pitches[_next_pitch_idx]);
                break;
            case SynthControlPitchMode_BUS:
                _next_pitch_idx = _next_repeat_idx % SYNTH_CONTROL_PITCH_TABLE_SIZE;
                break;
        }
        if (noe->numRepeats > 0) {
            schedule_noteOn_event(
                    (noteParamSets[noe->parameterSet].eventDeltaBeats
                     * noteParamSets[noe->parameterSet].swing[_next_swing_idx])
                     * SCHED_BEAT_RES,
                    NoteOnEvent_new(1,
                        noe->parameterSet,
                        noe->numRepeats - 1,
                        _next_repeat_idx,
                        noe->currentFade 
                            * noteParamSets[noe->parameterSet].fadeRate,
                        MM_fwrap(noe->currentPosition
                            + noteParamSets[noe->parameterSet].positionStride,
                            0,1),
                         _next_pitch,
                         noe->pitchOffset,
                         _next_pitch_idx,
                         _next_swing_idx)
                         );
        }
        MMSample voiceNum = pm_get_next_free_voice_number();
        if (voiceNum != -1 && 
                ((noteParamSets[noe->parameterSet].amplitude
                    * noe->currentFade) > SCHEDULING_AMP_FLOOR)) { 
            /* there is a voice free */
            pm_claim_params_from_allocator((void*)&voiceAllocator,
                    (void*)&voiceNum);
            ((MMEnvedSamplePlayer*)&spsps[(int)voiceNum])->onDone =
                autorelease_on_done;
            MMEnvedSamplePlayerTwoBus_set_out_bus(
                    &spsps_2bus_wrappers[(int)voiceNum],
                    NULL);
            if ((noe->parameterSet == 0) && (synth_control_get_feedbackState() == 1)) {
                MMEnvedSamplePlayerTwoBus_set_out_bus(
                        &spsps_2bus_wrappers[(int)voiceNum],
                        signal_chain_get_n1fbBus());
            }
                
            MMTrapEnvedSamplePlayer_noteOnStruct no;
            no.note = voiceNum;
            no.amplitude = noe->currentFade * noteParamSets[noe->parameterSet].initialFade;
            no.p_gain = &noteParamSets[noe->parameterSet].amplitude; 
            no.index = MM_fwrap(
                noteParamSets[noe->parameterSet].startPoint + noe->currentPosition,
                0,1) * MMArray_get_length(theSound->wavtab);
            float attackTime = noteParamSets[noe->parameterSet].sustainTime
                    * (MMSample)MMArray_get_length(theSound->wavtab)
                    / (MMSample)audio_hw_get_sample_rate(NULL)
                    * noteParamSets[noe->parameterSet].attackTime,
                  releaseTime = noteParamSets[noe->parameterSet].sustainTime
                    * (MMSample)MMArray_get_length(theSound->wavtab)
                    / (MMSample)audio_hw_get_sample_rate(NULL)
                    * noteParamSets[noe->parameterSet].releaseTime;
            /* If this note belongs to N1, then it can have a very short attack
            time if it likes. This so that continuously feeding back N1 won't
            keep applying an envelope, which changes the audio unpleasantly. */
            if (short_attack_allowed(noe)) {
                no.attackTime = MIN(
                    MAX(attackTime,
                        0),
                        SYNTH_CONTROL_MAX_ATTACK_TIME);
            } else {
                no.attackTime = MIN(
                    MAX(attackTime,
                        SYNTH_CONTROL_MIN_ATTACK_TIME),
                        SYNTH_CONTROL_MAX_ATTACK_TIME);
            }
            if (short_release_allowed(noe)) {
                no.releaseTime = MIN(
                    MAX(releaseTime,
                        0),
                        SYNTH_CONTROL_MAX_RELEASE_TIME);
            } else {
                no.releaseTime = MIN(
                    MAX(releaseTime,
                        SYNTH_CONTROL_MIN_RELEASE_TIME),
                        SYNTH_CONTROL_MAX_RELEASE_TIME);
            }
            /* sustainTime is the length of the audio, times
             * noteParamSets[parameterSet].sustainTime *
             * length_of_sound_seconds * (1 -
             * noteParamSets[parameterSet].attackTime -
             * noteParamSets[parametersSet].releaseTime) */
            no.sustainTime =
                noteParamSets[noe->parameterSet].sustainTime
                * (MMSample)MMArray_get_length(theSound->wavtab)
                / (MMSample)audio_hw_get_sample_rate(NULL)
                * (1. - no.attackTime - no.releaseTime);
            no.samples = theSound->wavtab;
            MMWavTab_inc_n_players(theSound->wavtab);
            if (noe->pitch_mode == SynthControlPitchMode_BUS) {
                no.p_rate = &noteParamSets[noe->parameterSet].rate_busses[
                        noe->pitch_idx];
                no.rate = MMCC_et12_rate(noe->pitchOffset
                        + SYNTH_CONTROL_PITCH_OFFSET);
                MMTrapEnvedSamplePlayer_noteOn_pRate(
                        &spsps[(int)voiceNum], &no);
            } else {
                no.rate = MMCC_et12_rate(
                        synth_control_clip_valid_pitch(
                            noe->currentPitch
                            + noe->pitchOffset));
                MMTrapEnvedSamplePlayer_noteOn_Rate(
                        &spsps[(int)voiceNum], &no);
            }
        }
    }
    MMDLList_remove((MMDLList*)noe->parent);
    free(noe->parent);
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
                NoteOnEvent *noe = NoteOnEvent_new(1,
                            n,
                            noteParamSets[n].numRepeats,
                            0,
                            nse->amplitude_scalar,
                            noteParamSets[n].noteStrideAcc,
                            noteParamSets[n].pitches[0]
                                + noteParamSets[n].fine_pitches[0],
                            nse->pitch_offset,
                            0,
                            0);
                noe->pitch_mode = nse->pitch_mode;
                schedule_noteOn_event(
                        noteParamSets[n].offsetBeats
                        * SCHED_BEAT_RES,
                        noe);
                noteParamSets[n].noteStrideAcc = MM_fwrap(
                    noteParamSets[n].noteStrideAcc + noteParamSets[n].noteStride,
                    0,1);
            } else {
                noteOnEventCount[n] += 1;
            }
        }
        if (nse->one_shot == 0) {
            schedule_noteSched_event(SYNTH_CONTROL_DEFAULT_EVENTDELTABEATS
                    * SCHED_BEAT_RES,
                    NoteSchedEvent_new(1));
            /* Turn on LED */
            MEASURE_LED_SET();
            /* Schedule measure LED off event to turn off the measure indicating LED
             * */
            schedule_measureLEDOff_event(SYNTH_CONTROL_DEFAULT_EVENTDELTABEATS
                    * SCHED_BEAT_RES / MEASURE_LED_LENGTH_SCALAR,
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
             * synth_contol_record_stop_helper) */
            if (scheduleRecording > 0) {
                if (scheduleRecording == 2) {
                    scheduleRecording = 1;
                    synth_control_record_stop_helper(scrsh_source_USER);
                } else if (scheduleRecording == 1) {
                    synth_control_record_stop_helper(scrsh_source_SCHEDULER);
                }
                //synth_control_record_start_helper();
                ///* Start recording next frame */
                RecordStartEvent *rse = RecordStartEvent_new();
                schedule_RecordStartEvent_next_frame(rse);    
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

static void RecordStartEvent_happen(MMEvent *event)
{
    synth_control_record_start_helper();
    free(event);
}
 
static MMTime
sched_time_one_frame(void)
{
    return (synth_control_get_tempoBPM() / 60.) 
                / ((MMSample)audio_hw_get_sample_rate(NULL) 
                    / (MMSample)audio_hw_get_block_size(NULL)) * SCHED_BEAT_RES;
}

void scheduler_incTimeAndDoEvents(void)
{
#ifdef DEBUG
    assert(sequence);
#endif
    if (scheduler_get_advance_mode() == sched_advance_mode_INTERNAL) {
        MMSeq_incTime(sequence,sched_time_one_frame());
        MMSeq_doAllCurrentEvents(sequence);
    }
}

/* Like scheduler_incTimeAndDoEvents but increments one the clock 1/24 of a
 * quarter note (according to the most common midi clock rate) */
void scheduler_incTimeAndDoEvents_midiclock(void)
{
#ifdef DEBUG
    assert(sequence);
#endif 
    if (scheduler_get_advance_mode() == sched_advance_mode_MIDI) {
        MMSeq_incTime(sequence, SCHED_BEAT_RES / 24);
        MMSeq_doAllCurrentEvents(sequence);
    }
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
