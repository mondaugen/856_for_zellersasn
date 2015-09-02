#ifndef SWITCH_CONTROL_H
#define SWITCH_CONTROL_H 

#include <stdint.h> 

typedef struct __switch_control_t {
    /* The address of the GPIO port that toggles */
    uint32_t *port_addr;
    /* Bit of the port */
    uint32_t port_bit;
    /* A function that will be called. It will be passed the state of the pin
     * (uint32_t) and a pointer to some data (void*) that it will maybe want to
     * use. */
    void (*func)(switch_control_t*,uint32_t);
    /* Some auxiliary data that will be passed to "func" */
    void *data;
    /* The next one in the list. This should be NULL if last in the list. */
    struct __switch_control_t *next;
} switch_control_t;

void switch_control_do_all(void);
void switch_control_init(switch_control_t *sc,
                         uint32_t *port_addr,
                         uint32_t port_bit,
                         void (*func)(switch_control_t*,void*),
                         void *data);
void switch_control_add(switch_control_t *sc);

#define switch_control_get_state(ctl) ((ctl->port_addr >> ctl->port_bit) & 0x1)

#endif /* SWITCH_CONTROL_H */
