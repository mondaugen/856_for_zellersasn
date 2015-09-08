#include "switch_control.h"

//void synth_control_noteDeltaFromBuffer_control(void *data_,
//        uint32_t noteDeltaFromBuffer_param)
//void synth_control_record_trig(void *data_, uint32_t record_param)
//void synth_control_feedback_control(void *data_, uint32_t feedback_param)
//void synth_control_schedulerState_control(void *data_, uint32_t schedulerState_param)
//void synth_control_editingWhichParams_control(void *data_,
//        uint32_t editingWhichParams_param)
//void synth_control_deltaButtonMode_control(void *data_,
//        uint32_t deltaButtonMode_param)
//void synth_control_recordScheduling_control(void *data_,
//        uint32_t recordScheduling_param)
//void synth_control_gainMode_control(void *data_,
//        uint32_t gainMode_param)
//void synth_control_posMode_control(void *data_, uint32_t posMode_param)
//void synth_control_presetNumber_control(void *data_, uint32_t presetNumber_param)
//void synth_control_presetStore_control(void *data_, uint32_t presetStore_param)
//void synth_control_presetRecall_control(void *data_, uint32_t presetRecall_param)

/* sc must contain a uint32_t indicating which note the switch selects */
void synth_switch_control_editingWhichParams_control(switch_control_t *sc)
{
    switch (*((uint32_t*)sc->data)) {
        case 0:
        case 2:
            if (!switch_control_get_state(sc)) {
                synth_control_set_editingWhichParams(*((uint32_t*)sc->data));
            }
            break;
        case 1:
            if (switch_control_get_state(sc)) {
                synth_control_set_editingWhichParams(*((uint32_t*)sc->data));
            }
            break;
    }
}

/* TODO: Add switch port address and pin */
void synth_switch_control_editingWhichParams_setup(void)
{
    static switch_control_t _switch_controls[NUM_NOTE_PARAM_SETS];
    static uint32_t _param_indices[] = {0, 1, 2};
    
    uint32_t n;
    for (n = 0; n < NUM_NOTE_PARAM_SETS; n++) {
        switch_control_init(&_switch_controls[n],
                            _sw_addrs[n],
                            _sw_pins[n],
                            (void*)&_param_indices[n]);
    }
}
