/**
 * @file KPAD.h
 * @brief Matrix keypad driver for STM32F4 (F407, F411).
 *
 * For STM32G071 use KPADg.h instead.
 *
 * Define before including:
 *   KPAD_ROWS × KPAD_COLS array  KPAD_KeyMap[KPAD_ROWS][KPAD_COLS]
 *   KPAD_COLS  (default 4; use 3 for a 3×4 keypad)
 *   KPAD_STEP  (default 250; debounce delay in ms)
 */

#ifndef ARM_KIT_KPAD
#define ARM_KIT_KPAD

#include "kit.h"
#if defined(STM32G071xx)
  #error "Use KPADg.h for STM32G071."
#endif

#include "chrono.h"

#ifndef KPAD_STEP
  #define KPAD_STEP 250
#endif

#ifndef KPAD_COLS
  #define KPAD_COLS 4
#endif

#if defined(STM32F407xx)
/* STM32F4-DISCOVERY: rows PD6-PD9, columns PD0-PD3 */

/**
 * @brief Scan the keypad and return the pressed key character, or 0 if none.
 */
uint8_t KPAD_getKey(void) {
  uint16_t readout;
  uint8_t i;

  delay(KPAD_STEP);

  for (i = 0; i < 4; i++) {
    GPIOD->ODR &= ~(0x0F << 6);
    GPIOD->ODR |= ((~(1UL << i) & 0x0F) << 6);   /* drive row i low */
    readout = GPIOD->IDR & 0x0F;
    switch (readout) {
      case 0x0E: return KPAD_KeyMap[i][0];
      case 0x0D: return KPAD_KeyMap[i][1];
      case 0x0B: return KPAD_KeyMap[i][2];
#if KPAD_COLS > 3
      case 0x07: return KPAD_KeyMap[i][3];
#endif
    }
  }
  return 0;
}

/**
 * @brief Configure the keypad GPIO pins.
 */
void KPAD_setup(void) {
  uint8_t i;
  RCC->AHB1ENR |= (1UL << 3);   /* port D clock */

  for (i = 6; i < 10; i++) {    /* rows: outputs */
    GPIOD->MODER   &= ~(3UL << 2*i);
    GPIOD->MODER   |=  (1UL << 2*i);
    GPIOD->OTYPER  &= ~(1UL << i);
    GPIOD->OSPEEDR &= ~(3UL << 2*i);
    GPIOD->OSPEEDR |=  (2UL << 2*i);
    GPIOD->PUPDR   &= ~(3UL << 2*i);
  }

  for (i = 0; i < 4; i++) {     /* columns: inputs with pull-up */
    GPIOD->MODER &= ~(3UL << 2*i);
    GPIOD->PUPDR &= ~(3UL << 2*i);
    GPIOD->PUPDR |=  (1UL << 2*i);
  }
}

#else
/* NUCLEO-F411RE with SPSE shield: rows PA0, PA4, PA5, PA6; columns PA7-PA10 */

static const uint8_t KPAD_rowPin[4] = { 6, 5, 4, 0 };     /* ROW0 - ROW3 (port A) */
static const uint8_t KPAD_colPin[4] = { 10, 9, 8, 7 };    /* COL0 - COL3 (port A) */

/**
 * @brief Scan the keypad and return the pressed key character, or 0 if none.
 */
uint8_t KPAD_getKey(void) {
  uint8_t i, j;
  volatile uint8_t d;

  delay(KPAD_STEP);

  for (i = 0; i < 4; i++) {
    GPIOA->BSRR = (1UL << (KPAD_rowPin[i] + 16));   /* row low */
    for (d = 0; d < 50; d++) { /* settle */ }

    for (j = 0; j < KPAD_COLS; j++) {
      if (!(GPIOA->IDR & (1UL << KPAD_colPin[j]))) {
        GPIOA->BSRR = (1UL << KPAD_rowPin[i]);       /* row high */
        return KPAD_KeyMap[i][j];
      }
    }

    GPIOA->BSRR = (1UL << KPAD_rowPin[i]);           /* row high */
  }
  return 0;
}

/**
 * @brief Configure the keypad GPIO pins.
 */
void KPAD_setup(void) {
  uint8_t i, p;
  RCC->AHB1ENR |= (1UL << 0);   /* port A clock */

  for (i = 0; i < 4; i++) {     /* rows: outputs, idle high */
    p = KPAD_rowPin[i];
    GPIOA->BSRR    =  (1UL << p);
    GPIOA->MODER  &= ~(3UL << 2*p);
    GPIOA->MODER  |=  (1UL << 2*p);
    GPIOA->OTYPER &= ~(1UL << p);
    GPIOA->PUPDR  &= ~(3UL << 2*p);
  }

  for (i = 0; i < 4; i++) {     /* columns: inputs with pull-up */
    p = KPAD_colPin[i];
    GPIOA->MODER &= ~(3UL << 2*p);
    GPIOA->PUPDR &= ~(3UL << 2*p);
    GPIOA->PUPDR |=  (1UL << 2*p);
  }
}

#endif  /* board selection */

#endif  /* ARM_KIT_KPAD */
