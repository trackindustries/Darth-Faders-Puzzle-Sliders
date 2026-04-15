#include "motors.h"
#include "config.h"
#include <Adafruit_MotorShield.h>

static Adafruit_MotorShield AFMS;
static Adafruit_DCMotor* motors[4]; // index 0 unused; 1-3 active

// ── Direction defines ─────────────────────────────────────────────────────────
// Sliders are mounted with motors at the BOTTOM.
// Physical bottom (motor end) = FORWARD, Physical top = BACKWARD
#define MOTOR_DIR_BOTTOM  FORWARD
#define MOTOR_DIR_TOP     BACKWARD

void motorsInit() {
  AFMS = Adafruit_MotorShield();
  if (!AFMS.begin()) {
    Serial.println("[Motors] MotorShield not found! Check I2C.");
    while (1) delay(10);
  }
  for (int i = 1; i <= 3; i++) {
    motors[i] = AFMS.getMotor(i);
    motors[i]->setSpeed(0);
    motors[i]->run(RELEASE);
  }
  Serial.println("[Motors] OK");
}

void motorBrake(uint8_t ch) {
  if (ch < 1 || ch > 3) return;
  motors[ch]->run(BRAKE);
  motors[ch]->setSpeed(0);
}

void motorToTop(uint8_t ch) {
  if (ch < 1 || ch > 3) return;
  motors[ch]->setSpeed(MOTOR_SPEED_NORMAL);
  motors[ch]->run(MOTOR_DIR_TOP);
  delay(MOTOR_SETTLE_MS * 2);
  motorBrake(ch);
}

void motorToBottom(uint8_t ch) {
  if (ch < 1 || ch > 3) return;
  motors[ch]->setSpeed(MOTOR_SPEED_NORMAL);
  motors[ch]->run(MOTOR_DIR_BOTTOM);
  delay(MOTOR_SETTLE_MS * 2);
  motorBrake(ch);
}

void motorToCenter(uint8_t ch) {
  motorToBottom(ch);
  delay(100);
  motors[ch]->setSpeed(MOTOR_SPEED_NORMAL);
  motors[ch]->run(MOTOR_DIR_TOP);
  delay(MOTOR_SETTLE_MS);
  motorBrake(ch);
}

void allMotorsToTop() {
  for (int i = 1; i <= 3; i++) {
    motors[i]->setSpeed(MOTOR_SPEED_NORMAL);
    motors[i]->run(MOTOR_DIR_TOP);
  }
  delay(MOTOR_SETTLE_MS * 2);
  for (int i = 1; i <= 3; i++) motorBrake(i);
}

void allMotorsToBottom() {
  for (int i = 1; i <= 3; i++) {
    motors[i]->setSpeed(MOTOR_SPEED_NORMAL);
    motors[i]->run(MOTOR_DIR_BOTTOM);
  }
  delay(MOTOR_SETTLE_MS * 2);
  for (int i = 1; i <= 3; i++) motorBrake(i);
}

void allMotorsToCenter() {
  allMotorsToBottom();
  delay(100);
  for (int i = 1; i <= 3; i++) {
    motors[i]->setSpeed(MOTOR_SPEED_NORMAL);
    motors[i]->run(MOTOR_DIR_TOP);
  }
  delay(MOTOR_SETTLE_MS);
  for (int i = 1; i <= 3; i++) motorBrake(i);
}

void motorMoveToValue(uint8_t ch, uint8_t pinADC, uint8_t targetValue, int adcMin, int adcMax) {
  if (ch < 1 || ch > 3) return;
  uint32_t timeout = millis() + 5000;
  while (millis() < timeout) {
    int raw     = analogRead(pinADC);
    int current = map(constrain(raw, adcMin, adcMax), adcMin, adcMax, 1, SLIDER_STEPS);
    if (abs(current - (int)targetValue) <= 1) { motorBrake(ch); return; }
    motors[ch]->setSpeed(MOTOR_SPEED_SLOW);
    motors[ch]->run((current < (int)targetValue) ? MOTOR_DIR_TOP : MOTOR_DIR_BOTTOM);
    delay(20);
  }
  motorBrake(ch);
}

void allMotorsMoveToValues(uint8_t pinR, uint8_t valR,
                           uint8_t pinG, uint8_t valG,
                           uint8_t pinY, uint8_t valY,
                           int adcMin, int adcMax) {
  bool doneR = false, doneG = false, doneY = false;
  uint32_t timeout = millis() + 5000;
  while (millis() < timeout) {
    if (!doneR) {
      int cur = map(constrain(analogRead(pinR), adcMin, adcMax), adcMin, adcMax, 1, SLIDER_STEPS);
      if (abs(cur - (int)valR) <= 1) { motorBrake(1); doneR = true; }
      else { motors[1]->setSpeed(MOTOR_SPEED_SLOW); motors[1]->run(cur < (int)valR ? MOTOR_DIR_TOP : MOTOR_DIR_BOTTOM); }
    }
    if (!doneG) {
      int cur = map(constrain(analogRead(pinG), adcMin, adcMax), adcMin, adcMax, 1, SLIDER_STEPS);
      if (abs(cur - (int)valG) <= 1) { motorBrake(2); doneG = true; }
      else { motors[2]->setSpeed(MOTOR_SPEED_SLOW); motors[2]->run(cur < (int)valG ? MOTOR_DIR_TOP : MOTOR_DIR_BOTTOM); }
    }
    if (!doneY) {
      int cur = map(constrain(analogRead(pinY), adcMin, adcMax), adcMin, adcMax, 1, SLIDER_STEPS);
      if (abs(cur - (int)valY) <= 1) { motorBrake(3); doneY = true; }
      else { motors[3]->setSpeed(MOTOR_SPEED_SLOW); motors[3]->run(cur < (int)valY ? MOTOR_DIR_TOP : MOTOR_DIR_BOTTOM); }
    }
    if (doneR && doneG && doneY) return;
    delay(20);
  }
  // Timeout — brake any that didn't finish
  if (!doneR) motorBrake(1);
  if (!doneG) motorBrake(2);
  if (!doneY) motorBrake(3);
}
