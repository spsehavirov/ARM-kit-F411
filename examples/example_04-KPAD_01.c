/**
 * @file example_04-KPAD_01.c
 * @brief Read a key from the matrix keypad and display it on the LCD.
 */

#include "kit.h"

#define CLK_DIV   1000   /* SysTick period = 1 ms */
#define STEP      1000   /* display hold time in ms */

#define LCD_COLS   20
#define LCD_ROWS   4
#define LCD_ADDR   0x4E

#if defined(STM32F407xx)
  #define LCD_PARALLEL  1   /* 1 = directly connected LCD; 0 = I2C LCD */
#else
  #define LCD_PARALLEL  0   /* the shield uses I2C only */
#endif

#define KPAD_COLS  4
#define KPAD_ROWS  4

static uint8_t KPAD_KeyMap[KPAD_ROWS][KPAD_COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

#include "chrono.h"
#if LCD_PARALLEL
  #include "LCD.h"
#else
  #if defined(STM32G071xx)
    #include "LCD_I2Cg.h"
  #else
    #include "LCD_I2C.h"
  #endif
#endif
#if defined(STM32G071xx)
  #include "KPADg.h"
#else
  #include "KPAD.h"
#endif

int main(void) {
  uint8_t key;

  SystemCoreClockUpdate();
  SysTick_Config(SystemCoreClock / CLK_DIV);

#if LCD_PARALLEL
  LCD_setup();
#else
  I2C_GPIO_init();
  I2C_init();
  delay(10);
  {
    uint8_t n, ok = 0;
    for (n = 0; n < 5 && !ok; n++) {   /* wake up the bus / check the LCD responds */
      ok = I2C_probe(LCD_ADDR);
      if (!ok) delay(10);
    }
  }
  LCD_init();
#endif
  KPAD_setup();

  while (1) {
    LCD_clear();
    LCD_setCursor(0, 0);
    LCD_set(LCD_CUR_BLINK);

    do { key = KPAD_getKey(); } while (!key);

    LCD_put(key);
    LCD_set(LCD_CUR_OFF);
    delay(STEP);
  }

  return 0;
}
