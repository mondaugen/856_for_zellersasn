/* Copyright (c) 2016 Nicholas Esterer. All rights reserved. */

#ifndef LEDS_H
#define LEDS_H 

#include "stm32f4xx.h"

#if defined(BOARD_V1)

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

#elif defined(BOARD_V2)

/* NOTE: LED3,5,7 called 2,3,4 on board schematic */

#define LED1_ENR                    RCC_AHB1ENR_GPIOAEN 
#define LED3_ENR                    RCC_AHB1ENR_GPIOCEN 
#define LED5_ENR                    RCC_AHB1ENR_GPIOCEN 
#define LED7_ENR                    RCC_AHB1ENR_GPIOAEN 

#define LED1_PORT                   GPIOA
#define LED3_PORT                   GPIOC
#define LED5_PORT                   GPIOC
#define LED7_PORT                   GPIOA

#define LED1_PORT_PIN               2 
#define LED3_PORT_PIN               5 
#define LED5_PORT_PIN               4 
#define LED7_PORT_PIN               7 

#endif  


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
