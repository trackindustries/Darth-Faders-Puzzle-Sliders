#include "puzzle.h"
#include "config.h"
#include "motors.h"
#include "bargraph.h"
#include "switches.h"
#include <Arduino.h>
#include <EEPROM.h>

// ── EEPROM storage (replaces ESP32 Preferences/NVS) ──────────────────────────
// RP2040 Arduino-Pico core emulates EEPROM in flash.
// Call EEPROM.begin(EEPROM_SIZE) once in loadCode() before any read/write.
// Always call EEPROM.commit() after writes to flush to flash.

// ── Runtime ADC calibration values ───────────────────────────────────────────
// Set by calibrateSliders() on every boot. readSlider() uses these instead
// of the compile-time constants so calibration is always hardware-accurate.
static int adcMin = ADC_MIN_DEFAULT;
static int adcMax = ADC_MAX_DEFAULT;

// ── State enum ────────────────────────────────────────────────────────────────
enum PuzzleState {
  STATE_IDLE,
  STATE_SOLVE,
  STATE_SET,
  STATE_SET_COLOR,
  STATE_REVEAL,
  STATE_SUCCESS,
  STATE_FAIL
};
static PuzzleState state = STATE_IDLE;

// ── Color index constants ─────────────────────────────────────────────────────
#define IDX_R 0
#define IDX_G 1
#define IDX_Y 2
static const uint8_t BAR_COLORS[3] = { BAR_RED, BAR_GREEN, BAR_YELLOW };
static const uint8_t ADC_PINS[3] = { PIN_SLIDER_R, PIN_SLIDER_G, PIN_SLIDER_Y };
static const uint8_t MOTOR_CHS[3] = { MOTOR_CH_RED, MOTOR_CH_GREEN, MOTOR_CH_YELLOW };

// ── Solve code ────────────────────────────────────────────────────────────────
static uint8_t solveCode[3] = { DEFAULT_SOLVE_R, DEFAULT_SOLVE_G, DEFAULT_SOLVE_Y };

// ── SET mode ──────────────────────────────────────────────────────────────────
static bool setDone[3] = {};
static uint8_t pendingCode[3] = {};
static int8_t activeSetColor = -1;

// ── SOLVE mode ────────────────────────────────────────────────────────────────
static bool solveModeActive[3] = {};
static bool solveLocked[3] = {};
static uint8_t solveEntry[3] = {};

// ── Hold tracking ─────────────────────────────────────────────────────────────
static bool holdHandled5s = false;
static bool holdHandled10s = false;

// ── Last slider value cache ───────────────────────────────────────────────────
static uint8_t lastSliderVal[3] = {};

// ── Helpers ───────────────────────────────────────────────────────────────────
static uint8_t readSlider(uint8_t idx) {
  // int raw = analogRead(ADC_PINS[idx]);
  // // Uses runtime adcMin/adcMax set by calibrateSliders(), not compile-time constants
  // return (uint8_t)map(constrain(raw, adcMin, adcMax), adcMin, adcMax, 1, SLIDER_STEPS);

  int32_t sum = 0;
  for (int i = 0; i < 8; i++) {
    sum += analogRead(ADC_PINS[idx]);
    delay(1);
  }
  int raw = sum / 8;
  return (uint8_t)map(constrain(raw, adcMin, adcMax), adcMin, adcMax, 1, SLIDER_STEPS);
}

static void loadCode() {
  EEPROM.begin(EEPROM_SIZE);
  // Check validity flag — if not set, flash has never been written, use defaults
  if (EEPROM.read(EEPROM_ADDR_FLAG) != EEPROM_VALID_FLAG) {
    Serial.println("[Puzzle] EEPROM not initialised — using defaults");
    solveCode[IDX_R] = DEFAULT_SOLVE_R;
    solveCode[IDX_G] = DEFAULT_SOLVE_G;
    solveCode[IDX_Y] = DEFAULT_SOLVE_Y;
  } else {
    solveCode[IDX_R] = EEPROM.read(EEPROM_ADDR_R);
    solveCode[IDX_G] = EEPROM.read(EEPROM_ADDR_G);
    solveCode[IDX_Y] = EEPROM.read(EEPROM_ADDR_Y);
  }
  Serial.printf("[Puzzle] Code: R=%d G=%d Y=%d\n",
                solveCode[0], solveCode[1], solveCode[2]);
}

static void saveCode(uint8_t r, uint8_t g, uint8_t y) {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.write(EEPROM_ADDR_R, r);
  EEPROM.write(EEPROM_ADDR_G, g);
  EEPROM.write(EEPROM_ADDR_Y, y);
  EEPROM.write(EEPROM_ADDR_FLAG, EEPROM_VALID_FLAG);
  EEPROM.commit();
  solveCode[IDX_R] = r;
  solveCode[IDX_G] = g;
  solveCode[IDX_Y] = y;
  Serial.printf("[Puzzle] Code saved: R=%d G=%d Y=%d\n", r, g, y);
}

static void allLeds(uint8_t mode) {
  switchSetLed(0, mode);
  switchSetLed(1, mode);
  switchSetLed(2, mode);
}

static void clearSolveState() {
  for (int i = 0; i < 3; i++) {
    solveModeActive[i] = false;
    solveLocked[i] = false;
    solveEntry[i] = 0;
    switchSetLed(i, LED_OFF);
  }
}

static void enterIdle() {
  state = STATE_IDLE;
  clearSolveState();
  bargraphClear();
  allLeds(LED_OFF);
  Serial.println("[Puzzle] -> IDLE");
}

// static void celebrate() {
//   uint32_t t = millis();
//   allMotorsToCenter();
//   allLeds(LED_BLINK_VFAST);
//   while (millis() - t < SUCCESS_BLINK_MS) {
//     switchesUpdate();
//     delay(10);
//   }
// allMotorsToBottom();
// }
static void celebrate() {
  uint32_t t = millis();
  allMotorsToCenter();
  allLeds(LED_BLINK_VFAST);
  uint8_t specLed = 1;
  bool sweepUp = true;
  while (millis() - t < SUCCESS_BLINK_MS) {
    switchesUpdate();
    bargraphShowSpectrum(specLed);
    if (sweepUp) {
      specLed++;
      if (specLed > SLIDER_STEPS) {
        specLed = SLIDER_STEPS;
        sweepUp = false;
      }
    } else {
      specLed--;
      if (specLed < 1) {
        specLed = 1;
        sweepUp = true;
      }
    }
    delay(40);  // controls sweep speed — lower = faster
  }
  bargraphClear();
  allMotorsToBottom();
}


static void refreshLockedBargraph() {
  bargraphShowLocked(
    solveLocked[IDX_R] ? solveEntry[IDX_R] : 0,
    solveLocked[IDX_G] ? solveEntry[IDX_G] : 0,
    solveLocked[IDX_Y] ? solveEntry[IDX_Y] : 0);
}

// ── calibrateSliders ──────────────────────────────────────────────────────────
// Runs on every boot inside puzzleInit(). Drives all sliders to the physical
// bottom, reads the low ADC values, drives to the top, reads the high values,
// then sets the runtime adcMin / adcMax with a safety margin trimmed off each
// end. Bargraph sweeps left-to-right during calibration as visual feedback.
static void calibrateSliders() {
  Serial.println("[Cal] Starting auto-calibration...");
  allLeds(LED_ON);
  bargraphClear();

  // ── Step 1: drive all sliders to physical bottom ──────────────────────────
  allMotorsToBottom();
  delay(CAL_SETTLE_MS);

  // Read low values — take the minimum across all three to set a shared floor
  int rawLow = 4095;
  for (int i = 0; i < 3; i++) {
    int r = analogRead(ADC_PINS[i]);
    if (r < rawLow) rawLow = r;
    Serial.printf("[Cal] Low raw[%d] = %d\n", i, r);
  }

  // Bargraph sweeps left showing calibration in progress (red)
  for (int i = 0; i < SLIDER_STEPS; i++) {
    bargraphShowValue(i + 1, BAR_RED);
    delay(40);
  }

  // ── Step 2: drive all sliders to physical top ─────────────────────────────
  allMotorsToTop();
  delay(CAL_SETTLE_MS);

  // Read high values — take the maximum across all three to set a shared ceiling
  int rawHigh = 0;
  for (int i = 0; i < 3; i++) {
    int r = analogRead(ADC_PINS[i]);
    if (r > rawHigh) rawHigh = r;
    Serial.printf("[Cal] High raw[%d] = %d\n", i, r);
  }

  // Bargraph sweeps right showing calibration in progress (green)
  for (int i = SLIDER_STEPS; i > 0; i--) {
    bargraphShowValue(i, BAR_GREEN);
    delay(40);
  }

  // ── Step 3: apply safety margin and store runtime values ──────────────────
  adcMin = rawLow + ADC_CAL_MARGIN;
  adcMax = rawHigh - ADC_CAL_MARGIN;

  // Guard against a degenerate result (e.g. motor not connected)
  if (adcMin >= adcMax) {
    Serial.println("[Cal] WARNING: bad calibration range — falling back to defaults");
    adcMin = ADC_MIN_DEFAULT;
    adcMax = ADC_MAX_DEFAULT;
  }

  Serial.printf("[Cal] adcMin=%d  adcMax=%d\n", adcMin, adcMax);

  // ── Step 4: return sliders to bottom, clear feedback ─────────────────────
  allMotorsToBottom();
  bargraphClear();
  allLeds(LED_OFF);

  Serial.println("[Cal] Calibration complete");

  // Drain any switch edge events that accumulated during the blocking calibration sweep
  uint32_t drainUntil = millis() + 100;
  while (millis() < drainUntil) switchesUpdate();
}

// ── puzzleInit ────────────────────────────────────────────────────────────────
void puzzleInit() {
  loadCode();
  calibrateSliders();  // Auto-calibrates and leaves sliders at bottom
  enterIdle();
  Serial.println("[Puzzle] Init complete");
}

// ── puzzleUpdate ──────────────────────────────────────────────────────────────
void puzzleUpdate() {
  switchesUpdate();
  bargraphUpdate();

  uint32_t heldMs = 0;
  bool isHeld = momentaryHeld(&heldMs);

  if (isHeld) {
    // 10s factory reset (IDLE only, no switches in solve mode)
    //  if (!holdHandled10s && heldMs >= HOLD_RESET_MS && state == STATE_IDLE) {
    if (!holdHandled10s && heldMs >= HOLD_RESET_MS && (state == STATE_IDLE || state == STATE_SET)) {
      bool anySolve = solveModeActive[0] || solveModeActive[1] || solveModeActive[2];
      if (!anySolve) {
        holdHandled10s = holdHandled5s = true;
        Serial.println("[Puzzle] FACTORY RESET");
        saveCode(DEFAULT_SOLVE_R, DEFAULT_SOLVE_G, DEFAULT_SOLVE_Y);
        // run the backpack success pattern to indicate factory reset successfull
        for (int led = 1; led <= SLIDER_STEPS; led++) {
          bargraphShowSpectrum(led);
          delay(40);
        }
        for (int led = SLIDER_STEPS; led >= 1; led--) {
          bargraphShowSpectrum(led);
          delay(40);
        }
        bargraphClear();
        allLeds(LED_OFF);
        allMotorsToBottom();
        enterIdle();
        return;
      }
    }
    // 5s: enter SET mode or REVEAL
    if (!holdHandled5s && heldMs >= HOLD_SET_MS) {
      holdHandled5s = true;
      bool allSolve = solveModeActive[0] && solveModeActive[1] && solveModeActive[2];

      // REVEAL: all three switches in solve mode
      if (allSolve && state == STATE_SOLVE) {
        state = STATE_REVEAL;
        Serial.println("[Puzzle] -> REVEAL");
        allMotorsMoveToValues(
          ADC_PINS[IDX_R], solveCode[IDX_R],
          ADC_PINS[IDX_G], solveCode[IDX_G],
          ADC_PINS[IDX_Y], solveCode[IDX_Y],
          adcMin, adcMax);
        bargraphShowLocked(solveCode[IDX_R], solveCode[IDX_G], solveCode[IDX_Y]);
        delay(2000);
        momentaryPressed();  // flush any stale edge event
        return;
      }

      // SET mode: no switches in solve mode, must be IDLE
      bool anySolve = solveModeActive[0] || solveModeActive[1] || solveModeActive[2];
      if (!anySolve && state == STATE_IDLE) {
        state = STATE_SET;
        for (int i = 0; i < 3; i++) {
          setDone[i] = false;
          pendingCode[i] = 0;
        }
        activeSetColor = -1;
        allLeds(LED_BLINK_FAST);
        bargraphClear();
        Serial.println("[Puzzle] -> SET");
        return;
      }
    }
  } else {
    holdHandled5s = false;
    holdHandled10s = false;

    // Exit REVEAL on short momentary press
    if (state == STATE_REVEAL && momentaryPressed()) {
      allLeds(LED_OFF);
      allMotorsToBottom();
      bargraphClear();
      clearSolveState();
      enterIdle();
      return;
    }

    // Confirm SET code on short press when all three colors are set
    if (state == STATE_SET && setDone[0] && setDone[1] && setDone[2]
        && momentaryPressed()) {
      saveCode(pendingCode[IDX_R], pendingCode[IDX_G], pendingCode[IDX_Y]);
      celebrate();
      enterIdle();
      return;
    }
  }

  // ── State machine ──────────────────────────────────────────────────────────
  switch (state) {

    case STATE_IDLE:
      for (int i = 0; i < 3; i++) {
        if (switchPressed(i)) {
          solveModeActive[i] = true;
          switchSetLed(i, LED_BLINK_SLOW);
          state = STATE_SOLVE;
          bargraphClear();
          Serial.printf("[Puzzle] Color %d -> solve mode\n", i);
        }
      }
      break;

    case STATE_SOLVE:
      // Allow additional colors to enter solve mode
      for (int i = 0; i < 3; i++) {
        if (!solveModeActive[i] && !solveLocked[i] && switchPressed(i)) {
          solveModeActive[i] = true;
          switchSetLed(i, LED_BLINK_SLOW);
          Serial.printf("[Puzzle] Color %d -> solve mode\n", i);
        }
      }
      // Update bargraph for any moving unlocked slider
      for (int i = 0; i < 3; i++) {
        if (solveModeActive[i] && !solveLocked[i]) {
          uint8_t v = readSlider(i);
          if (v != lastSliderVal[i]) {
            lastSliderVal[i] = v;
            bargraphShowValue(v, BAR_COLORS[i]);
          }
        }
      }
      // Second press on active color locks the value
      for (int i = 0; i < 3; i++) {
        if (solveModeActive[i] && !solveLocked[i] && switchPressed(i)) {
          solveEntry[i] = readSlider(i);
          solveLocked[i] = true;
          solveModeActive[i] = false;
          switchSetLed(i, LED_ON);
          Serial.printf("[Puzzle] Color %d locked at %d\n", i, solveEntry[i]);
          refreshLockedBargraph();
          if (solveLocked[0] && solveLocked[1] && solveLocked[2]) {
            bool ok = (solveEntry[0] == solveCode[0]) && (solveEntry[1] == solveCode[1]) && (solveEntry[2] == solveCode[2]);
            state = ok ? STATE_SUCCESS : STATE_FAIL;
            Serial.println(ok ? "[Puzzle] -> SUCCESS" : "[Puzzle] -> FAIL");
          }
        }
      }
      break;

    case STATE_SET:
      for (int i = 0; i < 3; i++) {
        if (!setDone[i] && switchPressed(i)) {
          activeSetColor = i;
          state = STATE_SET_COLOR;
          allLeds(LED_OFF);
          switchSetLed(i, LED_BLINK_SLOW);
          bargraphClear();
          lastSliderVal[i] = readSlider(i);
          Serial.printf("[Puzzle] SET editing color %d\n", i);
        }
      }
      break;

    case STATE_SET_COLOR:
      {
        int i = activeSetColor;
        if (i < 0 || i > 2) {
          state = STATE_SET;
          break;
        }
        uint8_t v = readSlider(i);
        if (v != lastSliderVal[i]) {
          lastSliderVal[i] = v;
          bargraphShowValue(v, BAR_COLORS[i]);
        }
        if (switchPressed(i)) {
          uint8_t locked = readSlider(i);
          pendingCode[i] = locked;
          setDone[i] = true;
          switchSetLed(i, LED_ON);
          bargraphShowSingle(locked, BAR_COLORS[i]);
          for (int j = 0; j < 3; j++) {
            if (j != i && setDone[j] && pendingCode[j] == locked) {
              bargraphSetAlternating(locked, BAR_COLORS[i], BAR_COLORS[j]);
              break;
            }
          }
          Serial.printf("[Puzzle] SET color %d locked at %d\n", i, locked);
          activeSetColor = -1;
          state = STATE_SET;
          for (int j = 0; j < 3; j++) {
            if (!setDone[j]) switchSetLed(j, LED_BLINK_FAST);
          }
          if (setDone[0] && setDone[1] && setDone[2]) {
            bargraphShowLocked(pendingCode[0], pendingCode[1], pendingCode[2]);
            Serial.println("[Puzzle] All colors set - press momentary to confirm");
          }
        }
        break;
      }

    case STATE_SUCCESS:
      digitalWrite(PIN_TRIGGER, HIGH);
      celebrate();
      delay(TRIGGER_DURATION_MS);
      digitalWrite(PIN_TRIGGER, LOW);
      bargraphClear();
      enterIdle();
      break;

    case STATE_FAIL:
      allLeds(LED_OFF);
      bargraphClear();
      allMotorsToBottom();
      clearSolveState();
      enterIdle();
      break;

    case STATE_REVEAL:
      // Waiting for momentary press - handled in hold-released block above
      break;

    default:
      enterIdle();
      break;
  }
}
