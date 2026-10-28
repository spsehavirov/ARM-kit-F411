/**
 * @file example_02-userButton_01-controlLED.c
 * @brief Turn LEDs on or off based on the user button state.
 *
 * All kit LEDs are stored in an array of (port, pin) pairs so they can be
 * controlled with a simple loop instead of repeating the same register
 * operations for each LED individually.
 *
 * Button held: all LEDs on.
 * Button released: all LEDs off.
 */

#include "kit.h"

/* ---- LED table ---- */

#if defined(STM32F407xx)
/* STM32F4-DISCOVERY: PE12-PE15, active-low */
#define LED_COUNT   4
#define LED_ACTIVE  0

static GPIO_TypeDef * const ledPort[LED_COUNT] = { GPIOE, GPIOE, GPIOE, GPIOE };
static const uint8_t         ledPin [LED_COUNT] = {   12,    13,    14,    15  };

static void ledsInit(void) {
  uint8_t i;
  RCC->AHB1ENR |= (1UL << 4);   /* port E clock */
  for (i = 0; i < LED_COUNT; i++) {
    ledPort[i]->MODER &= ~(3UL << ledPin[i]*2);
    ledPort[i]->MODER |=  (1UL << ledPin[i]*2);   /* output */
    ledPort[i]->OTYPER&= ~(1UL << ledPin[i]);      /* push-pull */
    ledPort[i]->BSRR   =  (1UL << ledPin[i]);      /* off (active-low: set = off) */
  }
}

#else
/* NUCLEO-F411RE + SPSE shield: D1-D8, active-high */
#define LED_COUNT   8
#define LED_ACTIVE  1

static GPIO_TypeDef * const ledPort[LED_COUNT] = {
  GPIOB, GPIOB, GPIOB, GPIOB, GPIOB, GPIOB, GPIOC, GPIOC
};
static const uint8_t ledPin[LED_COUNT] = { 0, 3, 4, 5, 6, 10, 0, 1 };

static void ledsInit(void) {
  uint8_t i, p;
  RCC->AHB1ENR |= (1UL << 1) | (1UL << 2);   /* port B + C clock */
  for (i = 0; i < LED_COUNT; i++) {
    p = ledPin[i];
    ledPort[i]->MODER &= ~(3UL << p*2);
    ledPort[i]->MODER |=  (1UL << p*2);       /* output */
    ledPort[i]->BSRR   =  (1UL << (p + 16)); /* off */
  }
}
#endif

static void ledOn(uint8_t i) {
#if LED_ACTIVE
  ledPort[i]->BSRR = (1UL << ledPin[i]);
#else
  ledPort[i]->BSRR = (1UL << (ledPin[i] + 16));
#endif
}

static void ledOff(uint8_t i) {
#if LED_ACTIVE
  ledPort[i]->BSRR = (1UL << (ledPin[i] + 16));
#else
  ledPort[i]->BSRR = (1UL << ledPin[i]);
#endif
}

/* ---- Button ---- */

#if defined(STM32F407xx)
  #define BTN_GPIO  GPIOA
  #define BTN_PIN   0
  #define BTN_ON    1   /* pressed = logic 1 */
#else
  #define BTN_GPIO  GPIOC
  #define BTN_PIN   7
  #define BTN_ON    0   /* pressed = logic 0 */
#endif

int main(void) {
  uint8_t i;

  ledsInit();

#if defined(STM32F407xx)
  RCC->AHB1ENR |= (1UL << 0);   /* port A clock */
#elif defined(STM32F411xE)
  RCC->AHB1ENR |= (1UL << 2);   /* port C clock */
#elif defined(STM32G071xx)
  RCC->IOPENR  |= (1UL << 2);   /* port C clock */
#endif
   BTN_GPIO->MODER &= ~(3UL << BTN_PIN*2);   /* input */
#if !defined(STM32F407xx)
  BTN_GPIO->PUPDR &= ~(3UL << BTN_PIN*2);
  BTN_GPIO->PUPDR |=  (1UL << BTN_PIN*2);   /* pull-up – S1 on PC7 switches to GND */
#endif

  while (1) {
    if (((BTN_GPIO->IDR >> BTN_PIN) & 1UL) == BTN_ON) {
      for (i = 0; i < LED_COUNT; i++) ledOn(i);
    } else {
      for (i = 0; i < LED_COUNT; i++) ledOff(i);
    }
  }

  return 0;
}
