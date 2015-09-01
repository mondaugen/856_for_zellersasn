#include "synth_control.h"
#include "midi_setup.h" 
#include "poly_management.h" 
#include "wavetables.h" 
#include "mm_time.h" 
#include <math.h> 
#include "audio_setup.h" 
#include "scheduling.h" 
#include "quantization_tables.h" 
#include "env_map.h" 
#include "mm_mark_zerox.h" 
#include <string.h> 
#include "mm_common_calcs.h" 
#include "leds.h" 

/* Stuff that could be saved */
NoteParamSet                noteParamSets[NUM_NOTE_PARAM_SETS];
MMSample                    tempoBPM; 
SynthControlPosMode         posMode;
SynthControlDeltaButtonMode deltaButtonMode;
SynthControlPitchMode       pitchMode;
SynthControlGainMode        gainMode;

/* Stuff that shouldn't really be saved */
int                         noteDeltaFromBuffer;
int                         editingWhichParams;
int                         currentPreset;
int                         feedbackState;
int                         scheduleRecording;
int                         firstScheduledRecording;
/* Is the scheduler on or off ? */
int                         schedulerState;
/* What preset would be recalled/stored. First preset is numbered 0. */
int                         presetNumber;

/* Stuff that might not make it into the final application */
int16_t                     dryGain;

static void schedulerState_off_helper(void *data);
static void schedulerState_on_helper(void);

void synth_control_envelopeTime_control(void *data, float envelopeTime_param)
{
    env_map_attack_release_f(
            &((NoteParamSet*)data)[editingWhichParams].attackTime,
            &((NoteParamSet*)data)[editingWhichParams].releaseTime,
            envelopeTime_param,
            SYNTH_CONTROL_MIN_ATTACK_TIME,
            SYNTH_CONTROL_MAX_ATTACK_TIME,
            SYNTH_CONTROL_MIN_RELEASE_TIME,
            SYNTH_CONTROL_MAX_RELEASE_TIME);
}

void synth_control_sustainTime_control(void *data, float sustainTime_param)
{
    /* Sustain time is relative to length of recording, so here just 0-1.
     * It is scaled this way so that the length selection is more precise for
     * short lengths and less precise for longer ones */
    ((NoteParamSet*)data)[editingWhichParams].sustainTime
        = powf(2.,-7.*(1 - sustainTime_param));
}

void synth_control_tempoBPM_control(void *data, float tempoBPM_param)
{
    if (editingWhichParams == 0) {
        if (noteDeltaFromBuffer == 1) {
            /* the tempo is calculated so that 1 buffer * K is played per 1 beat
             * where K is some scalar. So if K > 1, the tempo is slower and K < 1
             * the tempo is faster */
            MMSample K = tempoBPM_param * 0.1 + 0.95;
            *((MMSample*)data) = 60. * (MMSample)audio_hw_get_sample_rate(NULL) 
                / ((MMSample)((MMArray*)theSound.wavtab)->length * K);
        } else {
            *((MMSample*)data) = 40. + (240. - 40.)*tempoBPM_param;
        }
    } else {
        noteParamSets[editingWhichParams].numRepeats = 
            (int)(16. * msg->data[2] / 128.);        
    }
}

void synth_control_pitch_control(void *data, float pitch_param)
{
    ((NoteParamSet*)data)[editingWhichParams].pitch
        = 48. + (72. - 48.) * pitch_param;
}

void synth_control_amplitude_control(void *data, float amplitude_param)
{
    ((NoteParamSet*)data)[editingWhichParams].amplitude
        = amplitude_param;
}

void synth_control_startPoint_control(void *data, float startPoint_param)
{
    switch (posMode) {
        case SynthControlPosMode_ABSOLUTE:
            ((NoteParamSet*)data)[editingWhichParams].startPoint
                = startPoint_param;
            break;
        case SynthControlPosMode_STRIDE:
            ((NoteParamSet*)data)[editingWhichParams].positionStride
                = startPoint_param * 0.2 - 0.1;
            break;
    }
}

void synth_control_eventDeltaBeats_control(void *data, float eventDeltaBeats_param)
{
    switch (deltaButtonMode) {
        case SynthControlDeltaButtonMode_EVENT_DELTA:
            if (editingWhichParams == 0) {
                /* The first set is quantized */
                ((NoteParamSet*)data)[editingWhichParams].eventDeltaBeats
                    = (MMSample)(1 + (int)(3. * (MMSample)eventDeltaBeats_param));
            } else {
                /* Other sets are free */
                ((NoteParamSet*)data)[editingWhichParams].eventDeltaBeats
                    = powf(2.,-6.*(1 - eventDeltaBeats_param));
                /*
                ((NoteParamSet*)data)[editingWhichParams].eventDeltaBeats
                    = 2. * (msg->data[2]+1.)/128.;
                */
            }
            break;
        case SynthControlDeltaButtonMode_INTERMITTENCY:
            ((NoteParamSet*)data)[editingWhichParams].intermittency =
                0 + (int)(3. * eventDeltaBeats_param);
            break;
    }
}

void synth_control_offsetBeats_control(void *data, float offsetBeats_param)
{
    if (editingWhichParams == 0) {
        ((NoteParamSet*)data)[editingWhichParams].offsetBeats
            = offsetBeats_param + 0.001;
    } else {
        /* The offset is relative to the total event delta of the note event
         * with parameterSet 0 */
        ((NoteParamSet*)data)[editingWhichParams].offsetBeats
            = ((NoteParamSet*)data)[0].eventDeltaBeats * offsetBeats_param;
    }
}

void synth_control_dryGain_control(void *data, float dryGain_param)
{
    *((int16_t*)data) = (int16_t)(dryGain_param * 127.);
}

void synth_control_gain_control(void *data, float gain_param)
{
    switch (gainMode) {
        case SynthControlGainMode_WET:
            /* Don't do anything for now */
            break;
        case SynthControlGainMode_FADE:
            ((NoteParamSet*)data)[editingWhichParams].fadeRate
                = gain_param * 2.;
    }
}
