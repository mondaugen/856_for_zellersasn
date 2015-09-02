#ifndef SWITCH_CONTROL_H
#define SWITCH_CONTROL_H 

#include <stdint.h> 

typedef struct __switch_control_t {
    /* The address of the GPIO port that toggles */
    volatile uint32_t *port_addr;
    /* Bit of the port */
    uint32_t port_bit;
    /* A function that will be called. It will be passed the state of the pin
     * (uint32_t) and a pointer to some data (void*) that it will maybe want to
     * use. */
    void (*func)(struct __switch_control_t *);
    /* Some auxiliary data that func can have access to */
    void *data;
    /* The next one in the list. This should be NULL if last in the list. */
    struct __switch_control_t *next;
} switch_control_t;

void switch_control_do_all(void);
void switch_control_init(switch_control_t *sc,
                         volatile uint32_t *port_addr,
                         uint32_t port_bit,
                         void (*func)(switch_control_t*),
                         void *data);
void switch_control_add(switch_control_t *sc);

#define switch_control_get_state(ctl) ((*((ctl)->port_addr) >> ctl->port_bit) & 0x1)
#define switch_control_set_state(ctl,state)\
    *((ctl)->port_addr) &= ~(0x1  << ctl->port_bit);\
    *((ctl)->port_addr) |= (state << ctl->port_bit)

#endif /* SWITCH_CONTROL_H */
