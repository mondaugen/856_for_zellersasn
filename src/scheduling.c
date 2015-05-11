#include "scheduling.h" 
#include "audio_setup.h" 
#include "mm_sample.h" 
#include "synth_control.h" 

struct __NoteOnEvent {
    MMEvent head;
    /* More stuff later */
};

MMSeq *sequence;

void scheduler_setup (void)
{
    *sequence = MMSeq_new();
    MMSeq_init(sequence, 0);
}

void schedule_event (uint64_t timeFromNow)
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
   /* TODO: trigger note on */
}
