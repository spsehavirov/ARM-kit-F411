/**
 * @file example_05-HWtest_all.c
 * @brief Hardware self-test for all supported boards.
 *
 * Connect with a terminal at 115200 baud, 8N1.
 * UART commands: m = next LED mode, a = toggle auto-report, s = status, ? = help.
 * LED modes: 0 = ADC level bar, 1 = last key (binary value), 2 = running light.
 *
 * Board differences:
 *   F411 Nucleo + shield: 8 LEDs, keypad on PA, LCD over I2C, photoresistor PA1,
 *                         board button PC13 + shield button PC7, UART via USB.
 *   F407 Discovery:       4 kit LEDs PE12-15 (active-low) mirrored on the 4 on-board
 *                         LEDs PD12-15 (active-high), keypad on PD, LCD direct 4-bit
 *                         (PE3-PE9), board button PA0, photoresistor PA1, UART via adapter.
 *                         LCD: row 0 = ADC, row 1 = last key + mode.
 *   G071 Nucleo + shield: same as F411 but different peripherals (g-variants);
 *                         shield LEDs all on port B (PB1,3,4,5,0,14,12,11).
 */

#include "kit.h"

#define CLK_DIV      1000
#define BAUD         115200

#define KPAD_STEP    0
#define KPAD_COLS    4
#define KPAD_ROWS    4

static uint8_t KPAD_KeyMap[KPAD_ROWS][KPAD_COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

#define LCD_COLS     20
#define LCD_ROWS     4
#define LCD_ADDR     0x4E
#define LCD_REFRESH  500

#include "chrono.h"
#if defined(STM32G071xx)
  #include "LCD_I2Cg.h"
  #include "KPADg.h"
#elif defined(STM32F407xx)
  #include "LCD.h"          /* direct 4-bit parallel */
  #include "KPAD.h"
#else
  #include "LCD_I2C.h"
  #include "KPAD.h"
#endif


/* ============================================================
 * LED helpers
 * F407: kit LEDs PE12-PE15 (active-low) + on-board LEDs PD12-PD15
 *       (green, orange, red, blue; active-high) – same pattern on both
 * F411 shield: PB0,3,4,5,6,10 + PC0,1, active-high, 8 LEDs
 * G071 shield: PB1,3,4,5,0,14,12,11,  active-high, 8 LEDs
 * ============================================================ */

#if defined(STM32F407xx)

#define LED_COUNT  4
static const uint8_t _lp[LED_COUNT]  = { 12, 13, 14, 15 };   /* PE – kit LEDs      */
static const uint8_t _lpd[LED_COUNT] = { 12, 13, 14, 15 };   /* PD – on-board LEDs */

static void ledsInit(void) {
  uint8_t i;
  RCC->AHB1ENR |= (1UL << 3) | (1UL << 4);   /* port D + E */
  for (i = 0; i < LED_COUNT; i++) {
    /* kit LEDs on PE, active-low */
    GPIOE->BSRR   =  (1UL << _lp[i]);           /* off = set */
    GPIOE->MODER &= ~(3UL << _lp[i]*2);
    GPIOE->MODER |=  (1UL << _lp[i]*2);
    GPIOE->OTYPER&= ~(1UL << _lp[i]);

    /* on-board LEDs on PD, active-high */
    GPIOD->BSRR   =  (1UL << (_lpd[i] + 16));   /* off = reset */
    GPIOD->MODER &= ~(3UL << _lpd[i]*2);
    GPIOD->MODER |=  (1UL << _lpd[i]*2);
    GPIOD->OTYPER&= ~(1UL << _lpd[i]);
  }
}

static void ledWrite(uint8_t mask) {
  uint8_t i;
  for (i = 0; i < LED_COUNT; i++) {
    if (mask & (1U << i)) {
      GPIOE->BSRR = (1UL << (_lp[i] + 16));    /* kit LED on = clear */
      GPIOD->BSRR = (1UL << _lpd[i]);          /* board LED on = set */
    } else {
      GPIOE->BSRR = (1UL << _lp[i]);
      GPIOD->BSRR = (1UL << (_lpd[i] + 16));
    }
  }
}

#else

#define LED_COUNT  8
#if defined(STM32G071xx)
/* G071: D1-D8 = A3, D3, D5, D4, D10, D6, A5, A4 */
static GPIO_TypeDef * const _lport[LED_COUNT] = {
  GPIOB, GPIOB, GPIOB, GPIOB, GPIOB, GPIOB, GPIOB, GPIOB
};
static const uint8_t _lp[LED_COUNT] = { 1, 3, 4, 5, 0, 14, 12, 11 };
#else
/* F411: D1-D8 = A3, D3, D5, D4, D10, D6, A5, A4 */
static GPIO_TypeDef * const _lport[LED_COUNT] = {
  GPIOB, GPIOB, GPIOB, GPIOB, GPIOB, GPIOB, GPIOC, GPIOC
};
static const uint8_t _lp[LED_COUNT] = { 0, 3, 4, 5, 6, 10, 0, 1 };
#endif

static void ledsInit(void) {
  uint8_t i, p;
#if defined(STM32G071xx)
  RCC->IOPENR  |= (1UL << 1);                /* port B */
#else
  RCC->AHB1ENR |= (1UL << 1) | (1UL << 2);   /* port B + C */
#endif
  for (i = 0; i < LED_COUNT; i++) {
    p = _lp[i];
    _lport[i]->BSRR   =  (1UL << (p + 16));   /* off */
    _lport[i]->MODER &= ~(3UL << p*2);
    _lport[i]->MODER |=  (1UL << p*2);
    _lport[i]->OTYPER&= ~(1UL << p);
  }
}

static void ledWrite(uint8_t mask) {
  uint8_t i, p;
  for (i = 0; i < LED_COUNT; i++) {
    p = _lp[i];
    if (mask & (1U << i)) _lport[i]->BSRR = (1UL << p);
    else                  _lport[i]->BSRR = (1UL << (p + 16));
  }
}

#endif

/* ============================================================
 * ADC helpers – PA1 on all boards
 * F407/F411: ADC1 legacy.  G071: ADC1 new.
 * PA1 = photoresistor on all boards.
 * ============================================================ */

#define ADC_MAX  4095U

static void adcInit(void) {
  volatile uint32_t i;
#if defined(STM32G071xx)
  RCC->IOPENR  |= (1UL << 0);
  RCC->APBENR2 |= RCC_APBENR2_ADCEN;
  ADC1->CFGR2 &= ~ADC_CFGR2_CKMODE;
  ADC1->CFGR2 |=  ADC_CFGR2_CKMODE_0;
  ADC1->CR    |=  ADC_CR_ADVREGEN;
  for (i = 0; i < 1000; i++) {}
  ADC1->CR    |=  ADC_CR_ADCAL;
  while (ADC1->CR & ADC_CR_ADCAL) {}
  for (i = 0; i < 100; i++) {}
  ADC1->CFGR1  =  0;
  ADC1->SMPR   = (7UL << ADC_SMPR_SMP1_Pos);
  ADC1->ISR    =  ADC_ISR_ADRDY;
  ADC1->CR    |=  ADC_CR_ADEN;
  while (!(ADC1->ISR & ADC_ISR_ADRDY)) {}
#else
  RCC->AHB1ENR |= (1UL << 0);
  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
  ADC1->CR1 = 0; ADC1->CR2 = 0; ADC1->SQR1 = 0;
  ADC1->CR2 |= ADC_CR2_ADON;
  for (i = 0; i < 1000; i++) {}
#endif
  GPIOA->MODER |=  (3UL << 1*2);   /* PA1 analog */
  GPIOA->PUPDR &= ~(3UL << 1*2);
}

static uint16_t adcRead(void) {
#if defined(STM32G071xx)
  ADC1->ISR    =  ADC_ISR_CCRDY;
  ADC1->CHSELR = (1UL << 1);
  while (!(ADC1->ISR & ADC_ISR_CCRDY)) {}
  ADC1->CR    |=  ADC_CR_ADSTART;
  while (!(ADC1->ISR & ADC_ISR_EOC)) {}
#else
  ADC1->SMPR2 |= (7UL << 3);
  ADC1->SQR3   =  1;
  ADC1->CR2   |=  ADC_CR2_SWSTART;
  while (!(ADC1->SR & ADC_SR_EOC)) {}
#endif
  return (uint16_t)(ADC1->DR & 0x0FFF);
}

/* ============================================================
 * UART helpers – USART2 PA2/PA3 on all boards
 * F4: AF7, SR/DR.  G071: AF1, ISR/TDR/RDR.
 * ============================================================ */

static void uartInit(uint32_t baud) {
#if defined(STM32G071xx)
  RCC->IOPENR  |= (1UL << 0);
  RCC->APBENR1 |= RCC_APBENR1_USART2EN;
  GPIOA->MODER &= ~((3UL << 2*2) | (3UL << 3*2));
  GPIOA->MODER |=  ((2UL << 2*2) | (2UL << 3*2));
  GPIOA->PUPDR &= ~(3UL << 3*2); GPIOA->PUPDR |= (1UL << 3*2);
  GPIOA->AFR[0] &= ~((0xFUL << 2*4) | (0xFUL << 3*4));
  GPIOA->AFR[0] |=  ((1UL << 2*4) | (1UL << 3*4));   /* AF1 */
#else
  RCC->AHB1ENR |= (1UL << 0);
  RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
  GPIOA->MODER &= ~((3UL << 2*2) | (3UL << 3*2));
  GPIOA->MODER |=  ((2UL << 2*2) | (2UL << 3*2));
  GPIOA->PUPDR &= ~(3UL << 3*2); GPIOA->PUPDR |= (1UL << 3*2);
  GPIOA->AFR[0] &= ~((0xFUL << 2*4) | (0xFUL << 3*4));
  GPIOA->AFR[0] |=  ((7UL << 2*4) | (7UL << 3*4));   /* AF7 */
#endif
  USART2->CR1 = 0; USART2->CR2 = 0; USART2->CR3 = 0;
  USART2->BRR = (SystemCoreClock + baud/2) / baud;
  USART2->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

static void uartPutc(char c) {
#if defined(STM32G071xx)
  while (!(USART2->ISR & USART_ISR_TXE_TXFNF)) {}
  USART2->TDR = (uint8_t)c;
#else
  while (!(USART2->SR & USART_SR_TXE)) {}
  USART2->DR = (uint8_t)c;
#endif
}

static void uartPuts(const char *s) {
  while (*s) { if (*s == '\n') uartPutc('\r'); uartPutc(*s++); }
}

static void uartPutu(uint32_t v) {
  char buf[10]; uint8_t n = 0;
  do { buf[n++] = (char)('0' + v % 10); v /= 10; } while (v);
  while (n) uartPutc(buf[--n]);
}

static uint8_t uartAvailable(void) {
#if defined(STM32G071xx)
  if (USART2->ISR & USART_ISR_ORE) USART2->ICR = USART_ICR_ORECF;
  return (USART2->ISR & USART_ISR_RXNE_RXFNE) ? 1 : 0;
#else
  if (USART2->SR & USART_SR_ORE) (void)USART2->DR;
  return (USART2->SR & USART_SR_RXNE) ? 1 : 0;
#endif
}

static char uartGetc(void) {
  while (!uartAvailable()) {}
#if defined(STM32G071xx)
  return (char)(USART2->RDR & 0xFF);
#else
  return (char)(USART2->DR & 0xFF);
#endif
}

/* ============================================================
 * Button helpers
 * F407: only board button PA0 (pressed = 1), no shield button.
 * F411/G071: board button PC13 (pressed = 0) + shield button PC7 (pressed = 0).
 * ============================================================ */

static void buttonsInit(void) {
  KIT_GPIO_CLK_EN(BOARD_BTN_PORT);
  BOARD_BTN_PORT->MODER &= ~(3UL << BOARD_BTN_PIN*2);   /* input */
#if !defined(STM32F407xx)
  /* Shield button S1 on PC7, active-low – needs internal pull-up */
  KIT_GPIO_CLK_EN(BTN2_PORT);
  BTN2_PORT->MODER &= ~(3UL << BTN2_PIN*2);
  BTN2_PORT->PUPDR &= ~(3UL << BTN2_PIN*2);
  BTN2_PORT->PUPDR |=  (1UL << BTN2_PIN*2);
#endif
}

/* Returns: 0 = none, 1 = board button, 2 = shield button (F411/G071 only) */
static uint8_t buttons(void) {
  if (((BOARD_BTN_PORT->IDR >> BOARD_BTN_PIN) & 1UL) == BOARD_BTN_ACTIVE) return 1;
#if !defined(STM32F407xx)
  if (((BTN2_PORT->IDR >> BTN2_PIN) & 1UL) == BTN2_ACTIVE) return 2;
#endif
  return 0;
}

/* ============================================================
 * Display / status helpers
 * ============================================================ */

#define MODE_COUNT  3

/* LCD layout
 *   F407 (direct LCD):            F411/G071 (20x4 I2C):
 *   0: ADC:xxxx x.xxV             0: SPSE - HW TEST  M:x
 *   1: KEY:x        M:x           1: ADC:xxxx   x.xxV
 *                                 2: KEY:x      BTN:nnn
 *                                 3: UART:x     RX:nnn
 */
#if defined(STM32F407xx)
  #define LCD_ADC_ROW   0
  #define LCD_ADC_COL   4
  #define LCD_VOLT_COL  9
  #define LCD_MODE_ROW  1
  #define LCD_MODE_COL  15
#else
  #define LCD_ADC_ROW   1
  #define LCD_ADC_COL   4
  #define LCD_VOLT_COL  11
  #define LCD_MODE_ROW  0
  #define LCD_MODE_COL  18
#endif

volatile uint16_t ldrRaw, ldrMv;
volatile uint8_t  mode;
volatile uint8_t  lastInput = '-';
volatile uint16_t keyCount, btnCount, rxCount;
static   uint8_t  lcdOk;
static   uint8_t  autoReport = 1;

static void fmtU(char *buf, uint32_t v, uint8_t width) {
  char tmp[10]; uint8_t n = 0, i;
  do { tmp[n++] = (char)('0' + v % 10); v /= 10; } while (v && n < sizeof(tmp));
  for (i = n; i < width; i++) *buf++ = ' ';
  while (n) *buf++ = tmp[--n];
  *buf = 0;
}

static void fmtVolt(char *buf, uint16_t mv) {
  buf[0] = (char)('0' + (mv / 1000) % 10); buf[1] = '.';
  buf[2] = (char)('0' + (mv /  100) % 10);
  buf[3] = (char)('0' + (mv /   10) % 10); buf[4] = 'V'; buf[5] = 0;
}

static void lcdAt(uint8_t row, uint8_t col, const char *s) {
  if (!lcdOk) return;
  LCD_setCursor(row, col); LCD_print(s);
}

static void lcdCharAt(uint8_t row, uint8_t col, char c) {
  char b[2] = { c, 0 }; lcdAt(row, col, b);
}

static void lcdMode(void) { lcdCharAt(LCD_MODE_ROW, LCD_MODE_COL, (char)('0' + mode)); }

static void lcdInput(uint8_t row) {
#if defined(STM32F407xx)
  /* F407: one input field – row 1 shows the last key (keypad or UART) */
  (void)row;
  lcdCharAt(1, 4, (char)lastInput);
#else
  char b[4];
  if (row == 2) {
    lcdCharAt(2, 4, (char)lastInput);
    fmtU(b, btnCount % 1000, 3); lcdAt(2, 15, b);
  } else {
    lcdCharAt(3, 5, (char)lastInput);
    fmtU(b, rxCount % 1000, 3);  lcdAt(3, 14, b);
  }
#endif
}

static void nextMode(const char *src) {
  mode = (uint8_t)((mode + 1) % MODE_COUNT);
  uartPuts(src); uartPuts(" -> mode "); uartPutu(mode); uartPuts("\n");
  lcdMode();
}

static void printHelp(void) {
  uartPuts("\n=== SPSE HW TEST - " KIT_NAME " ===\n");
  uartPuts("ADC: photoresistor on PA1\n");
  uartPuts("Button ......... next LED mode\n");
  uartPuts("UART: m=mode  a=auto-report  s=status  ?=help\n");
  uartPuts("LED modes: 0=ADC level  1=last key  2=running light\n\n");
}

static void printStatus(void) {
  uartPuts("ADC=");   uartPutu(ldrRaw);
  uartPuts(" (");     uartPutu(ldrMv); uartPuts(" mV)  ");
  uartPuts("mode=");  uartPutu(mode);
  uartPuts("  key="); uartPutc((char)lastInput);
  uartPuts("  keys=");uartPutu(keyCount);
  uartPuts("  btn="); uartPutu(btnCount);
  uartPuts("  rx=");  uartPutu(rxCount);
  uartPuts("  LCD="); uartPuts(lcdOk ? "OK" : "--");
  uartPuts("\n");
}

/* ============================================================
 * Startup test
 * ============================================================ */

static void startupTest(void) {
  uint8_t i;

  /* LEDs: all on, then one at a time */
  ledWrite(0xFF);
  delay(400);
  for (i = 0; i < LED_COUNT; i++) { ledWrite((uint8_t)(1U << i)); delay(80); }
  ledWrite(0x00);

#if defined(STM32F407xx)
  /* LCD direct 4-bit */
  LCD_setup();
  lcdOk = 1;
  LCD_set(LCD_CUR_OFF);
  lcdAt(0, 0, "ADC:");
  lcdAt(1, 0, "KEY:-        M:");
#else
  /* LCD over I2C */
  I2C_GPIO_init();
  I2C_init();
  delay(10);
  {
    uint8_t n;
    for (n = 0; n < 5 && !lcdOk; n++) {
      lcdOk = I2C_probe(LCD_ADDR);
      if (!lcdOk) delay(10);
    }
  }
  if (!lcdOk) {
    uint16_t a;
    uartPuts("LCD: no response at LCD_ADDR. Devices found on I2C:");
    for (a = 0x10; a < 0xF0; a += 2) {
      if (I2C_probe((uint8_t)a)) {
        uartPuts(" 0x");
        uartPutc("0123456789ABCDEF"[a >> 4]);
        uartPutc("0123456789ABCDEF"[a & 0x0F]);
      }
    }
    uartPuts("  (update LCD_ADDR)\n");
    return;
  }
  LCD_init();
  LCD_set(LCD_CUR_OFF);
  lcdAt(0, 0, "SPSE - HW TEST  M:");
  lcdAt(1, 0, "ADC:");
  lcdAt(2, 0, "KEY:-      BTN:");
  lcdAt(3, 0, "UART:-     RX:");
#endif
  lcdMode(); lcdInput(2); lcdInput(3);
  uartPuts("LCD: OK\n");
}

/* ============================================================
 * Main
 * ============================================================ */

int main(void) {
  uint32_t now;
  uint32_t tAdc = 0, tLcd = 0, tKey = 0, tBtn = 0, tRun = 0, tRep = 0;
  uint8_t  keyPrev = 0, keyStable = 0, btnPrev = 0, btnStable = 0;
  uint8_t  runPos = 0, runDir = 0;
  char     c;

  SystemCoreClockUpdate();
  SysTick_Config(SystemCoreClock / CLK_DIV);

  uartInit(BAUD);
  printHelp();

#if defined(STM32F407xx)
  KPAD_setup();     /* F407: before ledsInit – keypad and on-board LEDs share port D */
  ledsInit();
  buttonsInit();
#else
  ledsInit();
  buttonsInit();
  KPAD_setup();
#endif
  adcInit();
  startupTest();
  uartPuts("Test running.\n\n");

  while (1) {
    now = Ticks;

    /* ADC: sample every 50 ms, update LCD every LCD_REFRESH ms */
    if ((now - tAdc) >= 50) {
      tAdc  = now;
      ldrRaw = adcRead();
      ldrMv  = (uint16_t)(((uint32_t)ldrRaw * 3300UL) / ADC_MAX);
    }
    if (lcdOk && (now - tLcd) >= LCD_REFRESH) {
      char b[8];
      tLcd = now;
      fmtU(b, ldrRaw, 4);  lcdAt(LCD_ADC_ROW, LCD_ADC_COL,  b);
      fmtVolt(b, ldrMv);   lcdAt(LCD_ADC_ROW, LCD_VOLT_COL, b);
    }

    /* Buttons: debounce 20 ms */
    if ((now - tBtn) >= 20) {
      uint8_t btn = buttons();
      tBtn = now;
      if (btn == btnPrev && btn != btnStable) {
        btnStable = btn;
        if (btn) {
          btnCount++;
          nextMode(btn == 1 ? "Board button" : "Shield button");
          lcdInput(2);
        }
      }
      btnPrev = btn;
    }

    /* Keypad: debounce 20 ms */
    if ((now - tKey) >= 20) {
      uint8_t key = KPAD_getKey();
      tKey = now;
      if (key == keyPrev && key != keyStable) {
        keyStable = key;
        if (key) {
          keyCount++;
          lastInput = key;
          uartPuts("KEY "); uartPutc((char)key); uartPuts("\n");
          lcdInput(2);
        }
      }
      keyPrev = key;
    }

    /* UART */
    if (uartAvailable()) {
      c = uartGetc();
      rxCount++;
      switch (c) {
        case 'm': nextMode("UART"); break;
        case 'a': autoReport = !autoReport;
                  uartPuts("Auto-report: "); uartPuts(autoReport ? "ON\n" : "OFF\n"); break;
        case 's': printStatus(); break;
        case '?': printHelp(); break;
        case '\r': case '\n': break;
        default:
          lastInput = (uint8_t)c;
          uartPuts("RX '"); uartPutc(c); uartPuts("'\n");
          break;
      }
      lcdInput(3);
    }

    /* LEDs by current mode */
    switch (mode) {
      case 0: {
        uint8_t level = (uint8_t)(((uint32_t)ldrRaw * (LED_COUNT + 1)) / (ADC_MAX + 1));
        ledWrite((uint8_t)((1U << level) - 1U));
        break;
      }
      case 1:
        ledWrite(lastInput);
        break;
      default:
        if ((now - tRun) >= 100) {
          tRun = now;
          ledWrite((uint8_t)(1U << runPos));
          if (runDir == 0) { if (++runPos >= LED_COUNT - 1) runDir = 1; }
          else             { if (runPos == 0 || --runPos == 0) runDir = 0; }
        }
        break;
    }

    /* Periodic status report */
    if (autoReport && (now - tRep) >= 1000) {
      tRep = now;
      printStatus();
    }
  }

  return 0;
}