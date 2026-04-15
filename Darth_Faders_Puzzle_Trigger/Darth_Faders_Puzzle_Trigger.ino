/*
 * Darth Faders Puzzle
 * Hardware:
 *   Adafruit Feather RP2040 (#4884)
 *   3x Motorized Slide Potentiometers (#5466)
 *   DC Motor + Stepper FeatherWing (#2927)
 *   Terminal Block Breakout FeatherWing (#2926)
 *   Bi-Color 24-Bar Bargraph I2C Backpack (#1721)
 *   3x Retro Step Switches with LED (#5517) on PB86 boards (#5631)
 *   1x Momentary Button (#1445)
 *
 * Board: Adafruit Feather RP2040 (via Adafruit RP2040 board package)
 *
 * Required Libraries (install via Library Manager):
 *   - Adafruit Motor Shield V2
 *   - Adafruit LED Backpack Library
 *   - Adafruit GFX Library
 *   - EEPROM (built into Arduino-Pico core — no install needed)
 */

#include <Wire.h>
#include <EEPROM.h>
#include "config.h"
#include "motors.h"
#include "bargraph.h"
#include "switches.h"
#include "puzzle.h"

void setup() {
Serial.begin(115200);
while (!Serial && millis() < 3000);
delay(100);
Serial.println("[DarthFaders] Boot");
  Wire.begin();
  motorsInit();
  bargraphInit();
  switchesInit();
  puzzleInit();
  Serial.println("[DarthFaders] Ready");
}


void loop() {
puzzleUpdate();
}
