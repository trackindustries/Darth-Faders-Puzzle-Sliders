#pragma once
#include <stdint.h>

#define LED_OFF         0
#define LED_ON          1
#define LED_BLINK_SLOW  2
#define LED_BLINK_FAST  3
#define LED_BLINK_VFAST 4

void switchesInit();
void switchesUpdate();                  // Call every loop()
bool switchPressed(uint8_t index);      // 0=Red 1=Green 2=Yellow - edge detect
bool momentaryPressed();                // Edge detect
bool momentaryHeld(uint32_t* holdMs);  // True while held; fills elapsed ms
void switchSetLed(uint8_t index, uint8_t mode);