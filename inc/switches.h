#ifndef SWITCHES_H
#define SWITCHES_H 

#include <stdint.h> 
#include "stm32f4xx.h"

#define NUM_SWITCHES    18

#define FSW1_ENR        RCC_AHB1ENR_GPIOBEN
#define FSW2_ENR        RCC_AHB1ENR_GPIOEEN

#define SW1_TOP_ENR      RCC_AHB1ENR_GPIOGEN
#define SW1_BTM_ENR      RCC_AHB1ENR_GPIOGEN
#define SW2_TOP_ENR      RCC_AHB1ENR_GPIODEN
#define SW2_BTM_ENR      RCC_AHB1ENR_GPIODEN
#define SW3_TOP_ENR      RCC_AHB1ENR_GPIODEN
#define SW3_BTM_ENR      RCC_AHB1ENR_GPIOBEN
#define SW4_TOP_ENR      RCC_AHB1ENR_GPIOBEN
#define SW4_BTM_ENR      RCC_AHB1ENR_GPIOBEN
#define SW5_TOP_ENR      RCC_AHB1ENR_GPIOBEN
#define SW5_BTM_ENR      RCC_AHB1ENR_GPIODEN
#define SW6_TOP_ENR      RCC_AHB1ENR_GPIODEN
#define SW6_BTM_ENR      RCC_AHB1ENR_GPIODEN
#define SW7_TOP_ENR      RCC_AHB1ENR_GPIODEN
#define SW7_BTM_ENR      RCC_AHB1ENR_GPIOCEN
#define SW8_TOP_ENR      RCC_AHB1ENR_GPIOBEN
#define SW8_BTM_ENR      RCC_AHB1ENR_GPIOCEN

#define FSW1_PORT       GPIOB
#define FSW2_PORT       GPIOE

#define SW1_TOP_PORT     GPIOG
#define SW1_BTM_PORT     GPIOG
#define SW2_TOP_PORT     GPIOD
#define SW2_BTM_PORT     GPIOD
#define SW3_TOP_PORT     GPIOD
#define SW3_BTM_PORT     GPIOB
#define SW4_TOP_PORT     GPIOB
#define SW4_BTM_PORT     GPIOB
#define SW5_TOP_PORT     GPIOB
#define SW5_BTM_PORT     GPIOD
#define SW6_TOP_PORT     GPIOD
#define SW6_BTM_PORT     GPIOD
#define SW7_TOP_PORT     GPIOD
#define SW7_BTM_PORT     GPIOC
#define SW8_TOP_PORT     GPIOB
#define SW8_BTM_PORT     GPIOC

#define FSW1_PORT_PIN   4
#define FSW2_PORT_PIN   6

#define SW1_TOP_PORT_PIN 2
#define SW1_BTM_PORT_PIN 3
#define SW2_TOP_PORT_PIN 12
#define SW2_BTM_PORT_PIN 13
#define SW3_TOP_PORT_PIN 11 
#define SW3_BTM_PORT_PIN 0
#define SW4_TOP_PORT_PIN 1
#define SW4_BTM_PORT_PIN 7
#define SW5_TOP_PORT_PIN 8
#define SW5_BTM_PORT_PIN 7
#define SW6_TOP_PORT_PIN 4
#define SW6_BTM_PORT_PIN 2
#define SW7_TOP_PORT_PIN 3
#define SW7_BTM_PORT_PIN 8
#define SW8_TOP_PORT_PIN 9
#define SW8_BTM_PORT_PIN 6

#define FSW1_TOG_PORT_PIN 0 
#define FSW2_TOG_PORT_PIN 1

#define FSW1_ADDR      (&((FSW1_PORT)->IDR))
#define FSW2_ADDR      (&((FSW2_PORT)->IDR)) 

#define SW1_TOP_ADDR   (&((SW1_TOP_PORT)->IDR))
#define SW1_BTM_ADDR   (&((SW1_BTM_PORT)->IDR))
#define SW2_TOP_ADDR   (&((SW2_TOP_PORT)->IDR))
#define SW2_BTM_ADDR   (&((SW2_BTM_PORT)->IDR))
#define SW3_TOP_ADDR   (&((SW3_TOP_PORT)->IDR))   
#define SW3_BTM_ADDR   (&((SW3_BTM_PORT)->IDR))   
#define SW4_TOP_ADDR   (&((SW4_TOP_PORT)->IDR))   
#define SW4_BTM_ADDR   (&((SW4_BTM_PORT)->IDR))   
#define SW5_TOP_ADDR   (&((SW5_TOP_PORT)->IDR))   
#define SW5_BTM_ADDR   (&((SW5_BTM_PORT)->IDR))   
#define SW6_TOP_ADDR   (&((SW6_TOP_PORT)->IDR))   
#define SW6_BTM_ADDR   (&((SW6_BTM_PORT)->IDR))   
#define SW7_TOP_ADDR   (&((SW7_TOP_PORT)->IDR))   
#define SW7_BTM_ADDR   (&((SW7_BTM_PORT)->IDR))   
#define SW8_TOP_ADDR   (&((SW8_TOP_PORT)->IDR))   
#define SW8_BTM_ADDR   (&((SW8_BTM_PORT)->IDR))   

#define FSW1_TOG_ADDR  (&fsw_toggle_states) 
#define FSW2_TOG_ADDR  (&fsw_toggle_states)

#define FSW1_EXTICR    SYSCFG->EXTICR2 
#define FSW2_EXTICR    SYSCFG->EXTICR2 

#define FSW1_EXTI_PIN  SYSCFG_EXTICR2_EXTI4
#define FSW2_EXTI_PIN  SYSCFG_EXTICR2_EXTI6

#define FSW1_EXTI_PIN_PORT SYSCFG_EXTICR2_EXTI4_PB
#define FSW2_EXTI_PIN_PORT SYSCFG_EXTICR2_EXTI4_PE

#define FSW1_IRQ_HANDLER    EXTI4_IRQHandler 
#define FSW2_IRQ_HANDLER    EXTI6_IRQHandler 

#define FSW1_IRQ_N    EXTI4_IRQn 
#define FSW2_IRQ_N    EXTI6_IRQn 

#define FSW1_EXTI   EXTI4
#define FSW2_EXTI   EXTI6

extern volatile uint32_t fsw_toggle_states;

void switches_setup(void);
uint32_t fsw1_get_state(void);
uint32_t fsw2_get_state(void);
uint32_t sw1_top_get_state(void);
uint32_t sw1_btm_get_state(void);
uint32_t sw2_top_get_state(void);
uint32_t sw2_btm_get_state(void);
uint32_t sw3_top_get_state(void);
uint32_t sw3_btm_get_state(void);
uint32_t sw4_top_get_state(void);
uint32_t sw4_btm_get_state(void);
uint32_t sw5_top_get_state(void);
uint32_t sw5_btm_get_state(void);
uint32_t sw6_top_get_state(void);
uint32_t sw6_btm_get_state(void);
uint32_t sw7_top_get_state(void);
uint32_t sw7_btm_get_state(void);
uint32_t sw8_top_get_state(void);
uint32_t sw8_btm_get_state(void);
void get_switch_states(uint32_t *states);

#endif /* SWITCHES_H */
