#ifndef SWITCHES_H
#define SWITCHES_H 

FSW1_ENR        RCC_AHB1ENR_GPIOBEN
FSW2_ENR        RCC_AHB1ENR_GPIOEEN

SW1_UP_ENR      RCC_AHB1ENR_GPIOGEN
SW1_DN_ENR      RCC_AHB1ENR_GPIOGEN
SW2_UP_ENR      RCC_AHB1ENR_GPIODEN
SW2_DN_ENR      RCC_AHB1ENR_GPIODEN
SW3_UP_ENR      RCC_AHB1ENR_GPIODEN
SW3_DN_ENR      RCC_AHB1ENR_GPIOBEN
SW4_UP_ENR      RCC_AHB1ENR_GPIOBEN
SW4_DN_ENR      RCC_AHB1ENR_GPIOCEN
SW5_UP_ENR      RCC_AHB1ENR_GPIOCEN
SW5_DN_ENR      RCC_AHB1ENR_GPIODEN
SW6_UP_ENR      RCC_AHB1ENR_GPIODEN
SW6_DN_ENR      RCC_AHB1ENR_GPIODEN
SW7_UP_ENR      RCC_AHB1ENR_GPIODEN
SW7_DN_ENR      RCC_AHB1ENR_GPIOBEN
SW8_UP_ENR      RCC_AHB1ENR_GPIOBEN
SW8_DN_ENR      RCC_AHB1ENR_GPIOBEN

FSW1_PORT       GPIOB
FSW2_PORT       GPIOE

SW1_UP_PORT     GPIOG
SW1_DN_PORT     GPIOG
SW2_UP_PORT     GPIOD
SW2_DN_PORT     GPIOD
SW3_UP_PORT     GPIOD
SW3_DN_PORT     GPIOB
SW4_UP_PORT     GPIOB
SW4_DN_PORT     GPIOC
SW5_UP_PORT     GPIOC
SW5_DN_PORT     GPIOD
SW6_UP_PORT     GPIOD
SW6_DN_PORT     GPIOD
SW7_UP_PORT     GPIOD
SW7_DN_PORT     GPIOB
SW8_UP_PORT     GPIOB
SW8_DN_PORT     GPIOB

FSW1_PORT_PIN   4
FSW2_PORT_PIN   6

SW1_UP_PORT_PIN 2
SW1_DN_PORT_PIN 3
SW2_UP_PORT_PIN 12
SW2_DN_PORT_PIN 13
SW3_UP_PORT_PIN 11 
SW3_DN_PORT_PIN 0
SW4_UP_PORT_PIN 1
SW4_DN_PORT_PIN 6
SW5_UP_PORT_PIN 8
SW5_DN_PORT_PIN 2
SW6_UP_PORT_PIN 3
SW6_DN_PORT_PIN 4
SW7_UP_PORT_PIN 7
SW7_DN_PORT_PIN 7
SW8_UP_PORT_PIN 9
SW8_DN_PORT_PIN 8

#endif /* SWITCHES_H */