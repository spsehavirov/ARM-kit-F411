/**
 * @file example_03-LCD_02-customChar.c
 * @brief Store a custom character in CGRAM and display it (STM32F4-DISCOVERY only).
 *        For LCD over I2C see example_04-LCD_I2C_01.c.
 */

#include "kit.h"

#define CLK_DIV   1000   /* SysTick period = 1 ms */
#define STEP       500   /* step interval in ms */

#define LCD_COLS   8
#define LCD_ROWS   2

#include "chrono.h"
#include "LCD.h"
#include "LCD_customChar_table.h"

int main(void) {
  SystemCoreClockUpdate();
  SysTick_Config(SystemCoreClock / CLK_DIV);

  LCD_setup();

  LCD_customChar_write(0, batteryFull);   /* store bitmap in CGRAM slot 0 */
  LCD_customChar_read(0);                 /* display character from slot 0 */

  while (1) { /* nothing to do */ }

  return 0;
}
