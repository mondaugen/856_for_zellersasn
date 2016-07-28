/* Copyright (c) 2016 Nicholas Esterer. All rights reserved. */

#include <stddef.h> 
#include "stm32f4xx.h"
#include "timers.h" 
#include "leds.h" 

static timer_event_t *timer_events = NULL;

int timers_get_state(void)
{
    return (int)(TIM7->CR1 & TIM_CR1_CEN);
}

void timers_enable(void)
{
    /* enable auto-preload, interupt only on overflow, start counting */
    TIM7->CR1 =  TIM_CR1_URS | TIM_CR1_CEN;
}

void timers_disable(void)
{
    TIM7->CR1 &= ~TIM_CR1_CEN;
}

#ifdef TIMER_EVENT_TEST
static void timer_event_on_timeout_test(timer_event_t *ev)
{
    led_disco_green_tog();
    ev->time_rem = (uint32_t)1000;
}
#endif  


void timers_setup(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
    /* increments at 22.5 MHz with APB1 clock of 45MHz and a timer prescalar of
     * 2 (see clock tree). In other words, the clock counts at a rate of
     * 90MHz/(TIM7->PSC + 1) */
    TIM7->PSC = (uint16_t)3;
    /* overflows every millisecond with above increment frequecy */
    TIM7->ARR = (uint16_t)22500;
    /* enable interrupt */
    TIM7->DIER = TIM_DIER_UIE;
    /* attach ISRs */
    NVIC_EnableIRQ(TIM7_IRQn);
#ifdef TIMER_TEST
    leds_setup();
#endif
#ifdef TIMER_EVENT_TEST
    leds_setup();
    static timer_event_t tim_ev;
    timer_event_init(&tim_ev);
    tim_ev.on_timeout = timer_event_on_timeout_test;
    /* timeout every 1 second */
    tim_ev.time_rem = (uint32_t)1000;
    tim_ev.active = 1;
    timer_events_add_event(&tim_ev);
#endif  

}

/* Note that this will replace ev->next with timer_events. */
void timer_events_add_event(timer_event_t *ev)
{
    int _timstate;
    _timstate = timers_get_state();
    /* If timer on, disable */
    if (_timstate) {
        timers_disable();
    }
    if (ev) {
        timer_event_t *tmp = timer_events;
        timer_events = ev;
        ev->next = tmp;
    }
    /* If timers were on, enable */
    if (_timstate) {
        timers_enable();
    }
}

void timer_event_init(timer_event_t *ev)
{
    ev->next = NULL;
    ev->active = 0;
    ev->time_rem = 0;
    ev->on_timeout = NULL;
    ev->data = NULL;
}

static void timer_events_dec_all(timer_event_t *ev)
{
    while (ev != NULL) {
        if (ev->active && (ev->time_rem > 0)) {
            (ev->time_rem)--;
        }
        ev = ev->next;
    }
}

/* the ev->on_timeout function is responsible for setting ev->active to 0 if
 * repeat callings of on_timeout are unwanted */
static void timer_events_check_all(timer_event_t *ev)
{
    while (ev != NULL) {
        if (ev->active && TIMER_EVENT_TIMED_OUT(ev) && ev->on_timeout) {
            ev->on_timeout(ev);
        } 
        ev = ev->next;
    }
}

void TIM7_IRQHandler(void)
{
    NVIC_ClearPendingIRQ(TIM7_IRQn);
    uint16_t tmp;
    if (TIM7->SR & TIM_SR_UIF) {
        TIM7->SR &= ~TIM_SR_UIF;
        tmp = TIM7->CNT;
        timer_events_dec_all(timer_events);
        timer_events_check_all(timer_events);
#ifdef TIMER_TEST
        led_disco_green_tog();
#endif  
    }
}
