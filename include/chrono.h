/**
 * @file chrono.h
 * @brief SysTick-based timing helpers.
 *
 * Usage:
 *   Define CLK_DIV before including this header (default: 1000).
 *   Call SysTick_Config(SystemCoreClock / CLK_DIV) once in main() before using delay().
 *   With CLK_DIV = 1000 each tick is 1 ms, so delay(500) waits 500 ms.
 */

#ifndef ARM_KIT_CHRONO
#define ARM_KIT_CHRONO

#include "kit.h"

#include <stdlib.h>
#include <time.h>

/* Suppress semihosting calls (required by some toolchains when using time.h). */
#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6000000)
  __asm(".global __use_no_semihosting\n\t");
#elif defined(__CC_ARM)
  #pragma import(__use_no_semihosting_swi)
#endif

#ifndef CLK_DIV
  #define CLK_DIV 1000
#endif

volatile uint32_t Ticks;

void SysTick_Handler(void) { Ticks++; }

/**
 * @brief Wait for the given number of SysTick ticks.
 */
void delay(uint32_t value) {
  uint32_t start = Ticks;
  while ((Ticks - start) < value) { /* wait */ }
}

/**
 * @brief Busy-loop delay (no SysTick required).
 * @param value Loop count (0-65535).
 */
void cekej(uint16_t value) {
  uint16_t i, j;
  for (i = 0; i < value; i++) {
    for (j = 0; j < value; j++) { /* NOP */ }
  }
}

/* time() replacement – returns SysTick count since startup. */
time_t time(time_t *t) { return Ticks; }

void _sys_exit(int returncode) { while (1) { /* NOP */ } }

#endif  /* ARM_KIT_CHRONO */
