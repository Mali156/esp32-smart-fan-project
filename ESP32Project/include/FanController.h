//Mumina: PWM, TACH, and PID loop

// Header guard to prevent multiple inclusion
#ifndef FAN_CONTROLLER_H
#define FAN_CONTROLLER_H

#include <Arduino.h>

// Controls 4-pin PWM fan speed, reads tachometer RPM, and runs closed-loop PID control
class FanController {
public:
    // Constructor and public control/telemetry methods
    FanController(uint8_t pwmPin, uint8_t tachPin, float pulsesPerRevolution = 2.0f);
    void begin();
    void setSpeed(uint8_t dutyCyclePercent);
    float getRPM();

    // PID closed-loop control methods
    void setPIDGains(float kp, float ki, float kd);
    void setTargetRPM(float targetRPM);
    void setPIDEnabled(bool enabled);
    bool isPIDEnabled() const;
    float getTargetRPM() const;
    uint8_t updatePID(float measuredRPM);

private:
    // Tachometer interrupt handler
    static void IRAM_ATTR handleTachInterrupt();

    // Pin and pulse configuration
    uint8_t _pwmPin;
    uint8_t _tachPin;
    float _pulsesPerRevolution;

    // PWM frequency and resolution settings
    uint32_t _pwmFreq = 25000;
    uint8_t _pwmResolution = 8;

    // Pulse counter, timing state, and active instance pointer
    volatile unsigned long _tachPulseCount;
    unsigned long _lastMillis;
    static FanController* _instance;

    // PID state and tuning parameters
    bool _pidEnabled = false;
    float _kp = 0.05f;
    float _ki = 0.02f;
    float _kd = 0.005f;
    float _targetRPM = 0.0f;
    float _integral = 0.0f;
    float _prevError = 0.0f;
    unsigned long _lastPidMillis = 0;
    uint8_t _currentDuty = 0;
};

#endif