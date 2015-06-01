#include "scheduling.h" 
#include "audio_setup.h" 
#include "mm_sample.h" 
#include "synth_control.h" 
#include "wavetables.h" 
#include "poly_management.h" 
#include "signal_chain.h" 
#include <math.h> 

/* The scheduler is set up so that the 32 LSB of the time are fractional part 
 * and the 32 MSB are the integer part of a beat index. So that this works, the
 * scheduler time is incremented 0xffffffff*tempoBPM beats per minute and events
 * are scheduled eventDeltaBeats*0xffffffff ticks apart. */

struct __NoteOnEvent {
    MMEvent head;
    NoteOnEventListNode *parent;
    int active; /* 1 if active, 0 if not */
    int parameterSet; /* Which set of parameters to use */
    int numRepeats;   /* The number of times to repeat (reschedule) after playing this event */
    int repeatIndex;  /* 0 means the first time this has been played, 1 means the first repeat, etc. */
    int intermittency;/* The intermittency scheme, see synth_control.h */
};

MMSeq *sequence;
NoteOnEventListNode noteOnEventListHead[NUM_NOTE_PARAM_SETS];

static void NoteOnEvent_happen(MMEvent *event);

void scheduler_setup(void)
{
    sequence = MMSeq_new();
    MMSeq_init(sequence, 0);
}

NoteOnEvent *NoteOnEvent_new(int active,
        int parameterSet,
        int numRepeats,
        int repeatIndex,
        int intermittency)
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
    ev->intermittency = intermittency;
    return ev;
}

void schedule_event(uint64_t timeFromNow, NoteOnEvent *ev)
{
    if (!ev) {
        return;
    }
    ev->parent = (NoteOnEventListNode*)malloc(sizeof(NoteOnEventListNode));
    if (!ev->parent) {
        free(ev);
        return;
    }
    ev->parent->child = ev;
    MMDLList_insertAfter((MMDLList*)&noteOnEventListHead[ev->parameterSet],
            (MMDLList*)ev->parent);
    MMSeq_scheduleEvent(sequence, (MMEvent*)ev,
            MMSeq_getCurrentTime(sequence) + timeFromNow);
}

static void NoteOnEvent_happen(MMEvent *event)
{
    /* only play if event is active */
    if (((NoteOnEvent*)event)->active == 1) {
        /* schedule next event if this event is of parameterSet 0 */
        if (((NoteOnEvent*)event)->parameterSet == 0) {
            schedule_event(
                    noteParamSets[((NoteOnEvent*)event)->parameterSet].eventDeltaBeats
                    * 0xffffffff,
                    NoteOnEvent_new(1,((NoteOnEvent*)event)->parameterSet,0,0,1));
            /* Schedule the other notes, too */
            int n;
            for (n = 1; n < NUM_NOTE_PARAM_SETS; n++) {
                schedule_event(
                        noteParamSets[n].offsetBeats
                        * 0xffffffff,
                        NoteOnEvent_new(1,
                            n,
                            noteParamSets[n].numRepeats,
                            0,
                            noteParamSets[n].intermittency));
            }
        } else {
            /* This is a repeating event of parameterSet other than 0 */
            if (((NoteOnEvent*)event)->numRepeats > 0) {
                schedule_event(
                        noteParamSets[((NoteOnEvent*)event)->parameterSet].eventDeltaBeats
                        * 0xffffffff,
                        NoteOnEvent_new(1,
                            ((NoteOnEvent*)event)->parameterSet,
                            ((NoteOnEvent*)event)->numRepeats - 1,
                            ((NoteOnEvent*)event)->repeatIndex + 1,
                            ((NoteOnEvent*)event)->intermittency));
            }
        }
        if ((((NoteOnEvent*)event)->repeatIndex 
                % ((NoteOnEvent*)event)->intermittency) == 0) {
            MMSample voiceNum = pm_get_next_free_voice_number();
            if (voiceNum != -1 && (noteParamSets[((NoteOnEvent*)event)->parameterSet].amplitude > 0.001)) { 
                /* there is a voice free */
                pm_claim_params_from_allocator((void*)&voiceAllocator,
                        (void*)&voiceNum);
                ((MMEnvedSamplePlayer*)&spsps[(int)voiceNum])->onDone =
                    autorelease_on_done;
                MMSample sustainTime, attackTime, releaseTime;
                /* sustainTime is the length of the audio, times
                 * noteParamSets[parameterSet].sustainTime *
                 * length_of_sound_seconds * (1 -
                 * noteParamSets[parameterSet].attackTime -
                 * noteParamSets[parametersSet].releaseTime) */
                sustainTime =
                    noteParamSets[((NoteOnEvent*)event)->parameterSet].sustainTime
                        * (MMSample)MMArray_get_length(theSound)
                        / (MMSample)audio_hw_get_sample_rate(NULL)
                        * (1.
                            - noteParamSets[((NoteOnEvent*)event)->parameterSet].attackTime
                            - noteParamSets[((NoteOnEvent*)event)->parameterSet].releaseTime);
                attackTime = 
                    noteParamSets[((NoteOnEvent*)event)->parameterSet].sustainTime
                        * (MMSample)MMArray_get_length(theSound)
                        / (MMSample)audio_hw_get_sample_rate(NULL)
                        * noteParamSets[((NoteOnEvent*)event)->parameterSet].attackTime;
                releaseTime = 
                    noteParamSets[((NoteOnEvent*)event)->parameterSet].sustainTime
                        * (MMSample)MMArray_get_length(theSound)
                        / (MMSample)audio_hw_get_sample_rate(NULL)
                        * noteParamSets[((NoteOnEvent*)event)->parameterSet].releaseTime;
                MMTrapEnvedSamplePlayer_noteOn_Rate(
                        &spsps[(int)voiceNum],
                        voiceNum,
                        noteParamSets[((NoteOnEvent*)event)->parameterSet].amplitude,
                        MMInterpMethod_CUBIC,
                        noteParamSets[((NoteOnEvent*)event)->parameterSet].startPoint
                        * MMArray_get_length(theSound),
                        attackTime,
                        releaseTime,
                        sustainTime,
                        theSound, 
                        1,
                        pow(2.,
                            (noteParamSets[((NoteOnEvent*)event)->parameterSet].pitch-60)/12.));
            }
        }
    }
    MMDLList_remove((MMDLList*)((NoteOnEvent*)event)->parent);
    free(((NoteOnEvent*)event)->parent);
    free(event);
}

void scheduler_incTimeAndDoEvents(void)
{
    MMSeq_incTime(sequence,(tempoBPM / 60.) 
            / ((MMSample)audio_hw_get_sample_rate(NULL) 
                / (MMSample)audio_hw_get_block_size(NULL)) * 0xffffffff);
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
