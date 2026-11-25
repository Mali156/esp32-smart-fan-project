// Member B: the four buttons K1-K4 on the OLED module
// INPUT_PULLUP means: not pressed = HIGH, pressed = LOW.

#include <Arduino.h>
#include "ButtonModule.h"
#include "PinConfig.h"
#include "SharedData.h"
#include "DisplayModule.h"

#define DEBOUNCE_TIME 200   // ignore presses closer than 200 ms (button bounce)

const int buttonPins[4] = { BTN_UP_PIN, BTN_DOWN_PIN, BTN_MODE_PIN, BTN_PAGE_PIN };
int lastState[4] = { HIGH, HIGH, HIGH, HIGH };
unsigned long lastPressTime[4] = { 0, 0, 0, 0 };

void initButtons() {
  for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  Serial.println("Buttons ready");
}

// Returns true once when button i is pressed down
bool buttonPressed(int i) {
  bool pressed = false;
  int state = digitalRead(buttonPins[i]);

  if (state == LOW && lastState[i] == HIGH && millis() - lastPressTime[i] > DEBOUNCE_TIME) {
    pressed = true;
    lastPressTime[i] = millis();
  }
  lastState[i] = state;
  return pressed;
}

void checkButtons() {
  // K1: UP
  if (buttonPressed(0)) {
    if (fanMode == MODE_MANUAL) setFanDuty(fanDuty + 10);
    if (fanMode == MODE_PID)    setTargetRPM(targetRPM + 100);
  }

  // K2: DOWN
  if (buttonPressed(1)) {
    if (fanMode == MODE_MANUAL) setFanDuty(fanDuty - 10);
    if (fanMode == MODE_PID)    setTargetRPM(targetRPM - 100);
  }

  // K3: MODE
  if (buttonPressed(2)) {
    changeMode();
  }

  // K4: PAGE
  if (buttonPressed(3)) {
    nextPage();
  }
}
