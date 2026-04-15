#include "switches.h"
#include "config.h"
#include <Arduino.h>

static const uint8_t SW_PINS[3]  = { PIN_SW_RED,    PIN_SW_GREEN,    PIN_SW_YELLOW  };
static const uint8_t LED_PINS[3] = { PIN_LED_RED,   PIN_LED_GREEN,   PIN_LED_YELLOW };
static const uint32_t DEBOUNCE_MS = 30;

static bool     swState[3]      = {};
static bool     swPrev[3]       = {};
static bool     swEdge[3]       = {};
static uint32_t swLastChange[3] = {};

static bool     btnState      = false;
static bool     btnPrev       = false;
static bool     btnEdge       = false;
static uint32_t btnLastChange = 0;
static uint32_t btnHoldStart  = 0;

static uint8_t  ledMode[3]      = { LED_OFF, LED_OFF, LED_OFF };
static bool     ledBlinkOn[3]   = {};
static uint32_t ledLastBlink[3] = {};

void switchesInit() {
  for (int i = 0; i < 3; i++) {
    pinMode(SW_PINS[i],  INPUT_PULLUP);
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);
  }
  pinMode(PIN_MOMENT_BTN, INPUT_PULLUP);
  pinMode(PIN_TRIGGER, OUTPUT);
  digitalWrite(PIN_TRIGGER, LOW);
  Serial.println("[Switches] OK");
}

void switchesUpdate() {
  uint32_t now = millis();

  for (int i = 0; i < 3; i++) {
    bool reading = (digitalRead(SW_PINS[i]) == LOW);
    if (reading != swState[i] && (now - swLastChange[i] >= DEBOUNCE_MS)) {
      swState[i]      = reading;
      swLastChange[i] = now;
      if (swState[i] && !swPrev[i]) swEdge[i] = true;
    }
    swPrev[i] = swState[i];

    uint32_t period = 0;
    switch (ledMode[i]) {
      case LED_OFF: digitalWrite(LED_PINS[i], LOW);  continue;
      case LED_ON:  digitalWrite(LED_PINS[i], HIGH); continue;
      case LED_BLINK_SLOW:  period = BLINK_SLOW_MS;      break;
      case LED_BLINK_FAST:  period = BLINK_FAST_MS;      break;
      case LED_BLINK_VFAST: period = BLINK_VERY_FAST_MS; break;
    }
    if (now - ledLastBlink[i] >= period) {
      ledLastBlink[i] = now;
      ledBlinkOn[i]   = !ledBlinkOn[i];
      digitalWrite(LED_PINS[i], ledBlinkOn[i] ? HIGH : LOW);
    }
  }

  bool btnReading = (digitalRead(PIN_MOMENT_BTN) == LOW);
  if (btnReading != btnState && (now - btnLastChange >= DEBOUNCE_MS)) {
    btnState      = btnReading;
    btnLastChange = now;
    if (btnState && !btnPrev) { btnEdge = true; btnHoldStart = now; }
  }
  btnPrev = btnState;
}

bool switchPressed(uint8_t index) {
  if (index > 2 || !swEdge[index]) return false;
  swEdge[index] = false;
  return true;
}

bool momentaryPressed() {
  if (!btnEdge) return false;
  btnEdge = false;
  return true;
}

bool momentaryHeld(uint32_t* holdMs) {
  if (btnState) { *holdMs = millis() - btnHoldStart; return true; }
  *holdMs = 0;
  return false;
}

void switchSetLed(uint8_t index, uint8_t mode) {
  if (index > 2) return;
  ledMode[index] = mode;
  if (mode == LED_OFF) digitalWrite(LED_PINS[index], LOW);
  if (mode == LED_ON)  digitalWrite(LED_PINS[index], HIGH);
}