/**
 * @file board_nucleo_f411re.h
 * @brief Pin map and on-board peripherals for NUCLEO-F411RE with the SPSE shield.
 */

#ifndef ARM_KIT_BOARD_NUCLEO_F411RE
#define ARM_KIT_BOARD_NUCLEO_F411RE

#define KIT_NAME  "NUCLEO-F411RE"

#define KIT_GPIO_CLK_EN(port)  (RCC->AHB1ENR |= (1UL << ((((uint32_t)(port)) - GPIOA_BASE) >> 10)))

/* Arduino connector -> MCU pins */
#define ARD_D0_PORT   GPIOA   /* PA3  (SB62 open by default) */
#define ARD_D0_PIN    3
#define ARD_D1_PORT   GPIOA   /* PA2  (SB63 open by default) */
#define ARD_D1_PIN    2
#define ARD_D2_PORT   GPIOA   /* PA10 */
#define ARD_D2_PIN    10
#define ARD_D3_PORT   GPIOB   /* PB3 */
#define ARD_D3_PIN    3
#define ARD_D4_PORT   GPIOB   /* PB5 */
#define ARD_D4_PIN    5
#define ARD_D5_PORT   GPIOB   /* PB4 */
#define ARD_D5_PIN    4
#define ARD_D6_PORT   GPIOB   /* PB10 */
#define ARD_D6_PIN    10
#define ARD_D7_PORT   GPIOA   /* PA8 */
#define ARD_D7_PIN    8
#define ARD_D8_PORT   GPIOA   /* PA9 */
#define ARD_D8_PIN    9
#define ARD_D9_PORT   GPIOC   /* PC7 */
#define ARD_D9_PIN    7
#define ARD_D10_PORT  GPIOB   /* PB6 */
#define ARD_D10_PIN   6
#define ARD_D11_PORT  GPIOA   /* PA7 */
#define ARD_D11_PIN   7
#define ARD_D12_PORT  GPIOA   /* PA6 */
#define ARD_D12_PIN   6
#define ARD_D13_PORT  GPIOA   /* PA5 (shared with LD2) */
#define ARD_D13_PIN   5
#define ARD_D14_PORT  GPIOB   /* PB9  I2C1_SDA */
#define ARD_D14_PIN   9
#define ARD_D15_PORT  GPIOB   /* PB8  I2C1_SCL */
#define ARD_D15_PIN   8
#define ARD_A0_PORT   GPIOA   /* PA0  ADC1_IN0 */
#define ARD_A0_PIN    0
#define ARD_A1_PORT   GPIOA   /* PA1  ADC1_IN1 */
#define ARD_A1_PIN    1
#define ARD_A2_PORT   GPIOA   /* PA4  ADC1_IN4 */
#define ARD_A2_PIN    4
#define ARD_A3_PORT   GPIOB   /* PB0  ADC1_IN8 */
#define ARD_A3_PIN    0
#define ARD_A4_PORT   GPIOC   /* PC1  ADC1_IN11 */
#define ARD_A4_PIN    1
#define ARD_A5_PORT   GPIOC   /* PC0  ADC1_IN10 */
#define ARD_A5_PIN    0
#define ARD_A1_ADC_CH  1

/* On-board components */
#define BOARD_LED_PORT    GPIOA
#define BOARD_LED_PIN     5
#define BOARD_LED_ACTIVE  1
#define BOARD_BTN_PORT    GPIOC
#define BOARD_BTN_PIN     13
#define BOARD_BTN_ACTIVE  0   /* pressed = 0 */

/* I2C1 for LCD over I2C (PB8 = SCL, PB9 = SDA): see I2C.h */
#ifndef SDA_PIN
  #define SDA_PIN  9   /* PB9 */
#endif
#ifndef SCL_PIN
  #define SCL_PIN  8   /* PB8 */
#endif
#define I2C_AF  4UL    /* AF4 */

#include "shield_spse.h"

#endif  /* ARM_KIT_BOARD_NUCLEO_F411RE */
