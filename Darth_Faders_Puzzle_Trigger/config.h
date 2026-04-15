#pragma once

// ── Board: Adafruit Feather RP2040 (#4884) ────────────────────────────────────

// ── Slider ADC pins (wiper of each motorized fader) ──────────────────────────
#define PIN_SLIDER_R   A0   // GPIO26
#define PIN_SLIDER_G   A1   // GPIO27
#define PIN_SLIDER_Y   A2   // GPIO28

// ── Step switch input pins (active-LOW, internal pull-up enabled) ─────────────
#define PIN_SW_RED     12   // D12 / GPIO12
#define PIN_SW_GREEN   13   // D13 / GPIO13
#define PIN_SW_YELLOW  10   // D10 / GPIO10

// ── Step switch LED output pins ───────────────────────────────────────────────
#define PIN_LED_RED    11   // D11 / GPIO11 
#define PIN_LED_GREEN   9   // D9  / GPIO9
#define PIN_LED_YELLOW  8   // D6  / GPIO8

// ── Momentary button (active-LOW, internal pull-up enabled) ──────────────────
#define PIN_MOMENT_BTN  7   // D5  / GPIO7  
// ── Trigger output pin (HIGH for TRIGGER_DURATION_MS on correct solve) ────────────────────────
#define PIN_TRIGGER    24   // D24 / GPIO24  

// ── Motor FeatherWing channel assignments (M1-M3) ─────────────────────────────
#define MOTOR_CH_RED    1
#define MOTOR_CH_GREEN  2
#define MOTOR_CH_YELLOW 3

// ── Bargraph I2C address (default 0x70) ───────────────────────────────────────
// Solder A0 Bridge on back of bargraph
// I2C on RP2040 Feather: SDA=GPIO2, SCL=GPIO3 — Wire.begin() handles this automatically
#define BARGRAPH_ADDR  0x71

// ── Timing constants (ms) ─────────────────────────────────────────────────────
#define HOLD_SET_MS          5000
#define HOLD_RESET_MS       10000
#define BLINK_FAST_MS         100
#define BLINK_SLOW_MS         500
#define BLINK_VERY_FAST_MS     60
#define ALT_COLOR_MS          500
#define SUCCESS_BLINK_MS     4000
#define TRIGGER_DURATION_MS  3000

// ── Motor settings ────────────────────────────────────────────────────────────
#define MOTOR_SPEED_NORMAL  180
#define MOTOR_SPEED_SLOW    120
#define MOTOR_SETTLE_MS     120

// ── ADC auto-calibration ──────────────────────────────────────────────────────
// ADC_MIN_DEFAULT / ADC_MAX_DEFAULT are fallback values only.
// On every boot, calibrateSliders() in puzzle.cpp performs a live sweep and
// overwrites the runtime adcMin / adcMax variables automatically.
// RP2040 ADC is also 12-bit (0-4095) so defaults remain the same.
#define ADC_MIN_DEFAULT    100
#define ADC_MAX_DEFAULT   3900
#define ADC_CAL_MARGIN      50   // Safety margin trimmed from each end after sweep
#define CAL_SETTLE_MS      300   // Extra settle time after motor stops before reading
#define SLIDER_STEPS        24

// ── EEPROM layout  ────────────────────
// 3 bytes starting at address 0: R, G, Y solve code values
#define EEPROM_SIZE         4    // bytes allocated (3 data + 1 validity flag)
#define EEPROM_VALID_FLAG   0xA5 // written to byte 3 to confirm data is initialised
#define EEPROM_ADDR_R       0
#define EEPROM_ADDR_G       1
#define EEPROM_ADDR_Y       2
#define EEPROM_ADDR_FLAG    3

// ── Default solve code (restored on factory reset) ───────────────────────────
#define DEFAULT_SOLVE_R   7
#define DEFAULT_SOLVE_G   7
#define DEFAULT_SOLVE_Y   7
