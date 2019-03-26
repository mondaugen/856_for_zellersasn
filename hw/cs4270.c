#include <stdint.h>
#include "i2s_lowlevel.h"
#include "stm32f4xx.h"

#define CS4270_CODEC_ADDR  ((uint8_t)0x90) 

static uint16_t all_codec_reg[8];

void i2s_gpio_disable(void)
{
    RCC->AHB1ENR &= ~(RCC_AHB1ENR_GPIOBEN);
    while ((RCC->AHB1ENR & (RCC_AHB1ENR_GPIOBEN)));
}

void i2s_port_setup(uint32_t sr)
{
    /* Turn on GPIO clock for I2S3 pins */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN;
    /* Enable port E clock */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
    /* Configure GPIO */
    /* Configure PA15 to Alternate Function */
    GPIOA->MODER &= ~(0x3 << 30);
    GPIOA->MODER |= (0x2 << 30);
    /* Configure PC7, PC10-12 to Alternate Function */
    GPIOC->MODER &= ~((0x3 << 20) | (0x3 << 22) | (0x3 << 24) | (0x3 << 14));
    GPIOC->MODER |= (0x2 << 20) | (0x2 << 22) | (0x2 << 24) | (0x2 << 14);
    /* Configure PC1, PE 2,3,4 to output */
    GPIOE->MODER &= ~((0x3 << 4) | (0x3 << 6) | (0x3 << 8) | (0x3 << 10));
    GPIOE->MODER |= ((0x1 << 4) | (0x1 << 6) | (0x1 << 8) | (0x1 << 10));
    /* Set pins to high speed */
    GPIOA->OSPEEDR |= (0x3 << 30);
    GPIOC->OSPEEDR |= (0x3 << 20) | (0x3 << 22) | (0x3 << 24) | (0x3 << 14);
    GPIOE->OSPEEDR |= ((0x3 << 4) | (0x3 << 6) | (0x3 << 8) | (0x3 << 10));
    /* Pins have no-pull up nor pull-down */
    GPIOA->PUPDR &= ~(0x3 << 30);
    GPIOC->PUPDR &= ~((0x3 << 20) | (0x3 << 22) | (0x3 << 24) | (0x3 << 14));
    GPIOE->PUPDR &= ~((0x3 << 4) | (0x3 << 6) | (0x3 << 8) | (0x3 << 10));
    /* A15 Alternate function 6 */
    GPIOA->AFR[1] &= ~(0xf << 28);
    GPIOA->AFR[1] |= (0x6 << 28);
    /* C7 Alternate function 6 */
    GPIOC->AFR[0] &= ~(0xf << 28);
    GPIOC->AFR[0] |= (0x6 << 28);
    /* C10,12, Alternate function 6, C11 alternate function 5 */
    GPIOC->AFR[1] &= ~((0xf << 8) | (0xf << 12) | (0xf << 16));
    GPIOC->AFR[1] |= ((0x6 << 8) | (0x5 << 12) | (0x6 << 16));
/* Assert RESET by resetting pin PE5 which will be set once
 * codec is ready to be programmed
 */
    GPIOE->ODR &= ~(0x1 << 5);
    /* Reset AD0-2 pins */
    GPIOE->ODR &= ~((0x1 << 2) | (0x1 << 3) | (0x1 << 4));
}

void i2s_codec_start(void)
{
    /* Reset DAC, ADC power down bits to start sound */
    codec_prog_reg_i2c(CS4270_CODEC_ADDR,0x02,0x00);
    uint16_t reg=0xff;
    codec_read_reg_i2c(CS4270_CODEC_ADDR,0x02,&reg);
    while (reg != 0x00);
    uint8_t n;
    for (n=1;n<=8;n++) {
        codec_read_reg_i2c(CS4270_CODEC_ADDR,n,&all_codec_reg[n-1]);
    }
}

void i2s_codec_correct_frame_error(void)
{
    /* Assert reset by pulling pin low */
    GPIOE->ODR &= ~(0x1 << 5);
}

uint32_t codec_format_reg_addr(uint8_t reg_addr, uint16_t reg_val)
{
    return reg_addr;
}

void __attribute__((optimize("O0")))
codec_config_via_i2c(void) 
{
    /* Set ADC, DAC to I2S 16-bit */
    /* Chip reset should be asserted, deassert to allow programming */
    GPIOE->ODR |= (0x1 << 5);
    ///* Set power down bit to confirm software control */
    //codec_prog_reg_i2c(CS4270_CODEC_ADDR,0x02,0x01);
    ///* Wait */
    //int j = 1000000;
    //while (j--);
    uint16_t reg;
    /* Reset power down bit, power down ADC, DAC */
    codec_prog_reg_i2c(CS4270_CODEC_ADDR,0x02,0x23);
    /* Read register contents */
    codec_read_reg_i2c(CS4270_CODEC_ADDR,0x01,&reg);
    /* Check correct value */
    while ((reg & 0xf0) != 0xc0);
    /* Set pop-suppression, slave mode */
    codec_prog_reg_i2c(CS4270_CODEC_ADDR,0x03,0x31);
    /* Set DAC, ADC to I2S mode */
#if defined(CODEC_DIGITAL_LOOPBACK)
    codec_prog_reg_i2c(CS4270_CODEC_ADDR,0x04,0x29);
#else
    codec_prog_reg_i2c(CS4270_CODEC_ADDR,0x04,0x09);
#endif
    /* Set single DAC volume */
    codec_prog_reg_i2c(CS4270_CODEC_ADDR,0x05,0x80);
    /* Set DAC volume to 0dB (no attenuation) */
    codec_prog_reg_i2c(CS4270_CODEC_ADDR,0x07,0x00);
    /* Read register contents */
    /* Check correct values */
    reg=0;codec_read_reg_i2c(CS4270_CODEC_ADDR,0x02,&reg);
    while (reg != 0x23);
    reg=0;codec_read_reg_i2c(CS4270_CODEC_ADDR,0x03,&reg);
    while (reg != 0x31);
    reg=0;codec_read_reg_i2c(CS4270_CODEC_ADDR,0x04,&reg);
#if defined(CODEC_DIGITAL_LOOPBACK)
    while (reg != 0x29);
#else
    while (reg != 0x09);
#endif
    reg=0;codec_read_reg_i2c(CS4270_CODEC_ADDR,0x05,&reg);
    while (reg != 0x80);
    reg=1;codec_read_reg_i2c(CS4270_CODEC_ADDR,0x07,&reg);
    while (reg != 0x00);

}
