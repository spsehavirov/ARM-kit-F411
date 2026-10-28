/**
 * @file board_disco_f407vg.h
 * @brief Pin map and on-board peripherals for STM32F4-DISCOVERY.
 */

#ifndef ARM_KIT_BOARD_DISCO_F407VG
#define ARM_KIT_BOARD_DISCO_F407VG

#define KIT_NAME  "STM32F4-DISCOVERY"

#define KIT_GPIO_CLK_EN(port)  (RCC->AHB1ENR |= (1UL << ((((uint32_t)(port)) - GPIOA_BASE) >> 10)))

/* On-board components */
#define BOARD_LED_PORT    GPIOD          /* PD12 green, PD13 orange, PD14 red, PD15 blue */
#define BOARD_LED_PIN     12
#define BOARD_LED_ACTIVE  1
#define BOARD_BTN_PORT    GPIOA
#define BOARD_BTN_PIN     0
#define BOARD_BTN_ACTIVE  1              /* pressed = 1 */

/* External LEDs on PE12-PE15 */
#define KIT_LED_COUNT   4
#define KIT_LED_ACTIVE  0                /* active-low: logic 0 = LED on */
#define LED0_PORT  GPIOE
#define LED0_PIN   12
#define LED1_PORT  GPIOE
#define LED1_PIN   13
#define LED2_PORT  GPIOE
#define LED2_PIN   14
#define LED3_PORT  GPIOE
#define LED3_PIN   15

/* Keypad: rows PD6-PD9, columns PD0-PD3 */
#define KPAD_ROW0_PORT  GPIOD
#define KPAD_ROW0_PIN   6
#define KPAD_ROW1_PORT  GPIOD
#define KPAD_ROW1_PIN   7
#define KPAD_ROW2_PORT  GPIOD
#define KPAD_ROW2_PIN   8
#define KPAD_ROW3_PORT  GPIOD
#define KPAD_ROW3_PIN   9
#define KPAD_COL0_PORT  GPIOD
#define KPAD_COL0_PIN   0
#define KPAD_COL1_PORT  GPIOD
#define KPAD_COL1_PIN   1
#define KPAD_COL2_PORT  GPIOD
#define KPAD_COL2_PIN   2
#define KPAD_COL3_PORT  GPIOD
#define KPAD_COL3_PIN   3

/* LCD directly connected on PE3-PE10: see LCD.h */

/* I2C1 for LCD over I2C (PB6 = SCL, PB7 = SDA): see I2C.h */
#ifndef SDA_PIN
  #define SDA_PIN  7   /* PB7 */
#endif
#ifndef SCL_PIN
  #define SCL_PIN  6   /* PB6 */
#endif
#define I2C_AF  4UL    /* AF4 */

#endif  /* ARM_KIT_BOARD_DISCO_F407VG */
