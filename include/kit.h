/**
 * @file kit.h
 * @brief Top-level include for the SPSE ARM kit.
 *
 * Include this file first in every program.
 * It pulls in the device header and the matching board pin map based on the
 * preprocessor symbol set in the project (STM32F411xE / STM32F407xx / STM32G071xx).
 */

#ifndef ARM_KIT_H
#define ARM_KIT_H

#if defined(STM32F411xE) || defined(STM32F407xx)
  #include "stm32f4xx.h"
#elif defined(STM32G071xx)
  #include "stm32g0xx.h"
#else
  #error "Unknown device. Define STM32F411xE, STM32F407xx, or STM32G071xx in the project settings."
#endif

#if defined(STM32F407xx)
  #include "boards/board_disco_f407vg.h"
#elif defined(STM32F411xE)
  #include "boards/board_nucleo_f411re.h"
#elif defined(STM32G071xx)
  #include "boards/board_nucleo_g071rb.h"
#endif

#endif  /* ARM_KIT_H */
