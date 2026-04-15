#include "bargraph.h"
#include "config.h"
#include <Adafruit_LEDBackpack.h>

static Adafruit_24bargraph bar;

static uint8_t  altPos   = 0;
static uint8_t  altC1    = BAR_OFF;
static uint8_t  altC2    = BAR_OFF;
static bool     altPhase = false;
static uint32_t altLast  = 0;

static uint8_t lastFillValue = 0;
static uint8_t lastFillColor = BAR_OFF;
static bool    inFillMode    = false;

static uint8_t lockedR = 0, lockedG = 0, lockedY = 0;
static bool    showingLocked = false;

void bargraphInit() {
  if (!bar.begin(BARGRAPH_ADDR)) {
    Serial.printf("[Bargraph] ERROR: not found at 0x%02X — check SDA/SCL and power.\n", BARGRAPH_ADDR);
    while (1) delay(10);
  }
  bar.clear();
  bar.writeDisplay();
  Serial.println("[Bargraph] OK");
}

void bargraphClear() {
  inFillMode    = false;
  showingLocked = false;
  altPos = 0; altC1 = BAR_OFF; altC2 = BAR_OFF;
  bar.clear();
  bar.writeDisplay();
}

void bargraphShowValue(uint8_t value, uint8_t color) {
  inFillMode    = true;
  showingLocked = false;
  lastFillValue = value;
  lastFillColor = color;
  bar.clear();
  for (uint8_t i = 0; i < value && i < SLIDER_STEPS; i++) bar.setBar(i, color);
  bar.writeDisplay();
}

void bargraphShowSingle(uint8_t pos, uint8_t color) {
  bar.clear();
  if (pos >= 1 && pos <= SLIDER_STEPS) bar.setBar(pos - 1, color);
  bar.writeDisplay();
}

void bargraphShowLocked(uint8_t rVal, uint8_t gVal, uint8_t yVal) {
  lockedR = rVal; lockedG = gVal; lockedY = yVal;
  showingLocked = true;
  inFillMode    = false;
  altPos = 0; altC1 = BAR_OFF; altC2 = BAR_OFF;
  bar.clear();

  if (rVal > 0 && rVal == gVal && gVal == yVal) {
    altPos = rVal; altC1 = BAR_RED; altC2 = BAR_GREEN;
    bar.setBar(rVal - 1, BAR_RED);
  } else if (rVal > 0 && gVal > 0 && rVal == gVal) {
    altPos = rVal; altC1 = BAR_RED; altC2 = BAR_GREEN;
    bar.setBar(rVal - 1, BAR_RED);
    if (yVal > 0) bar.setBar(yVal - 1, BAR_YELLOW);
  } else if (rVal > 0 && yVal > 0 && rVal == yVal) {
    altPos = rVal; altC1 = BAR_RED; altC2 = BAR_YELLOW;
    bar.setBar(rVal - 1, BAR_RED);
    if (gVal > 0) bar.setBar(gVal - 1, BAR_GREEN);
  } else if (gVal > 0 && yVal > 0 && gVal == yVal) {
    altPos = gVal; altC1 = BAR_GREEN; altC2 = BAR_YELLOW;
    bar.setBar(gVal - 1, BAR_GREEN);
    if (rVal > 0) bar.setBar(rVal - 1, BAR_RED);
  } else {
    if (rVal > 0) bar.setBar(rVal - 1, BAR_RED);
    if (gVal > 0) bar.setBar(gVal - 1, BAR_GREEN);
    if (yVal > 0) bar.setBar(yVal - 1, BAR_YELLOW);
  }
  bar.writeDisplay();
}

void bargraphSetAlternating(uint8_t pos, uint8_t c1, uint8_t c2) {
  altPos = pos; altC1 = c1; altC2 = c2;
  altPhase = false; altLast = millis();
}

void bargraphUpdate() {
  if (altPos == 0 || altC1 == BAR_OFF) return;
  if (millis() - altLast < ALT_COLOR_MS) return;
  altLast  = millis();
  altPhase = !altPhase;
  uint8_t color = altPhase ? altC2 : altC1;

  if (inFillMode) {
    bar.clear();
    for (uint8_t i = 0; i < lastFillValue && i < SLIDER_STEPS; i++)
      bar.setBar(i, lastFillColor);
    bar.setBar(altPos - 1, color);
    bar.writeDisplay();
  } else if (showingLocked) {
    bar.clear();
    if (lockedR > 0 && lockedR == lockedG && lockedG == lockedY) {
      bar.setBar(altPos - 1, color);
    } else if (lockedR > 0 && lockedR == lockedG) {
      bar.setBar(altPos - 1, color);
      if (lockedY > 0) bar.setBar(lockedY - 1, BAR_YELLOW);
    } else if (lockedR > 0 && lockedR == lockedY) {
      bar.setBar(altPos - 1, color);
      if (lockedG > 0) bar.setBar(lockedG - 1, BAR_GREEN);
    } else if (lockedG > 0 && lockedG == lockedY) {
      bar.setBar(altPos - 1, color);
      if (lockedR > 0) bar.setBar(lockedR - 1, BAR_RED);
    }
    bar.writeDisplay();
  } else {
    bar.setBar(altPos - 1, color);
    bar.writeDisplay();
  }
}

void bargraphShowSpectrum(uint8_t fillTo) {
  static const uint8_t colors[3] = { BAR_RED, BAR_GREEN, BAR_YELLOW };
  bar.clear();
  for (uint8_t i = 0; i < fillTo && i < SLIDER_STEPS; i++) {
    bar.setBar(i, colors[i % 3]);
  }
  bar.writeDisplay();
}