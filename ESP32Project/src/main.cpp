#include <Arduino.h>
#include <Wire.h>
#include "PinConfig.h"
#include "SharedData.h"
#include "FanController.h"   // Member A
#include "SensorModule.h"    // Member B
#include "DisplayModule.h"   // Member B
#include "LedModule.h"       // Member B
#include "ButtonModule.h"    // Member B

// Fan controller (Member A): PWM and TACH pins from PinConfig.h, 2 pulses per revolution
FanController fan(FAN_PWM_PIN, FAN_TACH_PIN, 2.0f);

// ---------- Shared data (see SharedData.h) ----------
int   fanMode = MODE_MANUAL;
int   fanDuty = 0;
float fanRPM = 0;
float targetRPM = 1200;

float temperature = 0;
float humidity = 0;
bool  sensorOK = false;

bool   wifiConnected = false;
String wifiIP = "";
bool   autoModeReady = false;

#define MAX_TARGET_RPM 5000   // highest PID target allowed

// ---------- Timers ----------
// RPM is read every 1010 ms (not 1000): fan.getRPM() returns 0 if it is
// called a little earlier than 1000 ms after the last call.
unsigned long lastRpmTime = 0;
unsigned long lastSensorTime = 0;
unsigned long lastLedTime = 0;
unsigned long lastDisplayTime = 0;

// ---------- Fan control functions ----------

// Manual mode: set duty 0-100 %
void setFanDuty(int duty) {
  duty = constrain(duty, 0, 100);
  fanMode = MODE_MANUAL;
  fan.setPIDEnabled(false);
  fanDuty = duty;
  fan.setSpeed(fanDuty);
  Serial.printf("Manual duty: %d%%\n", fanDuty);
}

// PID mode: set target RPM
void setTargetRPM(float rpm) {
  rpm = constrain(rpm, 0, MAX_TARGET_RPM);
  targetRPM = rpm;
  fan.setTargetRPM(targetRPM);
  if (fanMode != MODE_PID) {
    fanMode = MODE_PID;
    fan.setPIDEnabled(true);
  }
  Serial.printf("PID target: %.0f RPM\n", targetRPM);
}

// MANUAL -> PID -> AUTO (only when Member C has finished it) -> MANUAL
void changeMode() {
  if (fanMode == MODE_MANUAL) {
    setTargetRPM(targetRPM);
  } else if (fanMode == MODE_PID && autoModeReady) {
    fanMode = MODE_AUTO;
    fan.setPIDEnabled(false);
    Serial.println("Mode: AUTO");
  } else {
    setFanDuty(fanDuty);
  }
}

// ---------- I2C scanner (for checking wiring) ----------
// Runs once at start-up and with the serial command "scan".
// OLED should be at 0x3C, SHT30 at 0x44 (or 0x45).
void scanI2C() {
  Serial.println("Scanning I2C bus (SDA=13, SCL=12)...");
  int found = 0;
  for (int address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      Serial.printf("  Found device at 0x%02X", address);
      if (address == 0x3C) Serial.print("  <- OLED");
      if (address == 0x44 || address == 0x45) Serial.print("  <- SHT30");
      Serial.println();
      found++;
    }
  }
  if (found == 0) {
    Serial.println("  No I2C devices found! Check 3V3, GND, SDA (GPIO 13) and SCL (GPIO 12).");
  }
}

// ---------- Serial commands (from Member A, Member D will extend them) ----------
// "0"-"100" = manual duty, "rpm 1500" = PID target, "scan" = check the I2C bus
void handleSerialSpeedCommand() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.length() == 0) {
      return;
    }

    if (input == "scan") {
      scanI2C();
      return;
    }

    if (input.startsWith("rpm ") || input.startsWith("RPM ")) {
      setTargetRPM(input.substring(4).toFloat());
      return;
    }

    bool isNumber = true;
    for (size_t i = 0; i < input.length(); i++) {
      if (!isDigit(input.charAt(i))) {
        isNumber = false;
      }
    }

    int requestedDuty = input.toInt();
    if (isNumber && requestedDuty >= 0 && requestedDuty <= 100) {
      setFanDuty(requestedDuty);
    } else {
      Serial.println("Enter 0-100 for duty cycle, 'rpm <value>' for PID, or 'scan'.");
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1500);
  Serial.println("Smart Fan Initializing...");

  // Member A: fan
  fan.begin();
  fan.setPIDGains(0.05f, 0.02f, 0.005f);
  fan.setSpeed(fanDuty);

  // Member B: OLED + SHT30 share the I2C pins (SDA=13, SCL=12)
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  scanI2C();
  initDisplay();
  initSensor();
  initLed();
  initButtons();

  Serial.println("Ready. Type 0-100, 'rpm <target>', or 'scan'.\n");
}

void loop() {
  handleSerialSpeedCommand();
  checkButtons();

  unsigned long now = millis();

  // Every 1 s: read RPM, run PID, print data
  if (now - lastRpmTime >= 1010) {
    lastRpmTime = now;
    fanRPM = fan.getRPM();
    if (fanMode == MODE_PID) {
      fanDuty = fan.updatePID(fanRPM);
    }
    Serial.printf("[Telemetry] RPM: %.0f | Target: %.0f | Duty: %d%% | T: %.1f C | RH: %.1f %% | Sensor: %s\n",
                  fanRPM, targetRPM, fanDuty, temperature, humidity, sensorOK ? "OK" : "ERROR");
  }

  // Every 2 s: read temperature and humidity
  if (now - lastSensorTime >= 2000) {
    lastSensorTime = now;
    readSensor();
  }

  // Every 30 ms: LED animation
  if (now - lastLedTime >= 30) {
    lastLedTime = now;
    updateLed();
  }

  // Every 200 ms: refresh OLED
  if (now - lastDisplayTime >= 200) {
    lastDisplayTime = now;
    updateDisplay();
  }
}
