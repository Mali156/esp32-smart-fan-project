// Member B: data shared by all modules
// The variables live in main.cpp. Any module can read them after including this file.

#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include <Arduino.h>

// Fan modes
#define MODE_MANUAL  0
#define MODE_PID     1
#define MODE_AUTO    2   // for Member C (auto temperature control)

// Fan data
extern int   fanMode;        // MODE_MANUAL / MODE_PID / MODE_AUTO
extern int   fanDuty;        // 0-100 %
extern float fanRPM;         // measured speed
extern float targetRPM;      // PID target speed

// Sensor data (from SHT30)
extern float temperature;    // degrees C
extern float humidity;       // %
extern bool  sensorOK;       // false = sensor not working, do not use the values

// Wi-Fi data (Member C fills these in)
extern bool   wifiConnected;
extern String wifiIP;
extern bool   autoModeReady; // Member C: set to true when auto mode works

// Fan control functions (in main.cpp). Buttons, serial and web should all use these.
void setFanDuty(int duty);       // manual mode, 0-100 %
void setTargetRPM(float rpm);    // PID mode
void changeMode();               // MANUAL -> PID -> (AUTO) -> MANUAL

#endif
