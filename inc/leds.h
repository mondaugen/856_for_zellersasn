/* Copyright (c) 2016 Nicholas Esterer. All rights reserved. */

#ifndef LEDS_H
#define LEDS_H 

#include "stm32f4xx.h"

#define LED1_ENR                    RCC_AHB1ENR_GPIOEEN 
#define LED3_ENR                    RCC_AHB1ENR_GPIOEEN 
#define LED5_ENR                    RCC_AHB1ENR_GPIOEEN 
#define LED7_ENR                    RCC_AHB1ENR_GPIOEEN 
#define LED_DISCO_GREEN_ENR         RCC_AHB1ENR_GPIOGEN 

#define LED1_PORT                   GPIOE
#define LED3_PORT                   GPIOE
#define LED5_PORT                   GPIOE
#define LED7_PORT                   GPIOE
#define LED_DISCO_GREEN_PORT        GPIOG

#define LED1_PORT_PIN               3 
#define LED3_PORT_PIN               2 
#define LED5_PORT_PIN               5 
#define LED7_PORT_PIN               4 
#define LED_DISCO_GREEN_PORT_PIN    13

void leds_setup(void);
void led1_set(void);
void led1_reset(void);
void led3_set(void);
void led3_reset(void);
void led5_set(void);
void led5_reset(void);
void led7_set(void);
void led7_reset(void);
void led1_tog(void);
void led3_tog(void);
void led5_tog(void);
void led7_tog(void);
void led_disco_green_set(void);
void led_disco_green_reset(void);
void led_disco_green_tog(void);


#endif /* LEDS_H */
