/* Routines that check the states of input (e.g., switches) to get information
 * while starting up. */
#include <stdint.h> 
#include <assert.h> 
#include "switches.h" 
#include "leds.h" 
#include "synth_midi_control.h" 
#include "timers.h" 

#define SUPO_REQUEST_MIDICH_CHK() (sw1_top_get_state()==0)
/* The index to check in the switch state array for the corresponding bit of a
 * half byte describing the MIDI channel (4 bits) from LSB to MSB. Last index
 * must be -1 to indicate end of array. */
#define SUPO_B_IS {17,13,9,5,-1}
/* The led functions to call in order from LSB to MSB if the bit is set. The
 * last item must be NULL to indicate the end. */
#define SUPO_LED_SET_FUNS {led7_set,led5_set,led3_set,led1_set,NULL}
/* The led functions to call to reset bit. Same order as above. */
#define SUPO_LED_RESET_FUNS {led7_reset,led5_reset,led3_reset,led1_reset,NULL}
/* Process the switch state to get the bit (e.g., if the switch being low (0)
 * actually means a high bit (1) then negate, etc.) */
#define SUPO_PROCESS_SS(x) (!(x)) 
/* Time to wait after switches released to debounce them */
#define SUPO_RESET_DEBOUNCE_TIME_MS 200 

void supo_debounce_on_timeout(timer_event_t *ev)
{
    ev->active = 0;
    *((uint32_t*)ev->data) = 0;
}


/* Checks switches and interprets configuration as MIDI channel, putting it in
 * datum pointed to by midi_channel. If no request is made, then the contents of
 * midi_channel are left untouched. */
void supo_get_midi_channel(int *midi_channel)
{
    static volatile uint32_t debounce_wait_flag;
    static timer_event_t debounce_event;
    void (*led_set_funs[])(void) = SUPO_LED_SET_FUNS;
    void (*led_reset_funs[])(void) = SUPO_LED_RESET_FUNS;
    if (SUPO_REQUEST_MIDICH_CHK()) {
       uint32_t switch_states[NUM_SWITCHES];
       int32_t supo_b_is[] = SUPO_B_IS;
       int32_t *ptr = supo_b_is;
       *midi_channel = 0;
       get_switch_states(switch_states);
       int n = 0;
       while (*ptr != -1) {
#ifdef DEBUG
           assert(*ptr < NUM_SWITCHES);
#endif  
           /* Switches are depressed if low, therefore the negation. */
           *midi_channel |= SUPO_PROCESS_SS(switch_states[*ptr]) << n;
           if (SUPO_PROCESS_SS(switch_states[*ptr])) {
               led_set_funs[n]();
           }
           ptr++;
           n++;
       }
       /* Display requested MIDI channel on LEDs while request button held */
       while (SUPO_REQUEST_MIDICH_CHK());
       /* Wait additional time to debounce. */
       int _timstate = timers_get_state();
       debounce_wait_flag = 1;
       timer_event_init(&debounce_event);
       debounce_event.data = (void*)&debounce_wait_flag;
       debounce_event.time_rem = SUPO_RESET_DEBOUNCE_TIME_MS;
       debounce_event.active = 1;
       debounce_event.on_timeout = supo_debounce_on_timeout;
       timer_events_add_event(&debounce_event);
       if (_timstate == 0) {
           timers_enable();
       }
       while (debounce_wait_flag);
       if (_timstate == 0) {
           timers_disable();
       }
       /* Reset LEDs */
       void(**funptr)(void) = led_reset_funs;
       while (*funptr) {
           (*funptr)();
           funptr++;
       }
       /* reset toggle states to get rid of erroneous toggling */
       reset_sw_toggle_states();
    }
}


void supo_get_preset_reset_rqst(int *request)
{
    static volatile uint32_t debounce_wait_flag;
    static timer_event_t debounce_event;
    /* If both footswitches down on startup, set presets to default values. This
     * doesn't overwrite what is in flash. That will only happen if the user
     * chooses to save the presets. */
    *request = 0;
    if ((fsw1_get_state() == 0) && (fsw2_get_state() == 0)) {
        *request = 1;
        /* Wait for switches to go high before continuing */
        while ((fsw1_get_state() == 0) || (fsw2_get_state() == 0));
        /* Wait additional time to debounce. */
        int _timstate = timers_get_state();
        debounce_wait_flag = 1;
        timer_event_init(&debounce_event);
        debounce_event.data = (void*)&debounce_wait_flag;
        debounce_event.time_rem = SUPO_RESET_DEBOUNCE_TIME_MS;
        debounce_event.active = 1;
        debounce_event.on_timeout = supo_debounce_on_timeout;
        timer_events_add_event(&debounce_event);
        if (_timstate == 0) {
            timers_enable();
        }
        while (debounce_wait_flag);
        /* flash leds to show it worked */
        led1_set();
        led3_set();
        led5_set();
        led7_set();
        /* wait again */
        debounce_event.time_rem = SUPO_RESET_DEBOUNCE_TIME_MS;
        debounce_event.active = 1;
        debounce_wait_flag = 1;
        while (debounce_wait_flag);
        /* reset leds */
        led1_reset();
        led3_reset();
        led5_reset();
        led7_reset();
        if (_timstate == 0) {
            timers_disable();
        }
        /* reset toggle states to get rid of erroneous toggling */
        reset_sw_toggle_states();
    }
}
