/**
 * @file I2Cg.h
 * @brief I2C1 master driver for STM32G071.
 *
 * For STM32F4 (F407, F411) use I2C.h instead.
 * On the G071 each I2C_write() call issues its own START+address+data+STOP
 * because the peripheral's AUTOEND mode makes multi-byte framing tricky.
 * The LCD_I2Cg.h driver is written to match this behaviour.
 */

#ifndef ARM_KIT_I2C_G
#define ARM_KIT_I2C_G

#if defined(ARM_KIT_I2C_BACKEND_F4) || defined(ARM_KIT_I2C_BACKEND_G)
  #error "Include only one I2C backend: I2C.h or I2Cg.h."
#endif
#define ARM_KIT_I2C_BACKEND_G

#include "kit.h"
#if !defined(STM32G071xx)
  #error "I2Cg.h is for STM32G071."
#endif

#include "chrono.h"

static uint8_t I2C_addr;   /* address stored by I2C_send_addr() */

/**
 * @brief Configure the GPIO pins used by I2C1 (SDA_PIN, SCL_PIN on port B).
 */
void I2C_GPIO_init(void) {
  RCC->IOPENR |= (1UL << 1);   /* port B clock */

  GPIOB->MODER &= ~(3UL << (SDA_PIN*2));
  GPIOB->MODER |=  (2UL << (SDA_PIN*2));   /* alternate function */
  GPIOB->MODER &= ~(3UL << (SCL_PIN*2));
  GPIOB->MODER |=  (2UL << (SCL_PIN*2));

  GPIOB->PUPDR &= ~(3UL << (SDA_PIN*2));
  GPIOB->PUPDR |=  (1UL << (SDA_PIN*2));   /* pull-up */
  GPIOB->PUPDR &= ~(3UL << (SCL_PIN*2));
  GPIOB->PUPDR |=  (1UL << (SCL_PIN*2));

  GPIOB->OTYPER &= ~(1UL << SDA_PIN);
  GPIOB->OTYPER |=  (1UL << SDA_PIN);      /* open-drain */
  GPIOB->OTYPER &= ~(1UL << SCL_PIN);
  GPIOB->OTYPER |=  (1UL << SCL_PIN);

  GPIOB->OSPEEDR &= ~(3UL << (SDA_PIN*2));
  GPIOB->OSPEEDR |=  (2UL << (SDA_PIN*2)); /* high speed */
  GPIOB->OSPEEDR &= ~(3UL << (SCL_PIN*2));
  GPIOB->OSPEEDR |=  (2UL << (SCL_PIN*2));

  /* AFR[0] = pins 0-7, AFR[1] = pins 8-15; four bits per pin */
  GPIOB->AFR[SDA_PIN / 8] &= ~(0xFUL << ((SDA_PIN % 8) * 4));
  GPIOB->AFR[SDA_PIN / 8] |=  (I2C_AF  << ((SDA_PIN % 8) * 4));
  GPIOB->AFR[SCL_PIN / 8] &= ~(0xFUL << ((SCL_PIN % 8) * 4));
  GPIOB->AFR[SCL_PIN / 8] |=  (I2C_AF  << ((SCL_PIN % 8) * 4));
}

/**
 * @brief Initialize I2C1 at 100 kHz with a 16 MHz clock
 *        (PRESC=3, SCLDEL=4, SDADEL=2, SCLH=0x0F, SCLL=0x13).
 */
void I2C_init(void) {
  RCC->APBENR1  |= RCC_APBENR1_I2C1EN;
  RCC->APBRSTR1 |=  RCC_APBRSTR1_I2C1RST;
  RCC->APBRSTR1 &= ~RCC_APBRSTR1_I2C1RST;

  I2C1->CR1    &= ~I2C_CR1_PE;
  I2C1->TIMINGR = 0x30420F13;
  I2C1->CR1    |=  I2C_CR1_PE;
}

/** Wait until the bus is idle (START is issued per-byte by I2C_write). */
void I2C_start(void) {
  while (I2C1->ISR & I2C_ISR_BUSY) { /* wait */ }
}

/** Store the slave address used by subsequent I2C_write() calls. */
void I2C_send_addr(uint8_t addr) {
  I2C_addr = addr;
}

/**
 * @brief Send one byte as a complete START+address+data+STOP transaction.
 */
void I2C_write(uint8_t data) {
  I2C1->ICR = I2C_ICR_NACKCF | I2C_ICR_STOPCF;
  I2C1->CR2 = (I2C_addr & 0xFE) | (1UL << I2C_CR2_NBYTES_Pos) | I2C_CR2_AUTOEND | I2C_CR2_START;

  while (!(I2C1->ISR & (I2C_ISR_TXIS | I2C_ISR_NACKF))) { /* wait */ }
  if (I2C1->ISR & I2C_ISR_TXIS) {
    I2C1->TXDR = data;
  }

  while (!(I2C1->ISR & I2C_ISR_STOPF)) { /* wait */ }
  I2C1->ICR = I2C_ICR_NACKCF | I2C_ICR_STOPCF;
}

/** No-op: AUTOEND already sent the STOP. */
void I2C_stop(void) { /* NOP */ }

/**
 * @brief Check whether a device responds at the given address.
 * @return 1 if acknowledged; 0 if not.
 */
uint8_t I2C_probe(uint8_t addr) {
  uint8_t ack;
  I2C_start();
  I2C1->ICR = I2C_ICR_NACKCF | I2C_ICR_STOPCF;
  I2C1->CR2 = (addr & 0xFE) | I2C_CR2_AUTOEND | I2C_CR2_START;   /* 0 bytes: address-only */

  while (!(I2C1->ISR & I2C_ISR_STOPF)) { /* wait */ }
  ack = (I2C1->ISR & I2C_ISR_NACKF) ? 0 : 1;
  I2C1->ICR = I2C_ICR_NACKCF | I2C_ICR_STOPCF;
  return ack;
}

#endif  /* ARM_KIT_I2C_G */
