/**
 * @file LCD_customChar_table.h
 * @brief Example 5x8 bitmap definitions for HD44780 custom characters.
 *        Pass one of these arrays to LCD_customChar_write().
 */

#ifndef ARM_KIT_LCD_CUSTOM_CHAR_TABLE
#define ARM_KIT_LCD_CUSTOM_CHAR_TABLE

static uint8_t fullChar[] = {
  0x1F,   /* 11111 */
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F
};

static uint8_t emptyChar[] = {
  0x00,   /* 00000 */
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00
};

static uint8_t batteryFull[] = {
  0x0E,   /* 01110 – top cap */
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F
};

#endif  /* ARM_KIT_LCD_CUSTOM_CHAR_TABLE */
