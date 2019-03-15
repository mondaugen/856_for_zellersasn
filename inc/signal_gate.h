#ifndef SIGNAL_GATE_H
#define SIGNAL_GATE_H 

struct signal_gate_init {
    unsigned int buffer_size;
    unsigned int ramp_time;
};

struct signal_gate *
signal_gate_new (struct signal_gate_init *i);
void
signal_gate_tick(struct signal_gate *s, float *x);
void 
signal_gate_set_state(struct signal_gate *s, int state);

#endif /* SIGNAL_GATE_H */
