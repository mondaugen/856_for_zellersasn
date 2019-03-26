#include <stdint.h>
#include "i2s_lowlevel.h"
#include "stm32f4xx.h"

/* the address is 0x34 because the CE pin is pulled low (see i2s_lowlevel.c) */
#define WM8778_CODEC_ADDR  ((uint8_t)0x34)

void i2s_gpio_disable(void)
{
    RCC->AHB1ENR &= ~(RCC_AHB1ENR_GPIOAEN |  RCC_AHB1ENR_GPIOCEN);
    while ((RCC->AHB1ENR & (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN )));
}
    
void i2s_port_setup(uint32_t sr)
{
    /* Turn on GPIO clock for I2S3 pins */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN;
    /* Configure GPIO */
    /* Configure PA15 to Alternate Function */
    GPIOA->MODER &= ~(0x3 << 30);
    GPIOA->MODER |= (0x2 << 30);
    /* Configure PC7, PC10-12 to Alternate Function */
    GPIOC->MODER &= ~((0x3 << 20) | (0x3 << 22) | (0x3 << 24) | (0x3 << 14));
    GPIOC->MODER |= (0x2 << 20) | (0x2 << 22) | (0x2 << 24) | (0x2 << 14);
    /* Set pins to high speed */
    GPIOA->OSPEEDR |= (0x3 << 30);
    GPIOC->OSPEEDR |= (0x3 << 20) | (0x3 << 22) | (0x3 << 24) | (0x3 << 14);
    /* Pins have no-pull up nor pull-down */
    GPIOA->PUPDR &= ~(0x3 << 30);
    GPIOC->PUPDR &= ~((0x3 << 20) | (0x3 << 22) | (0x3 << 24) | (0x3 << 14));
    /* A15 Alternate function 6 */
    GPIOA->AFR[1] &= ~(0xf << 28);
    GPIOA->AFR[1] |= (0x6 << 28);
    /* C7 Alternate function 6 */
    GPIOC->AFR[0] &= ~(0xf << 28);
    GPIOC->AFR[0] |= (0x6 << 28);
    /* C10,12, Alternate function 6, C11 alternate function 5 */
    GPIOC->AFR[1] &= ~((0xf << 8) | (0xf << 12) | (0xf << 16));
    GPIOC->AFR[1] |= ((0x6 << 8) | (0x5 << 12) | (0x6 << 16));
}

void i2s_codec_start(void)
{
    /* this codec needs nothing to start */
}

void i2s_codec_correct_frame_error(void)
{
    /* needs nothing for this */
}

uint32_t codec_format_reg_addr(uint8_t reg_addr, uint16_t reg_val)
{
    return (reg_addr << 1) | ((reg_val >> 8) & (0x1));
}

void __attribute__((optimize("O0")))
codec_config_via_i2c(void) 
{
    codec_prog_reg_i2c(WM8778_CODEC_ADDR,0x17,0x0000);
    codec_prog_reg_i2c(WM8778_CODEC_ADDR,0xa,0x0002);
    codec_prog_reg_i2c(WM8778_CODEC_ADDR,0xb,0x0042);
    codec_prog_reg_i2c(WM8778_CODEC_ADDR,0x5,0x00ff);

#ifdef CODEC_ANALOG_DIGITAL_MIX 
    /*
    Mix analog and digital output, necessary for boards without separate
    buffering circuit
    */
 #if (!defined(AUDIO_HW_TEST_THROUGHPUT)) && (!defined(AUDIO_HW_TEST_WET_DRY_MIX))
    codec_prog_reg_i2c(WM8778_CODEC_ADDR,0x16,0x5);
 #endif  
#endif  
}
