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
static void synth_switch_control_editingWhichParams_control(switch_control_t *sc)
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

void synth_switch_control_editingWhichParams_setup(void)
{
    static switch_control_t _switch_controls[NUM_NOTE_PARAM_SETS];
    static uint32_t _param_indices[] = {0, 1, 2};
    /* For the middle switch we just want to check that it is "off", so we
     * arbitrarily use the top port pin */
    volatile uint32_t *_sw_addrs[] = {SW2_TOP_ADDR,SW2_TOP_ADDR,SW2_BTM_ADDR};
    uint32_t _sw_pins[] = {SW2_TOP_PORT_PIN,SW2_TOP_PORT_PIN,SW2_BTM_PORT_PIN};
    uint32_t n;
    for (n = 0; n < NUM_NOTE_PARAM_SETS; n++) {
        switch_control_init(&_switch_controls[n],
                _sw_addrs[n],
                _sw_pins[n],
                synth_switch_control_editingWhichParams_control,
                (void*)&_param_indices[n]);
    }
}

void synth_switch_control_editingWhichParams_setup(void)
{
    static switch_control_t _switch_controls[NUM_NOTE_PARAM_SETS];
    static uint32_t _param_indices[] = {0, 1, 2};
    /* For the middle switch we just want to check that it is "off", so we
     * arbitrarily use the top port pin */
    volatile uint32_t *_sw_addrs[] = {SW2_TOP_ADDR,SW2_TOP_ADDR,SW2_BTM_ADDR};
    uint32_t _sw_pins[] = {SW2_TOP_PORT_PIN,SW2_TOP_PORT_PIN,SW2_BTM_PORT_PIN};
    uint32_t n;
    for (n = 0; n < NUM_NOTE_PARAM_SETS; n++) {
        switch_control_init(&_switch_controls[n],
                _sw_addrs[n],
                _sw_pins[n],
                synth_switch_control_editingWhichParams_control,
                (void*)&_param_indices[n]);
    }
}

/* sc must contain a uint32_t indicating which preset the switch selects */
static void synth_switch_control_presetNumber_control(switch_control_t *sc)
{
    switch (*((uint32_t*)sc->data)) {
        case 0:
        case 2:
            if (!switch_control_get_state(sc)) {
                synth_control_set_presetNumber(*((uint32_t*)sc->data));
            }
            break;
        case 1:
            if (switch_control_get_state(sc)) {
                synth_control_set_presetNumber(*((uint32_t*)sc->data));
            }
            break;
    }
}

void synth_switch_control_presetNumber_setup(void)
{
    static switch_control_t _switch_controls[NUM_SYNTH_CONTROL_PRESETS];
    static uint32_t _preset_numbers[] = {0, 1, 2};
    /* For the middle switch we just want to check that it is "off", so we
     * arbitrarily use the top port pin */
    volatile uint32_t *_sw_addrs[] = {SW5_TOP_ADDR,SW5_TOP_ADDR,SW5_BTM_ADDR};
    uint32_t _sw_pins[] = {SW5_TOP_PORT_PIN,SW5_TOP_PORT_PIN,SW5_BTM_PORT_PIN};
    uint32_t n;
    for (n = 0; n < ; n++) {
        switch_control_init(&_switch_controls[n],
                _sw_addrs[n],
                _sw_pins[n],
                synth_switch_control_presetNumber_control,
                (void*)&_preset_numbers[n]);
    }
}

/* sc must contain a SynthControlPosMode indicating which preset the switch
 * selects */
static void synth_switch_control_posMode_control(switch_control_t *sc)
{
    switch (*((uint32_t*)sc->data)) {
        case SynthControlPosMode_ABSOLUTE:
        case SynthControlPosMode_STRIDE: /* Should eventually be replaced */
            if (!switch_control_get_state(sc)) {
                synth_control_set_posMode(*((SynthControlPosMode*)sc->data));
            }
            break;
        case SynthControlPosMode_STRIDE:
            if (switch_control_get_state(sc)) {
                synth_control_set_posMode(*((SynthControlPosMode*)sc->data));
            }
            break;
    }
}

void synth_switch_control_posMode_setup(void)
{
    static switch_control_t _switch_controls[SYNTH_CONTROL_POS_MODE_N_MODES];
    static SynthControlPosMode _pos_modes[] = {
        SynthControlPosMode_ABSOLUTE,
        SynthControlPosMode_STRIDE,
        SynthControlPosMode_STRIDE /* Should eventually be replaced by something
                                      more interesting. */
    };
    /* For the middle switch we just want to check that it is "off", so we
     * arbitrarily use the top port pin */
    volatile uint32_t *_sw_addrs[] = {SW1_TOP_ADDR,SW1_TOP_ADDR,SW1_BTM_ADDR};
    uint32_t _sw_pins[] = {SW1_TOP_PORT_PIN,SW1_TOP_PORT_PIN,SW1_BTM_PORT_PIN};
    uint32_t n;
    for (n = 0; n < ; n++) {
        switch_control_init(&_switch_controls[n],
                _sw_addrs[n],
                _sw_pins[n],
                synth_switch_control_posMode_control,
                (void*)&_pos_modes[n]);
    }
}

/* sc must contain a SynthControlDeltaButtonMode indicating which preset the switch
 * selects */
static void synth_switch_control_deltaButtonMode_control(switch_control_t *sc)
{
    switch (*((uint32_t*)sc->data)) {
        case 0:
        case 2:
            if (!switch_control_get_state(sc)) {
                synth_control_set_deltaButtonMode(
                        *((SynthControlDeltaButtonMode*)sc->data));
            }
            break;
        case 1:
            if (switch_control_get_state(sc)) {
                synth_control_set_deltaButtonMode(
                        *((SynthControlDeltaButtonMode*)sc->data));
            }
            break;
    }
}

void synth_switch_control_deltaButtonMode_setup(void)
{
    static switch_control_t _switch_controls[SYNTH_CONTROL_POS_MODE_N_MODES];
    static SynthControlDeltaButtonMode _pos_modes[] = {
        SynthControlDeltaButtonMode_ABSOLUTE,
        SynthControlDeltaButtonMode_STRIDE,
        SynthControlDeltaButtonMode_STRIDE 
    };
    /* For the middle switch we just want to check that it is "off", so we
     * arbitrarily use the top port pin */
    volatile uint32_t *_sw_addrs[] = {SW1_TOP_ADDR,SW1_TOP_ADDR,SW1_BTM_ADDR};
    uint32_t _sw_pins[] = {SW1_TOP_PORT_PIN,SW1_TOP_PORT_PIN,SW1_BTM_PORT_PIN};
    uint32_t n;
    for (n = 0; n < ; n++) {
        switch_control_init(&_switch_controls[n],
                _sw_addrs[n],
                _sw_pins[n],
                synth_switch_control_deltaButtonMode_control,
                (void*)&_pos_modes[n]);
    }
}

