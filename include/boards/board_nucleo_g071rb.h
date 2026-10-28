/**
 * @file board_nucleo_g071rb.h
 * @brief Pin map and on-board peripherals for NUCLEO-G071RB with the SPSE shield.
 */

#ifndef ARM_KIT_BOARD_NUCLEO_G071RB
#define ARM_KIT_BOARD_NUCLEO_G071RB

#define KIT_NAME  "NUCLEO-G071RB"

/* G071 GPIO clock register is IOPENR, not AHB1ENR */
#define KIT_GPIO_CLK_EN(port)  (RCC->IOPENR |= (1UL << ((((uint32_t)(port)) - IOPORT_BASE) >> 10)))

/* Arduino connector -> MCU pins */
#define ARD_D0_PORT   GPIOC   /* PC5 */
#define ARD_D0_PIN    5
#define ARD_D1_PORT   GPIOC   /* PC4 */
#define ARD_D1_PIN    4
#define ARD_D2_PORT   GPIOA   /* PA10 */
#define ARD_D2_PIN    10
#define ARD_D3_PORT   GPIOB   /* PB3 */
#define ARD_D3_PIN    3
#define ARD_D4_PORT   GPIOB   /* PB5 */
#define ARD_D4_PIN    5
#define ARD_D5_PORT   GPIOB   /* PB4 */
#define ARD_D5_PIN    4
#define ARD_D6_PORT   GPIOB   /* PB14 */
#define ARD_D6_PIN    14
#define ARD_D7_PORT   GPIOA   /* PA8  – also UCPD1_CC1; KPAD_setup() disables the pull-down */
#define ARD_D7_PIN    8
#define ARD_D8_PORT   GPIOA   /* PA9 */
#define ARD_D8_PIN    9
#define ARD_D9_PORT   GPIOC   /* PC7 */
#define ARD_D9_PIN    7
#define ARD_D10_PORT  GPIOB   /* PB0 */
#define ARD_D10_PIN   0
#define ARD_D11_PORT  GPIOA   /* PA7 */
#define ARD_D11_PIN   7
#define ARD_D12_PORT  GPIOA   /* PA6 */
#define ARD_D12_PIN   6
#define ARD_D13_PORT  GPIOA   /* PA5 (shared with LD4) */
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
#define ARD_A3_PORT   GPIOB   /* PB1  ADC1_IN9 */
#define ARD_A3_PIN    1
#define ARD_A4_PORT   GPIOB   /* PB11 ADC1_IN15 */
#define ARD_A4_PIN    11
#define ARD_A5_PORT   GPIOB   /* PB12 ADC1_IN16 */
#define ARD_A5_PIN    12
#define ARD_A1_ADC_CH  1

/* On-board components */
#define BOARD_LED_PORT    GPIOA
#define BOARD_LED_PIN     5
#define BOARD_LED_ACTIVE  1
#define BOARD_BTN_PORT    GPIOC
#define BOARD_BTN_PIN     13
#define BOARD_BTN_ACTIVE  0   /* pressed = 0 */

/* I2C1 for LCD over I2C (PB8 = SCL, PB9 = SDA): see I2Cg.h */
#ifndef SDA_PIN
  #define SDA_PIN  9   /* PB9 */
#endif
#ifndef SCL_PIN
  #define SCL_PIN  8   /* PB8 */
#endif
#define I2C_AF  6UL    /* AF6 on G071 */

#include "shield_spse.h"

#endif  /* ARM_KIT_BOARD_NUCLEO_G071RB */
