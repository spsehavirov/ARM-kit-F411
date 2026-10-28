# SPSE ARM kit

Register-level STM32 programming kit for year 3 at SPSE Havirov.

One SEGGER Embedded Studio project for all supported boards –
select your board from the configuration dropdown in the toolbar.

> **Full documentation (pin assignments, setup guide, driver reference):**
> download `ARM_kit_docs.pdf` from the [Releases](../../releases) page.

---

## Supported boards

| Configuration | Board | LCD |
|---|---|---|
| **F411 Debug** | NUCLEO-F411RE + SPSE shield | I2C (PCF8574) |
| **F407 Debug** | STM32F4-DISCOVERY | Direct 4-bit parallel |
| **G071 Debug** | NUCLEO-G071RB + SPSE shield | I2C (PCF8574) |

---

## Getting started

1. Open `ARM_kit.emProject` in SEGGER Embedded Studio.
2. Select the configuration that matches your board (e.g. **F411 Debug**) in the toolbar dropdown.
3. To run an example from the `examples/` folder:
   - Right-click the example file → **Options** → **Exclude From Build** = **No**
   - Set `src/main.c` → **Options** → **Exclude From Build** = **Yes**
   - *(Only one file with `main()` may be active at a time)*
4. **F7** – build, **F5** – flash and start the debugger.

---

## Drivers

| File | Description | Boards |
|---|---|---|
| `include/kit.h` | Top-level include – automatically pulls in the MCU header and board pin map. | All |
| `include/chrono.h` | `delay(ms)`, `cekej(n)`, SysTick handler. | All |
| `include/I2C.h` | I2C1 master driver. | F407, F411 |
| `include/I2Cg.h` | I2C1 master driver. | G071 |
| `include/LCD_I2C.h` | HD44780 LCD over I2C (PCF8574). `LCD_init()`, `LCD_setCursor(row, col)`. | F407, F411 |
| `include/LCD_I2Cg.h` | Same API as LCD_I2C.h. | G071 |
| `include/LCD.h` | HD44780 LCD direct 4-bit. `LCD_setup()`, `LCD_setCursor(row, col)`. | F407 |
| `include/KPAD.h` | 3×4 / 4×4 matrix keypad. `KPAD_setup()`, `KPAD_getKey()`. | F407, F411 |
| `include/KPADg.h` | Same API as KPAD.h. | G071 |

---

## Examples

| File | What it demonstrates |
|---|---|
| `example_01-externalLED_01.c` | Blink an LED with a busy-loop delay (`cekej`) |
| `example_01-externalLED_02-SysTick.c` | Blink an LED with a SysTick delay (`delay`) |
| `example_02-userButton_01-controlLED.c` | Button controls LEDs; LED table with port/pin pairs |
| `example_03-LCD_01.c` | Print text on the LCD |
| `example_03-LCD_02-customChar.c` | Custom character stored in CGRAM |
| `example_03-LCD_I2C_01.c` | I2C LCD cursor blink demo |
| `example_04-KPAD_01.c` | Read a key from the keypad and show it on the LCD |
| `example_05-HWtest_all.c` | Full hardware test – LEDs, buttons, keypad, LCD, ADC, UART |

---

## UART via USB

Connect at **115200 baud, 8N1**, no hardware flow control.

| Board | Connection |
|---|---|
| F411 / G071 | USB cable to the ST-LINK connector (CN1) – appears as a virtual COM port |
| F407 | External USB-UART adapter on PA2 (TX) / PA3 (RX) |

---

## Board differences

| | F407 Discovery | F411 Nucleo | G071 Nucleo |
|---|---|---|---|
| Core | Cortex-M4F | Cortex-M4F | Cortex-M0+ |
| GPIO clock register | `RCC->AHB1ENR` | `RCC->AHB1ENR` | `RCC->IOPENR` |
| Pin reset state | Input | Input | Analog |
| On-board LED | PD12–PD15 | PA5 | PA5 |
| User button | PA0, pressed = 1 | PC13, pressed = 0 | PC13, pressed = 0 |
| Kit LEDs | 4× PE12–PE15, active-low | 8× shield, active-high | 8× shield, active-high |
| I2C peripheral | Legacy SR1/DR/CCR, AF4 | Legacy SR1/DR/CCR, AF4 | New TIMINGR/CR2/ISR, AF6 |
| UART | External USB-UART adapter | USB (ST-LINK) | USB (ST-LINK) |

---

*Copyright SPSE Havirov © 2023–2026*
