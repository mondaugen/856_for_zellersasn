#include "switch_control.h" 
/* Functions for looking at a GPIO port and calling functions on the value */

switch_control_t * _switch_controls;

void switch_control_do_all(void)
{
    switch_control_t *tmp;
    tmp = _switch_controls;
    while (tmp) {
        tmp->func(tmp, tmp->data);
        tmp = tmp->next;
    }
}

void switch_control_add(switch_control_t *sc)
{
    sc->next = _switch_controls;
    _switch_controls = sc;
}

void switch_control_init(switch_control_t *sc,
                         uint32_t *port_addr,
                         uint32_t port_bit,
                         void (*func)(switch_control_t*,void*),
                         void *data)
{
    sc->port_addr = port_addr;
    sc->port_bit = port_bit;
    sc->func = func;
    sc->data = data;
    sc->next = NULL;
}
