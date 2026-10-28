/**
 * @file I2C.h
 * @brief I2C1 master driver for STM32F4 (F407, F411).
 *
 * For STM32G071 use I2Cg.h instead.
 */

#ifndef ARM_KIT_I2C
#define ARM_KIT_I2C

#if defined(ARM_KIT_I2C_BACKEND_F4) || defined(ARM_KIT_I2C_BACKEND_G)
  #error "Include only one I2C backend: I2C.h or I2Cg.h."
#endif
#define ARM_KIT_I2C_BACKEND_F4

#include "kit.h"
#if defined(STM32G071xx)
  #error "Use I2Cg.h for STM32G071."
#endif

#include "chrono.h"

/**
 * @brief Configure the GPIO pins used by I2C1 (SDA_PIN, SCL_PIN on port B).
 */
void I2C_GPIO_init(void) {
  RCC->AHB1ENR |= (1UL << 1);   /* port B clock */

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
 * @brief Initialize I2C1 at 100 kHz with a 16 MHz peripheral clock.
 */
void I2C_init(void) {
  RCC->APB1ENR |= (1UL << 21);   /* I2C1 clock */

  I2C1->CR1 |=  (1UL << 15);     /* reset */
  I2C1->CR1 &= ~(1UL << 15);

  I2C1->CR2   |=  (16UL << 0);   /* FREQ = 16 MHz */
  I2C1->OAR1  |=  (1UL << 14);   /* must be kept 1 */
  I2C1->CCR   |=  (0x50UL << 0); /* SCL 100 kHz */
  I2C1->TRISE |=  (17UL << 0);   /* rise time 1000 ns */

  I2C1->CR1   |=  (1UL << 0);    /* enable */
}

/** Generate a START condition. */
void I2C_start(void) {
  I2C1->CR1 |= (1UL << 10);   /* ACK */
  I2C1->CR1 |= (1UL << 8);    /* START */
  while (!(I2C1->SR1 & I2C_SR1_SB)) { /* wait */ }
}

/** Send the slave address. */
void I2C_send_addr(uint8_t addr) {
  uint8_t dummy;
  I2C1->DR = addr;
  while (!(I2C1->SR1 & (I2C_SR1_ADDR | I2C_SR1_AF))) { /* wait */ }
  dummy = (uint8_t)(I2C1->SR1 | I2C1->SR2);   /* clear ADDR */
  (void)dummy;
}

/** Write one byte. */
void I2C_write(uint8_t data) {
  while (!(I2C1->SR1 & I2C_SR1_TXE)) { /* wait */ }
  I2C1->DR = data;
  while (!(I2C1->SR1 & I2C_SR1_BTF)) { /* wait */ }
}

/** Generate a STOP condition. */
void I2C_stop(void) {
  I2C1->CR1 |= (1UL << 9);
}

/**
 * @brief Check whether a device responds at the given address.
 * @return 1 if acknowledged; 0 if not.
 */
uint8_t I2C_probe(uint8_t addr) {
  uint8_t ack;
  I2C_start();
  I2C1->DR = addr & 0xFE;
  while (!(I2C1->SR1 & (I2C_SR1_ADDR | I2C_SR1_AF))) { /* wait */ }
  ack = (I2C1->SR1 & I2C_SR1_ADDR) ? 1 : 0;
  (void)I2C1->SR2;
  I2C_stop();
  I2C1->SR1 &= ~I2C_SR1_AF;
  while (I2C1->SR2 & I2C_SR2_BUSY) { /* wait for STOP */ }
  return ack;
}

#endif  /* ARM_KIT_I2C */
