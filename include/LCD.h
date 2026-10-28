/**
 * @file LCD.h
 * @brief 4-bit parallel driver for HD44780-compatible LCDs on STM32F4-DISCOVERY.
 *
 * Pins: PE3 = RS, PE4 = R/W, PE5 = E, PE6-PE9 = D4-D7.
 * Define LCD_COLS and LCD_ROWS before including.
 *
 * LCD_setCursor(row, col) – row comes first, matching the I2C drivers.
 */

#ifndef ARM_KIT_LCD
#define ARM_KIT_LCD

#include "kit.h"
#ifndef STM32F407xx
  #error "LCD.h is for STM32F4-DISCOVERY (STM32F407xx) only. Use LCD_I2C.h for other boards."
#endif

#include "chrono.h"
#include <string.h>

#define LCD_ON           0x0C
#define LCD_OFF          0x08
#define LCD_CLR          0x01
#define LCD_CUR_ON       0x0E
#define LCD_CUR_OFF      LCD_ON
#define LCD_CUR_BLINK    0x0F
#define LCD_CUR_NO_BLINK LCD_CUR_ON
#define LCD_CUR_HOME     0x03
#define LCD_SL           0x18
#define LCD_SR           0x1C
#define LCD_LINE0        0x80
#define LCD_LINE1        0xC0
#define LCD_LINE2        0x94
#define LCD_LINE3        0xD4

const uint16_t LCD_RESOLUTION = (LCD_COLS * 100) + LCD_ROWS;

static void LCD_busy(void) { delay(10); }

static void LCD_write_nibble(uint16_t nibble) {
  uint16_t x;
  GPIOE->BSRR = (1UL << (4+16)) | (1UL << (5+16));  /* R/W=0, E=0 */
  delay(1);
  GPIOE->BSRR = (1UL << 5);                          /* E=1 */
  nibble = (nibble & 0x0F) << 6;
  x = GPIOE->ODR & 0xFC3F;
  GPIOE->ODR = nibble | x;
  delay(1);
  GPIOE->BSRR = (1UL << (5+16));                     /* E=0 */
  delay(1);
}

/**
 * @brief Send a command byte to the LCD.
 */
void LCD_set(uint8_t cmd) {
  LCD_busy();
  GPIOE->BSRR = (1UL << (3+16));   /* RS=0 */
  GPIOE->BSRR = (1UL << (4+16));   /* R/W=0 */
  LCD_write_nibble(cmd >> 4);
  LCD_write_nibble(cmd);
}

/**
 * @brief Initialize the LCD (enables port E clock and configures pins PE3-PE10).
 */
void LCD_setup(void) {
  uint8_t i;
  RCC->AHB1ENR |= (1UL << 4);   /* port E clock */

  for (i = 3; i < 11; i++) {
    GPIOE->MODER   &= ~(3UL << 2*i);
    GPIOE->MODER   |=  (1UL << 2*i);   /* output */
    GPIOE->OTYPER  &= ~(1UL << i);     /* push-pull */
    GPIOE->OSPEEDR &= ~(3UL << 2*i);
    GPIOE->OSPEEDR |=  (2UL << 2*i);   /* high speed */
    GPIOE->PUPDR   &= ~(3UL << 2*i);   /* no pull */
  }

  LCD_set(0x03); LCD_set(0x03); LCD_set(0x02);  /* reset to 4-bit */
  LCD_set(0x28);  /* 4-bit, 2 lines, 5x8 */
  LCD_set(0x0F);  /* display on, cursor on, blink */
  LCD_set(0x06);  /* increment, no shift */
  LCD_set(0x01);  /* clear */
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

/**
 * @brief Write a custom character bitmap to CGRAM.
 * @param position  CGRAM slot (0-7).
 * @param customChar  Pointer to an 8-byte bitmap.
 */
void LCD_customChar_write(uint8_t position, uint8_t *customChar) {
  uint8_t i;
  if (position >= 8) return;

  GPIOE->BSRR = (1UL << 3);                   /* RS=1 */
  LCD_set(0x40 | (position << 3));             /* CGRAM address */

  for (i = 0; i < 8; i++) {
    LCD_busy();
    GPIOE->BSRR = (1UL << 3);                 /* RS=1 */
    GPIOE->BSRR = (1UL << (4+16));            /* R/W=0 */
    LCD_write_nibble(customChar[i] >> 4);
    LCD_write_nibble(customChar[i]);
    delay(1);
  }

  LCD_set(0x80);   /* back to DDRAM */
}

/**
 * @brief Display the custom character stored in CGRAM slot position.
 */
void LCD_customChar_read(uint8_t position) {
  if (position >= 8) return;
  LCD_busy();
  GPIOE->BSRR = (1UL << 3);    /* RS=1 */
  GPIOE->BSRR = (1UL << 4);    /* R/W=1 */
  LCD_write_nibble(position >> 4);
  LCD_write_nibble(position);
  delay(1);
}

/** Write one character to the current cursor position. */
void LCD_put(uint8_t data) {
  LCD_busy();
  GPIOE->BSRR = (1UL << 3);        /* RS=1 */
  GPIOE->BSRR = (1UL << (4+16));   /* R/W=0 */
  LCD_write_nibble(data >> 4);
  LCD_write_nibble(data & 0x0F);
}

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

#endif  /* ARM_KIT_LCD */
