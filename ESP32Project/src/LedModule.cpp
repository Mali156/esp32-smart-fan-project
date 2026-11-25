// Member B: RGB LED effect that follows the fan speed
// Uses rgbLedWrite(), which is built into the ESP32 Arduino core (no extra library).
//   - fan stopped : dim white
//   - fan running : colour goes blue (slow) -> purple -> red (fast),
//                   and the LED breathes faster when the fan is faster

#include <Arduino.h>
#include "LedModule.h"
#include "PinConfig.h"
#include "SharedData.h"

#define FAN_MAX_RPM  4200   // our fan runs at about 4000 RPM at 100 % duty

// Breathing effect variables
int breathe = 20;      // current brightness (20-255)
int breatheDir = 1;    // 1 = getting brighter, -1 = getting darker

void initLed() {
  // Self-test: red, green, blue, then off
  rgbLedWrite(LED_PIN, 50, 0, 0);
  delay(300);
  rgbLedWrite(LED_PIN, 0, 50, 0);
  delay(300);
  rgbLedWrite(LED_PIN, 0, 0, 50);
  delay(300);
  rgbLedWrite(LED_PIN, 0, 0, 0);
  Serial.println("LED ready");
}

void updateLed() {
  // Fan stopped: dim white light
  if (fanDuty == 0) {
    rgbLedWrite(LED_PIN, 10, 10, 10);
    return;
  }

  int rpm = constrain((int)fanRPM, 0, FAN_MAX_RPM);

  // 1. Colour from speed: slow = blue, fast = red
  int red = map(rpm, 0, FAN_MAX_RPM, 0, 255);
  int blue = 255 - red;

  // 2. Breathing speed from fan speed
  int step = map(rpm, 0, FAN_MAX_RPM, 2, 12);
  breathe = breathe + breatheDir * step;
  if (breathe >= 255) {
    breathe = 255;
    breatheDir = -1;
  }
  if (breathe <= 20) {
    breathe = 20;
    breatheDir = 1;
  }

  // 3. Show it (colour x brightness)
  rgbLedWrite(LED_PIN, red * breathe / 255, 0, blue * breathe / 255);
}
