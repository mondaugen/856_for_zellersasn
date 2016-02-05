#include "switches.h"
#include "switch_control.h"
#include "synth_control.h" 

/* This structure is a subclass of a switch_control_t struct but includes two
 * additional port_addr and port_bit fields so that the state of two pins can be
 * checked. */
typedef struct __synth_switch_control_t {
    switch_control_t head;
    volatile uint32_t *port_addr;
    uint32_t port_bit;
} synth_switch_control_t;

static int switch_control_get_tristate(switch_control_t *sc)
{
        if (!switch_control_get_state(sc)) {
            return 0;
        } else if (!switch_control_get_state((synth_switch_control_t*)sc)) {
            return 2;
        } else {
            return 1;
        }
}

#define SYNTH_SWITCH_CONTROL(type,fun,c0,c1,c2)\
    static void synth_switch_control_ ## type ## _control(switch_control_t *sc)\
    {\
        switch (switch_control_get_tristate(sc)) {\
            case 0:\
                fun(c0);\
                break;\
            case 2:\
                fun(c2);\
                break;\
            case 1:\
                fun(c1);\
                break;\
        }\
    }

#define SYNTH_SWITCH_CONTROL_ONCHANGE(type,fun,c0,c1,c2)\
    static void synth_switch_control_ ## type ## _control(switch_control_t *sc)\
    {\
        /* sc->data contains the last state */\
        switch (switch_control_get_tristate(sc)) {\
            case 0:\
                fun(c0,sc->data);\
                break;\
            case 2:\
                fun(c2,sc->data);\
                break;\
            case 1:\
                fun(c1,sc->data);\
                break;\
        }\
    }

#define SYNTH_SWITCH_SETUP(type,sw,c0,c1,c2)\
    static void synth_switch_control_ ## type ## _setup(void)\
    {\
        static synth_switch_control_t _switch_control;\
        static type _last_state;\
        volatile uint32_t *_sw_addrs[] = {\
            sw ## _TOP_ADDR,\
            sw ## _BTM_ADDR};\
        uint32_t _sw_pins[] = {\
            sw ## _TOP_PORT_PIN,\
            sw ## _BTM_PORT_PIN};\
        /* Note that the following function sets the _sw_addrs and _sw_pins\
         * of the parent class! */\
        switch_control_init((switch_control_t*)&_switch_control,\
                _sw_addrs[1],/* Because btm is when the head of switch is up. */\
                _sw_pins[1],\
                synth_switch_control_ ## type ## _control,\
                NULL);\
        /* Note that the following sets the _sw_addrs and _sw_pins\
         * of the sub class! */\
        _switch_control.port_addr = _sw_addrs[0];\
        _switch_control.port_bit  = _sw_pins[0];\
        /* Call sc->fun with NULL data, forces setting of current value */\
        synth_switch_control_ ## type ## _control((switch_control_t*)&_switch_control);\
        /* Now set sc-data to _last_state */\
        ((switch_control_t*)&_switch_control)->data = (void*)_last_state;\
        /* Initialize last_state with current state on pin */\
        switch (switch_control_get_tristate((switch_control_t*)&_switch_control)) {\
            case 0:\
                _last_state = c0;\
                break;\
            case 2:\
                _last_state = c2;\
                break;\
            case 1:\
                _last_state = c1;\
                break;\
        }\
        switch_control_add((switch_control_t*)&_switch_control);\
    }

#define SYNTH_SWITCH_SETUP_CALL(type)\
    synth_switch_control_ ## type ## _setup()

#define SYNTH_SWITCH_CONTROL_TOG(name)\
    void synth_switch_control_ ## name ## _tog_func(switch_debouncer_t *sd)\
    {\
        synth_control_ ## name ## _tog();\
    }

#define SYNTH_SWITCH_SETUP_TOG(name,sw)\
    static void synth_switch_control_ ## name ## _tog_setup(void)\
    {\
        static mom_state_t mom_state = {\
            sw ## _TOG_ADDR,\
            sw ## _TOG_PORT_PIN,\
            sw ## _ADDR,\
            sw ## _PORT_PIN\
        };\
        static switch_debouncer_t debouncer;\
        static switch_control_t control;\
        switch_debouncer_init(&debouncer,\
                synth_switch_control_ ## name ## _tog_func,\
                2,\
                &mom_state);\
        switch_control_debounce_init(&control,&debouncer);\
        switch_control_add(&control);\
    }

#define SYNTH_SWITCH_SETUP_TOG_CALL(name)\
   synth_switch_control_ ## name ## _tog_setup() 


SYNTH_SWITCH_CONTROL(SynthControlEditingWhichParamsIndex,
        synth_control_set_editingWhichParams,0,1,2);
SYNTH_SWITCH_SETUP(SynthControlEditingWhichParamsIndex,SW2,0,1,2);
SYNTH_SWITCH_CONTROL(SynthControlPresetNumber,
        synth_control_set_presetNumber,0,1,2);
SYNTH_SWITCH_SETUP(SynthControlPresetNumber,
        SW5,0,1,2);
SYNTH_SWITCH_CONTROL_ONCHANGE(SynthControlPosMode,
        synth_control_set_posMode_onChange_curParams,
        SynthControlPosMode_STRIDE,
        SynthControlPosMode_ABSOLUTE,
        SynthControlPosMode_PITCH_RESET);
SYNTH_SWITCH_SETUP(SynthControlPosMode,SW1,
        SynthControlPosMode_STRIDE,
        SynthControlPosMode_ABSOLUTE,
        SynthControlPosMode_PITCH_RESET);
SYNTH_SWITCH_CONTROL(SynthControlDeltaButtonMode,
        synth_control_set_deltaButtonMode,
        SynthControlDeltaButtonMode_EVENT_DELTA_FREE,
        SynthControlDeltaButtonMode_EVENT_DELTA_QUANT,
        SynthControlDeltaButtonMode_INTERMITTENCY);
SYNTH_SWITCH_SETUP(SynthControlDeltaButtonMode,SW6,
        SynthControlDeltaButtonMode_EVENT_DELTA_FREE,
        SynthControlDeltaButtonMode_EVENT_DELTA_QUANT,
        SynthControlDeltaButtonMode_INTERMITTENCY);
SYNTH_SWITCH_CONTROL(SynthControlPitchIndex,
        synth_control_set_editing_which_pitch,
        0,
        1,
        2);
SYNTH_SWITCH_SETUP(SynthControlPitchIndex,SW8,
        0,
        1,
        2);
SYNTH_SWITCH_CONTROL_ONCHANGE(SynthControlRecMode,
        synth_control_set_recMode_onChange,
        SynthControlRecMode_NORMAL,
        SynthControlRecMode_REC_LEN_1_BEAT,
        SynthControlRecMode_REC_LEN_1_BEAT_REC_SCHED);
SYNTH_SWITCH_SETUP(SynthControlRecMode,SW4,
        SynthControlRecMode_NORMAL,
        SynthControlRecMode_REC_LEN_1_BEAT,
        SynthControlRecMode_REC_LEN_1_BEAT_REC_SCHED);
        /* Incase the switch gets caught down momentarily when toggling the
         * feedback, the gain mode will stay in wet mode. */
SYNTH_SWITCH_CONTROL(SynthControlGainMode,
        synth_control_set_gainMode,
        SynthControlGainMode_FADE,
        SynthControlGainMode_WET,
        SynthControlGainMode_WET); 
SYNTH_SWITCH_SETUP(SynthControlGainMode,SW7,
        SynthControlGainMode_FADE,
        SynthControlGainMode_WET,
        SynthControlGainMode_WET); 
SYNTH_SWITCH_CONTROL_TOG(record);
SYNTH_SWITCH_SETUP_TOG(record,FSW1);
SYNTH_SWITCH_CONTROL_TOG(schedulerState);
SYNTH_SWITCH_SETUP_TOG(schedulerState,FSW2);
SYNTH_SWITCH_CONTROL_TOG(presetRecall);
SYNTH_SWITCH_SETUP_TOG(presetRecall,MSW3_TOP);
SYNTH_SWITCH_CONTROL_TOG(presetStore);
SYNTH_SWITCH_SETUP_TOG(presetStore,MSW3_BTM);
SYNTH_SWITCH_CONTROL_TOG(feedback);
SYNTH_SWITCH_SETUP_TOG(feedback,MSW7_TOP);
SYNTH_SWITCH_CONTROL_TOG(pitch_reset);
SYNTH_SWITCH_SETUP_TOG(pitch_reset,MSW1_TOP);

void synth_switch_control_setup(void)
{
    SYNTH_SWITCH_SETUP_CALL(SynthControlEditingWhichParamsIndex);
    SYNTH_SWITCH_SETUP_CALL(SynthControlPresetNumber);
    SYNTH_SWITCH_SETUP_CALL(SynthControlPosMode);
    SYNTH_SWITCH_SETUP_CALL(SynthControlDeltaButtonMode);
    SYNTH_SWITCH_SETUP_CALL(SynthControlPitchIndex);
    SYNTH_SWITCH_SETUP_CALL(SynthControlRecMode);
    SYNTH_SWITCH_SETUP_CALL(SynthControlGainMode);
    SYNTH_SWITCH_SETUP_TOG_CALL(record);
    SYNTH_SWITCH_SETUP_TOG_CALL(schedulerState);
    SYNTH_SWITCH_SETUP_TOG_CALL(presetRecall);
    SYNTH_SWITCH_SETUP_TOG_CALL(presetStore);
    SYNTH_SWITCH_SETUP_TOG_CALL(feedback);
    SYNTH_SWITCH_SETUP_TOG_CALL(pitch_reset);
}
