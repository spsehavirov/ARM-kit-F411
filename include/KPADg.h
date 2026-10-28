/**
 * @file KPADg.h
 * @brief Matrix keypad driver for STM32G071.
 *
 * For STM32F4 (F407, F411) use KPAD.h instead.
 *
 * Define before including:
 *   KPAD_ROWS × KPAD_COLS array  KPAD_KeyMap[KPAD_ROWS][KPAD_COLS]
 *   KPAD_COLS  (default 4)
 *   KPAD_STEP  (default 250; debounce delay in ms)
 *
 * Shield wiring: rows PA0, PA4, PA5, PA6; columns PA7-PA10.
 * PA8 (COL1) is also the UCPD1 CC1 pin; KPAD_setup() disables the 5.1 kOhm
 * "dead-battery" pull-down so the input reads correctly.
 */

#ifndef ARM_KIT_KPAD_G
#define ARM_KIT_KPAD_G

#include "kit.h"
#if !defined(STM32G071xx)
  #error "KPADg.h is for STM32G071."
#endif

#include "chrono.h"

#ifndef KPAD_STEP
  #define KPAD_STEP 250
#endif

#ifndef KPAD_COLS
  #define KPAD_COLS 4
#endif

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

    GPIOA->BSRR = (1UL << KPAD_rowPin[i]);
  }
  return 0;
}

/**
 * @brief Configure the keypad GPIO pins.
 */
void KPAD_setup(void) {
  uint8_t i, p;

  RCC->IOPENR |= (1UL << 0);   /* port A clock */

  /* Disable UCPD "dead-battery" pull-downs on PA8 (COL1) and PB15. */
  RCC->APBENR2  |= RCC_APBENR2_SYSCFGEN;
  SYSCFG->CFGR1 |= SYSCFG_CFGR1_UCPD1_STROBE | SYSCFG_CFGR1_UCPD2_STROBE;

  for (i = 0; i < 4; i++) {   /* rows: outputs, idle high */
    p = KPAD_rowPin[i];
    GPIOA->BSRR    =  (1UL << p);
    GPIOA->MODER  &= ~(3UL << 2*p);
    GPIOA->MODER  |=  (1UL << 2*p);
    GPIOA->OTYPER &= ~(1UL << p);
    GPIOA->PUPDR  &= ~(3UL << 2*p);
  }

  for (i = 0; i < 4; i++) {   /* columns: inputs with pull-up (G071 resets to analog!) */
    p = KPAD_colPin[i];
    GPIOA->MODER &= ~(3UL << 2*p);   /* input mode */
    GPIOA->PUPDR &= ~(3UL << 2*p);
    GPIOA->PUPDR |=  (1UL << 2*p);   /* pull-up */
  }
}

#endif  /* ARM_KIT_KPAD_G */
