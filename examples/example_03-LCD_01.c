/**
 * @file example_03-LCD_01.c
 * @brief Write text to a directly connected LCD (STM32F4-DISCOVERY only).
 *        For LCD over I2C see example_04-LCD_I2C_01.c.
 */

#include "kit.h"

#define CLK_DIV   1000   /* SysTick period = 1 ms */
#define STEP      1000   /* step interval in ms */

#define LCD_COLS  20
#define LCD_ROWS  4

#include "chrono.h"
#include "LCD.h"

int main(void) {
  SystemCoreClockUpdate();
  SysTick_Config(SystemCoreClock / CLK_DIV);

  LCD_setup();

  while (1) {
    LCD_set(LCD_CUR_ON);
    delay(STEP);

    LCD_set(LCD_CUR_BLINK);
    delay(STEP * 2);

    LCD_set(LCD_CUR_NO_BLINK);
    delay(STEP);

    LCD_set(LCD_CUR_OFF);
    delay(STEP);

    LCD_put('#');
    delay(STEP);

    LCD_set(LCD_LINE0);
    LCD_print("Hello");
    delay(STEP);

    LCD_setCursor(1, 0);   /* row 1, col 0 */
    LCD_print("World!");
    delay(STEP);

    LCD_clear();
  }

  return 0;
}
