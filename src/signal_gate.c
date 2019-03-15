/* Simply allows passing a signal or not by multiplying it by 0 or 1. When
switching a ramp is used for smoothness. */
#include <stdlib.h>
#include "signal_gate.h"

struct signal_gate {
    /* 1 means on (passing signal) 0 means off */
    int state;
    /* the current scalar value */
    float scalar;
    /* the number of samples it expects to process */
    int buffer_size;
    /* the increment or decrement summed each sample when ramping */ 
    float ramp_inc;
};

struct signal_gate *
signal_gate_new (struct signal_gate_init *i)
{
    struct signal_gate *ret = calloc(1,sizeof(struct signal_gate));
    if (!ret) { goto fail; }
    ret->buffer_size = i->buffer_size;
    ret->ramp_inc = 1./i->ramp_time;
    if (ret->ramp_inc <= 0.) { goto fail; }
    return ret;
fail:
    if (ret) { free(ret); }
    return NULL;
}

void
signal_gate_tick(struct signal_gate *s, float *x)
{
    float scalar = s->scalar, ramp_inc = s->ramp_inc;
    int state = s->state;
    unsigned int n = s->buffer_size;
    while (n-- > 0) {
        if ((state == 0) && (scalar > 0)) {
            scalar -= ramp_inc;
            if (scalar <= 0) {
                scalar = 0; 
                ramp_inc = 0;
            }
        } else if ((state == 1) && (scalar < 1)) {
            scalar += ramp_inc;
            if (scalar >= 1) {
                scalar = 1;
                ramp_inc = 0;
            }
        }
        *x++ *= scalar;
    }
    s->scalar = scalar;
}

void 
signal_gate_set_state(struct signal_gate *s, int state)
{
    if (state != 0) { s->state = 1; }
    else { s->state = 0; }
}
