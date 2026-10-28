/**
 * @file LCD_I2C.h
 * @brief I2C driver for HD44780-compatible LCDs using a PCF8574 I/O expander (STM32F4).
 *
 * For STM32G071 use LCD_I2Cg.h instead.
 *
 * Define before including:
 *   LCD_ADDR  – 7-bit I2C address shifted left by 1 (default 0x4E for PCF8574; PCF8574A is often 0x7E).
 *
 * LCD_setCursor(row, col) – row 0 is the top row.
 */

#ifndef ARM_KIT_LCD_I2C
#define ARM_KIT_LCD_I2C

#include "kit.h"
#if defined(STM32G071xx)
  #error "Use LCD_I2Cg.h for STM32G071."
#endif

#include "chrono.h"
#include "I2C.h"

#ifndef LCD_ADDR
  #define LCD_ADDR  0x4E
#endif

#include <string.h>

#define LCD_ON           0x0C   /* display on, cursor off */
#define LCD_OFF          0x08   /* display off */
#define LCD_CLR          0x01   /* clear display */
#define LCD_CUR_ON       0x0E   /* cursor on, no blink */
#define LCD_CUR_OFF      LCD_ON
#define LCD_CUR_BLINK    0x0F   /* cursor on, blink */
#define LCD_CUR_NO_BLINK LCD_CUR_ON
#define LCD_CUR_HOME     0x03   /* cursor to home */
#define LCD_SL           0x18   /* shift display left */
#define LCD_SR           0x1C   /* shift display right */
#define LCD_LINE0        0x80   /* DDRAM address of row 0, column 0 */
#define LCD_LINE1        0xC0   /* row 1 */
#define LCD_LINE2        0x94   /* row 2 */
#define LCD_LINE3        0xD4   /* row 3 */

/**
 * @brief Send a command (or data) nibble-pair to the LCD over I2C.
 * @param cmd  Command byte.  Set bit 8 (0x100) to write character data.
 */
void LCD_set(uint16_t cmd) {
  uint8_t d_h = (cmd & 0x00F0);
  uint8_t d_l = ((cmd << 4) & 0x00F0);

  I2C_start();
  I2C_send_addr(LCD_ADDR);

  if (cmd & 0x0100) {
    /* data write: RS=1 */
    I2C_write(0x40);             /* data register select */
    I2C_write(d_h | 0x0D);      /* EN=1, RS=1, RW=0 */
    delay(2);
    I2C_write(d_h | 0x09);      /* EN=0, RS=1, backlight */
    I2C_write(d_l | 0x0D);
    delay(2);
    I2C_write(d_l | 0x09);
  } else {
    /* command write: RS=0 */
    I2C_write(0x00);             /* instruction register select */
    I2C_write(d_h | 0x0C);      /* EN=1, RS=0, RW=0 */
    delay(2);
    I2C_write(d_h | 0x08);      /* EN=0, RS=0, backlight */
    I2C_write(d_l | 0x0C);
    delay(2);
    I2C_write(d_l | 0x08);
  }

  I2C_stop();
}

/**
 * @brief Initialize the LCD (call after I2C_GPIO_init() and I2C_init()).
 */
void LCD_init(void) {
  delay(50);
  LCD_set(0x03); delay(5);
  LCD_set(0x03); delay(1);
  LCD_set(0x02); delay(10);  /* 4-bit mode */

  LCD_set(0x28); delay(5);   /* 4-bit, 2 lines, 5x8 dots */
  LCD_set(0x0F); delay(5);   /* display on, cursor on, blink */
  LCD_set(0x06); delay(5);   /* increment, no shift */
  LCD_set(0x01); delay(5);   /* clear */
}

/** Clear the display and move the cursor to row 0, column 0. */
void LCD_clear(void) { LCD_set(LCD_CLR); }

/**
 * @brief Move the cursor to the given position.
 * @param row  Zero-based row index (0 = top).
 * @param col  Zero-based column index.
 */
void LCD_setCursor(uint8_t row, uint8_t col) {
  switch (row) {
    case 1:  LCD_set(LCD_LINE1 + col); break;
    case 2:  LCD_set(LCD_LINE2 + col); break;
    case 3:  LCD_set(LCD_LINE3 + col); break;
    default: LCD_set(LCD_LINE0 + col); break;
  }
}

/** Write one character to the current cursor position. */
void LCD_put(uint8_t data) { LCD_set(0x0100 | data); }

/**
 * @brief Write a null-terminated string to the LCD.
 *        Does not wrap; handle line breaks in the caller.
 */
void LCD_print(const char *__restrict__ text) {
  uint8_t i;
  for (i = 0; i < strlen(text); i++) {
    LCD_put((uint8_t)text[i]);
  }
}

#endif  /* ARM_KIT_LCD_I2C */
