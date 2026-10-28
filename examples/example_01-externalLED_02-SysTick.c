/**
 * @file example_01-externalLED_02-SysTick.c
 * @brief Blink an external LED using SysTick timing (delay).
 */

#include "kit.h"

#define CLK_DIV  1000   /* SysTick period = 1 ms */
#define STEP      500   /* blink interval in ms */

#if defined(STM32F407xx)
  #define LED_GPIO  GPIOE
  #define LED       12   /* PE12 */
#elif defined(STM32F411xE)
  #define LED_GPIO  GPIOB
  #define LED       0    /* D1 on the shield (PB0) */
#elif defined(STM32G071xx)
  #define LED_GPIO  GPIOB
  #define LED       1    /* D1 on the shield (PB1) */
#endif

#include "chrono.h"

volatile uint8_t out;

int main(void) {
  SystemCoreClockUpdate();
  SysTick_Config(SystemCoreClock / CLK_DIV);

#if defined(STM32F407xx)
  RCC->AHB1ENR |= (1UL << 4);   /* port E clock */
#elif defined(STM32F411xE)
  RCC->AHB1ENR |= (1UL << 1);   /* port B clock */
#elif defined(STM32G071xx)
  RCC->IOPENR  |= (1UL << 1);   /* port B clock */
#endif

  LED_GPIO->MODER &= ~(3UL << LED*2);
  LED_GPIO->MODER |=  (1UL << LED*2);   /* output */

  while (1) {
    LED_GPIO->ODR |=  (1UL << LED);   out = 0xFF;
    delay(STEP);
    LED_GPIO->ODR &= ~(1UL << LED);   out = 0x00;
    delay(STEP);
  }

  return 0;
}
