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
};

MMSeq *sequence;
NoteOnEventListNode noteOnEventListHead;

static void NoteOnEvent_happen(MMEvent *event);

void scheduler_setup(void)
{
    sequence = MMSeq_new();
    MMSeq_init(sequence, 0);
}

void schedule_event(uint64_t timeFromNow)
{
    NoteOnEvent *ev = (NoteOnEvent*)malloc(sizeof(NoteOnEvent));
    if (!ev) {
        return;
    }
    ((MMEvent*)ev)->happen = NoteOnEvent_happen;
    ev->parent = (NoteOnEventListNode*)malloc(sizeof(NoteOnEventListNode));
    if (!ev->parent) {
        free(ev);
        return;
    }
    ev->parent->child = ev;
    MMDLList_insertAfter((MMDLList*)&noteOnEventListHead,(MMDLList*)ev->parent);
    /* Event is initially active */
    ev->active = 1;
    MMSeq_scheduleEvent(sequence, (MMEvent*)ev,
            MMSeq_getCurrentTime(sequence) + timeFromNow);
}

static void NoteOnEvent_happen(MMEvent *event)
{
    /* only play if event is active */
    if (((NoteOnEvent*)event)->active == 1) {
        /* schedule next event */
        schedule_event(eventDeltaBeats * 0xffffffff);
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
                    amplitude,
                    MMInterpMethod_CUBIC,
                    startPoint * MMArray_get_length(theSound),
                    attackTime,
                    releaseTime,
                    ((sustainTime * (MMSample)MMArray_get_length(theSound) 
                        / (MMSample)audio_hw_get_sample_rate(NULL) - attackTime 
                        - releaseTime) < 0) ? 
                            0 :
                            (sustainTime *
                            (MMSample)MMArray_get_length(theSound) /
                            (MMSample)audio_hw_get_sample_rate(NULL)
                            - attackTime - releaseTime),
                    theSound, 
                    1,
                    pow(2.,(pitch-60)/12.));
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
