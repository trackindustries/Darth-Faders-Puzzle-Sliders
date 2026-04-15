#pragma once
#include <stdint.h>


void motorsInit();
void motorBrake(uint8_t channel);
void motorToTop(uint8_t channel);
void motorToBottom(uint8_t channel);
void motorToCenter(uint8_t channel);
void allMotorsToTop();
void allMotorsToBottom();
void allMotorsToCenter();
// Closed-loop: drives motor until ADC reads targetValue (1-24). 3s timeout.
void motorMoveToValue(uint8_t channel, uint8_t pinADC, uint8_t targetValue, int adcMin, int adcMax);
// Closed-loop: drives all three motors simultaneously to their target values.
void allMotorsMoveToValues(uint8_t pinR, uint8_t valR,
                           uint8_t pinG, uint8_t valG,
                           uint8_t pinY, uint8_t valY,
                           int adcMin, int adcMax);