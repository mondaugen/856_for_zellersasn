/* Copyright (c) 2016 Nicholas Esterer. All rights reserved. */

#ifndef SWITCHES_H
#define SWITCHES_H 

#include <stdint.h> 
#include "stm32f4xx.h"

#if ((!defined(BOARD_V1)) & (!defined(BOARD_V2)))
 #error("Please define board version.")
#endif  

#define NUM_SWITCHES            19

#define FSW1_ENR                RCC_AHB1ENR_GPIOBEN
#define FSW2_ENR                RCC_AHB1ENR_GPIOEEN

#define SW1_TOP_ENR             RCC_AHB1ENR_GPIOGEN
#define SW1_BTM_ENR             RCC_AHB1ENR_GPIOGEN
#define SW2_TOP_ENR             RCC_AHB1ENR_GPIODEN
#define SW2_BTM_ENR             RCC_AHB1ENR_GPIODEN
#define SW3_TOP_ENR             RCC_AHB1ENR_GPIODEN
#define SW3_BTM_ENR             RCC_AHB1ENR_GPIOBEN
#define SW4_TOP_ENR             RCC_AHB1ENR_GPIOBEN
#define SW4_BTM_ENR             RCC_AHB1ENR_GPIOBEN
#define SW5_TOP_ENR             RCC_AHB1ENR_GPIOBEN
#define SW5_BTM_ENR             RCC_AHB1ENR_GPIODEN
#define SW6_TOP_ENR             RCC_AHB1ENR_GPIODEN
#define SW6_BTM_ENR             RCC_AHB1ENR_GPIODEN
#define SW7_TOP_ENR             RCC_AHB1ENR_GPIODEN
#define SW7_BTM_ENR             RCC_AHB1ENR_GPIOCEN
#define SW8_TOP_ENR             RCC_AHB1ENR_GPIOBEN
#define SW8_BTM_ENR             RCC_AHB1ENR_GPIOCEN
#if defined(BOARD_V2)
#define EXPSW_ENR               RCC_AHB1ENR_GPIOFEN
#endif /* defined(BOARD_V2) */

#define FSW1_PORT               GPIOB
#define FSW2_PORT               GPIOE

#define SW1_TOP_PORT            GPIOG
#define SW1_BTM_PORT            GPIOG
#define SW2_TOP_PORT            GPIOD
#define SW2_BTM_PORT            GPIOD
#define SW3_TOP_PORT            GPIOD
#define SW3_BTM_PORT            GPIOB
#define SW4_TOP_PORT            GPIOB
#define SW4_BTM_PORT            GPIOB
#define SW5_TOP_PORT            GPIOB
#define SW5_BTM_PORT            GPIOD
#define SW6_TOP_PORT            GPIOD
#define SW6_BTM_PORT            GPIOD
#define SW7_TOP_PORT            GPIOD
#define SW7_BTM_PORT            GPIOC
#define SW8_TOP_PORT            GPIOB
#define SW8_BTM_PORT            GPIOC
#if defined(BOARD_V2)
#define EXPSW_PORT              GPIOF
#endif /* defined(BOARD_V2) */

#define FSW1_PORT_PIN           4
#define FSW2_PORT_PIN           6

#if defined(BOARD_V1)
 #define SW1_TOP_PORT_PIN        2
#elif defined(BOARD_V2)
 #define SW1_TOP_PORT_PIN        7
#endif  
#define SW1_BTM_PORT_PIN        3
#define SW2_TOP_PORT_PIN        12
#define SW2_BTM_PORT_PIN        13
#define SW3_TOP_PORT_PIN        11 
#define SW3_BTM_PORT_PIN        0
#define SW4_TOP_PORT_PIN        1
#define SW4_BTM_PORT_PIN        7
#define SW5_TOP_PORT_PIN        8
#define SW5_BTM_PORT_PIN        7
#define SW6_TOP_PORT_PIN        4
#define SW6_BTM_PORT_PIN        2
#define SW7_TOP_PORT_PIN        3
#define SW7_BTM_PORT_PIN        8
#define SW8_TOP_PORT_PIN        9
#define SW8_BTM_PORT_PIN        6
#if defined(BOARD_V2)
#define EXPSW_PORT_PIN          8 
#endif /* defined(BOARD_V2) */

#define MSW3_TOP_PORT_PIN       SW3_TOP_PORT_PIN
#define MSW3_BTM_PORT_PIN       SW3_BTM_PORT_PIN
#define MSW7_TOP_PORT_PIN       SW7_TOP_PORT_PIN
#define MSW1_TOP_PORT_PIN       SW1_TOP_PORT_PIN

#define FSW1_TOG_PORT_PIN       0 
#define FSW2_TOG_PORT_PIN       1
#define MSW3_TOP_TOG_PORT_PIN   2
#define MSW3_BTM_TOG_PORT_PIN   3 
#define MSW7_TOP_TOG_PORT_PIN   4
#define MSW1_TOP_TOG_PORT_PIN   5

#define FSW1_ADDR               (&((FSW1_PORT)->IDR))
#define FSW2_ADDR               (&((FSW2_PORT)->IDR)) 

#define SW1_TOP_ADDR            (&((SW1_TOP_PORT)->IDR))
#define SW1_BTM_ADDR            (&((SW1_BTM_PORT)->IDR))
#define SW2_TOP_ADDR            (&((SW2_TOP_PORT)->IDR))
#define SW2_BTM_ADDR            (&((SW2_BTM_PORT)->IDR))
#define SW3_TOP_ADDR            (&((SW3_TOP_PORT)->IDR))   
#define SW3_BTM_ADDR            (&((SW3_BTM_PORT)->IDR))   
#define SW4_TOP_ADDR            (&((SW4_TOP_PORT)->IDR))   
#define SW4_BTM_ADDR            (&((SW4_BTM_PORT)->IDR))   
#define SW5_TOP_ADDR            (&((SW5_TOP_PORT)->IDR))   
#define SW5_BTM_ADDR            (&((SW5_BTM_PORT)->IDR))   
#define SW6_TOP_ADDR            (&((SW6_TOP_PORT)->IDR))   
#define SW6_BTM_ADDR            (&((SW6_BTM_PORT)->IDR))   
#define SW7_TOP_ADDR            (&((SW7_TOP_PORT)->IDR))   
#define SW7_BTM_ADDR            (&((SW7_BTM_PORT)->IDR))   
#define SW8_TOP_ADDR            (&((SW8_TOP_PORT)->IDR))   
#define SW8_BTM_ADDR            (&((SW8_BTM_PORT)->IDR))   
#if defined(BOARD_V2)
#define EXPSW_ADDR              (&((EXPSW_PORT)->IDR))   
#endif /* defined(BOARD_V2) */

#define MSW3_TOP_ADDR           SW3_TOP_ADDR 
#define MSW3_BTM_ADDR           SW3_BTM_ADDR 
#define MSW7_TOP_ADDR           SW7_TOP_ADDR 
#define MSW1_TOP_ADDR           SW1_TOP_ADDR 

#define FSW1_TOG_ADDR           (&sw_toggle_states) 
#define FSW2_TOG_ADDR           (&sw_toggle_states)
#define MSW3_TOP_TOG_ADDR       (&sw_toggle_states)
#define MSW3_BTM_TOG_ADDR       (&sw_toggle_states) 
#define MSW7_TOP_TOG_ADDR       (&sw_toggle_states)
#define MSW1_TOP_TOG_ADDR       (&sw_toggle_states)

#define FSW1_EXTICR             SYSCFG->EXTICR[1] 
#define FSW2_EXTICR             SYSCFG->EXTICR[1] 
#define MSW3_TOP_EXTICR         SYSCFG->EXTICR[2] 
#define MSW3_BTM_EXTICR         SYSCFG->EXTICR[0] 
#define MSW7_TOP_EXTICR         SYSCFG->EXTICR[0] 
#if defined(BOARD_V1)
 #define MSW1_TOP_EXTICR         SYSCFG->EXTICR[0] 
#elif defined(BOARD_V2)
 #define MSW1_TOP_EXTICR         SYSCFG->EXTICR[1] 
#endif  

#define FSW1_EXTI_PIN           SYSCFG_EXTICR2_EXTI4
#define FSW2_EXTI_PIN           SYSCFG_EXTICR2_EXTI6
#define MSW3_TOP_EXTI_PIN       SYSCFG_EXTICR3_EXTI11
#define MSW3_BTM_EXTI_PIN       SYSCFG_EXTICR1_EXTI0
#define MSW7_TOP_EXTI_PIN       SYSCFG_EXTICR1_EXTI3
#if defined(BOARD_V1)
 #define MSW1_TOP_EXTI_PIN       SYSCFG_EXTICR1_EXTI2
#elif defined(BOARD_V2)
 #define MSW1_TOP_EXTI_PIN       SYSCFG_EXTICR2_EXTI7
#endif  

#define FSW1_EXTI_PIN_PORT      SYSCFG_EXTICR2_EXTI4_PB
#define FSW2_EXTI_PIN_PORT      SYSCFG_EXTICR2_EXTI6_PE
#define MSW3_TOP_EXTI_PIN_PORT  SYSCFG_EXTICR3_EXTI11_PD
#define MSW3_BTM_EXTI_PIN_PORT  SYSCFG_EXTICR1_EXTI0_PB
#define MSW7_TOP_EXTI_PIN_PORT  SYSCFG_EXTICR1_EXTI3_PD
#if defined(BOARD_V1)
 #define MSW1_TOP_EXTI_PIN_PORT  SYSCFG_EXTICR1_EXTI2_PG
#elif defined(BOARD_V2)
 #define MSW1_TOP_EXTI_PIN_PORT  SYSCFG_EXTICR2_EXTI7_PG
#endif  

#define FSW1_IRQ_HANDLER        EXTI4_IRQHandler 
#define MSW3_TOP_IRQ_HANDLER    EXTI15_10_IRQHandler 
#define MSW3_BTM_IRQ_HANDLER    EXTI0_IRQHandler 
#define MSW7_TOP_IRQ_HANDLER    EXTI3_IRQHandler 
#if defined(BOARD_V1)
 #define FSW2_IRQ_HANDLER        EXTI9_5_IRQHandler 
 #define MSW1_TOP_IRQ_HANDLER    EXTI2_IRQHandler 
#elif defined(BOARD_V2)
 #define MSW1_TOP_FSW2_IRQ_HANDLER EXTI9_5_IRQHandler 
#endif  

#define FSW1_IRQ_N              EXTI4_IRQn 
#define MSW3_TOP_IRQ_N          EXTI15_10_IRQn 
#define MSW3_BTM_IRQ_N          EXTI0_IRQn 
#define MSW7_TOP_IRQ_N          EXTI3_IRQn 
#if defined(BOARD_V1)
 #define MSW1_TOP_IRQ_N         EXTI2_IRQn 
 #define FSW2_IRQ_N             EXTI9_5_IRQn 
#elif defined(BOARD_V2)
 #define MSW1_TOP_FSW2_IRQ_N    EXTI9_5_IRQn 
#endif  

#define FSW1_EXTI               EXTI
#define MSW3_TOP_EXTI           EXTI
#define MSW3_BTM_EXTI           EXTI
#define MSW7_TOP_EXTI           EXTI
#if defined(BOARD_V1) 
 #define FSW2_EXTI              EXTI
 #define MSW1_TOP_EXTI          EXTI
#elif defined(BOARD_V2)
 #define MSW1_TOP_FSW2_EXTI     EXTI
#endif  

extern volatile uint32_t sw_toggle_states;

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
uint32_t expsw_get_state(void);
void get_switch_states(uint32_t *states);
void reset_sw_toggle_states(void);

#endif /* SWITCHES_H */
