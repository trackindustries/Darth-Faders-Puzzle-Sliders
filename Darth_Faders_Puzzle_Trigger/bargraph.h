#pragma once
#include <stdint.h>

#define BAR_OFF    0
#define BAR_RED    1
#define BAR_YELLOW 2
#define BAR_GREEN  3


void bargraphInit();
void bargraphClear();
void bargraphShowValue(uint8_t value, uint8_t color);   // Fill LEDs 1..value
void bargraphShowSingle(uint8_t pos, uint8_t color);    // Light single LED at pos (1-24)
void bargraphShowLocked(uint8_t redVal, uint8_t greenVal, uint8_t yellowVal);
void bargraphSetAlternating(uint8_t pos, uint8_t c1, uint8_t c2);
void bargraphUpdate(); // Must be called every loop()
void bargraphShowSpectrum(uint8_t fillTo);