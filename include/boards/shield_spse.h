/**
 * @file shield_spse.h
 * @brief Pin mapping for the SPSE expansion shield (Arduino Uno R3 form factor).
 */

#ifndef ARM_KIT_SHIELD_SPSE
#define ARM_KIT_SHIELD_SPSE

/* LEDs D1-D8 (index 0 = D1, index 7 = D8); active-high */
#define KIT_LED_COUNT   8
#define KIT_LED_ACTIVE  1
#define LED0_PORT  ARD_A3_PORT   /* D1 */
#define LED0_PIN   ARD_A3_PIN
#define LED1_PORT  ARD_D3_PORT   /* D2 */
#define LED1_PIN   ARD_D3_PIN
#define LED2_PORT  ARD_D5_PORT   /* D3 */
#define LED2_PIN   ARD_D5_PIN
#define LED3_PORT  ARD_D4_PORT   /* D4 */
#define LED3_PIN   ARD_D4_PIN
#define LED4_PORT  ARD_D10_PORT  /* D5 */
#define LED4_PIN   ARD_D10_PIN
#define LED5_PORT  ARD_D6_PORT   /* D6 */
#define LED5_PIN   ARD_D6_PIN
#define LED6_PORT  ARD_A5_PORT   /* D7 */
#define LED6_PIN   ARD_A5_PIN
#define LED7_PORT  ARD_A4_PORT   /* D8 */
#define LED7_PIN   ARD_A4_PIN

/* Keypad U1 (connector pins 2-9)
 * Driver scan order (KPAD_rowPin[] index 0 = key-"1" row):
 *   ROW0 = D12 = PA6  (key row 1: 1 2 3 A)
 *   ROW1 = D13 = PA5  (key row 2: 4 5 6 B) – shared with on-board LED, flashes during scan
 *   ROW2 = A2  = PA4  (key row 3: 7 8 9 C)
 *   ROW3 = A0  = PA0  (key row 4: * 0 # D)
 *   COL0 = D11 = PA7  (key col 1: 1 4 7 *)
 *   COL1 = D8  = PA9  (key col 2: 2 5 8 0)
 *   COL2 = D7  = PA8  (key col 3: 3 6 9 #)
 *   COL3 = D2  = PA10 (key col 4: A B C D)
 */
#define KPAD_ROW0_PORT  ARD_D12_PORT  /* U1 pin 5 – key row 1 (1,2,3,A) */
#define KPAD_ROW0_PIN   ARD_D12_PIN
#define KPAD_ROW1_PORT  ARD_D13_PORT  /* U1 pin 4 – key row 2; shared with on-board LED */
#define KPAD_ROW1_PIN   ARD_D13_PIN
#define KPAD_ROW2_PORT  ARD_A2_PORT   /* U1 pin 3 – key row 3 */
#define KPAD_ROW2_PIN   ARD_A2_PIN
#define KPAD_ROW3_PORT  ARD_A0_PORT   /* U1 pin 2 – key row 4 (*,0,#,D) */
#define KPAD_ROW3_PIN   ARD_A0_PIN
#define KPAD_COL0_PORT  ARD_D11_PORT  /* U1 pin 6 – key col 1 (1,4,7,*) */
#define KPAD_COL0_PIN   ARD_D11_PIN
#define KPAD_COL1_PORT  ARD_D8_PORT   /* U1 pin 8 – key col 2 */
#define KPAD_COL1_PIN   ARD_D8_PIN
#define KPAD_COL2_PORT  ARD_D7_PORT   /* U1 pin 7 – key col 3 */
#define KPAD_COL2_PIN   ARD_D7_PIN
#define KPAD_COL3_PORT  ARD_D2_PORT   /* U1 pin 9 – key col 4 (A,B,C,D) */
#define KPAD_COL3_PIN   ARD_D2_PIN

/* Photoresistor on A1 */
#define KIT_HAS_LDR   1
#define LDR_PORT      ARD_A1_PORT
#define LDR_PIN       ARD_A1_PIN
#define LDR_ADC_CH    ARD_A1_ADC_CH

/* Shield button S1 on D9 (active-low) */
#define KIT_HAS_BTN2  1
#define BTN2_PORT     ARD_D9_PORT
#define BTN2_PIN      ARD_D9_PIN
#define BTN2_ACTIVE   0

#endif  /* ARM_KIT_SHIELD_SPSE */
