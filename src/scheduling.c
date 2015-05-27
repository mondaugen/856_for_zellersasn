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
};

MMSeq *sequence;
NoteOnEventListNode noteOnEventListHead[NUM_NOTE_PARAM_SETS];

static void NoteOnEvent_happen(MMEvent *event);

void scheduler_setup(void)
{
    sequence = MMSeq_new();
    MMSeq_init(sequence, 0);
}

NoteOnEvent *NoteOnEvent_new(int active, int parameterSet)
{
    NoteOnEvent *ev = (NoteOnEvent*)malloc(sizeof(NoteOnEvent));
    if (!ev) {
        return NULL;
    }
    ((MMEvent*)ev)->happen = NoteOnEvent_happen;
    ev->active = active;
    ev->parameterSet = parameterSet;
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
                    NoteOnEvent_new(1,((NoteOnEvent*)event)->parameterSet));
            /* If multiple parameter sets are allowed, schedule the other ones,
             * too */
            if (multiParamSetsAllowed) {
                int n;
                for (n = 1; n < NUM_NOTE_PARAM_SETS; n++) {
                    schedule_event(
                            noteParamSets[n].eventDeltaBeats
                            * 0xffffffff,
                            NoteOnEvent_new(1,n));
                }
            }
        }
        MMSample voiceNum = pm_get_next_free_voice_number();
        if (voiceNum != -1) { 
            /* there is a voice free */
            pm_claim_params_from_allocator((void*)&voiceAllocator,
                    (void*)&voiceNum);
            ((MMEnvedSamplePlayer*)&spsps[(int)voiceNum])->onDone =
                autorelease_on_done;
            MMTrapEnvedSamplePlayer_noteOn_Rate(
                    &spsps[(int)voiceNum],
                    voiceNum,
                    noteParamSets[((NoteOnEvent*)event)->parameterSet].amplitude,
                    MMInterpMethod_CUBIC,
                    noteParamSets[((NoteOnEvent*)event)->parameterSet].startPoint
                        * MMArray_get_length(theSound),
                    noteParamSets[((NoteOnEvent*)event)->parameterSet].attackTime,
                    noteParamSets[((NoteOnEvent*)event)->parameterSet].releaseTime,
                    ((noteParamSets[((NoteOnEvent*)event)->parameterSet].sustainTime *
                          (MMSample)MMArray_get_length(theSound) /
                          (MMSample)audio_hw_get_sample_rate(NULL) -
                          noteParamSets[((NoteOnEvent*)event)->parameterSet].attackTime -
                          noteParamSets[((NoteOnEvent*)event)->parameterSet].releaseTime) < 0) ?  
                        0 :
                        (noteParamSets[((NoteOnEvent*)event)->parameterSet].sustainTime *
                         (MMSample)MMArray_get_length(theSound) /
                         (MMSample)audio_hw_get_sample_rate(NULL) -
                         noteParamSets[((NoteOnEvent*)event)->parameterSet].attackTime -
                         noteParamSets[((NoteOnEvent*)event)->parameterSet].releaseTime),
                    theSound, 
                    1,
                    pow(2.,
                        (noteParamSets[((NoteOnEvent*)event)->parameterSet].pitch-60)/12.));
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
