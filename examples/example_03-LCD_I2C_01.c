/**
 * @file example_03-LCD_I2C_01.c
 * @brief Write text to an LCD connected over I2C (PCF8574 expander).
 *
 * Default LCD_ADDR is 0x4E (PCF8574).
 * If the display does not respond, run a bus scan with I2C_probe() or
 * check the address jumpers on the back of the PCF8574 board.
 * PCF8574A modules typically use 0x7E.
 */

#include "kit.h"

#define CLK_DIV   1000   /* SysTick period = 1 ms */
#define STEP      1000   /* step interval in ms */

#define LCD_COLS   20
#define LCD_ROWS   2
#define LCD_ADDR   0x4E  /* change if your module uses a different address */

#include "chrono.h"
#if defined(STM32G071xx)
  #include "LCD_I2Cg.h"
#else
  #include "LCD_I2C.h"
#endif

int main(void) {
  SystemCoreClockUpdate();
  SysTick_Config(SystemCoreClock / CLK_DIV);
  uint8_t n, ok = 0;

  I2C_GPIO_init();
  I2C_init();
  delay(10);
  for (n = 0; n < 5 && !ok; n++) {      /* wake up the bus / check the LCD responds */
    ok = I2C_probe(LCD_ADDR);
    if (!ok) delay(10);
  }
  LCD_init();

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

    LCD_setCursor(0, 0);   /* row 0, col 0 */
    LCD_print("World!");
    delay(STEP);

    LCD_clear();
  }

  return 0;
}
