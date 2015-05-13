#include "scheduling.h" 
#include "audio_setup.h" 
#include "mm_sample.h" 
#include "synth_control.h" 
#include "wavetables.h" 
#include "poly_management.h" 
#include "signal_chain.h" 
#include <math.h> 

struct __NoteOnEvent {
    MMEvent head;
    /* More stuff later */
};

MMSeq *sequence;

static void NoteOnEvent_happen(MMEvent *event);

void scheduler_setup(void)
{
    sequence = MMSeq_new();
    MMSeq_init(sequence, 0);
}

void schedule_event(uint64_t timeFromNow)
{
    NoteOnEvent *ev = (NoteOnEvent*)malloc(sizeof(NoteOnEvent));
    ((MMEvent*)ev)->happen = NoteOnEvent_happen;
    MMSeq_scheduleEvent(sequence, (MMEvent*)ev,
            MMSeq_getCurrentTime(sequence) + timeFromNow);
}

static void NoteOnEvent_happen(MMEvent *event)
{
    /* schedule next event */
    schedule_event(eventDelta);
    MMSample voiceNum = pm_get_next_free_voice_number();
    if (voiceNum == -1) { 
        /* No more voices free */
        return;
    }
    pm_claim_params_from_allocator((void*)&voiceAllocator,
            (void*)&voiceNum);
    ((MMEnvedSamplePlayer*)&spsps[(int)voiceNum])->onDone = autorelease_on_done;
    MMTrapEnvedSamplePlayer_noteOn_Rate(
            &spsps[(int)voiceNum],
            voiceNum,
            amplitude,
            MMInterpMethod_CUBIC,
            startPoint * MMArray_get_length(theSound),
            attackTime,
            releaseTime,
            sustainTime,
            theSound, 
            1,
            pow(2.,(pitch-60)/12.));
    free(event);
}

void scheduler_incTimeAndDoEvents(void)
{
    MMSeq_incTime(sequence,schedulerInc);
    MMSeq_doAllCurrentEvents(sequence);
}
