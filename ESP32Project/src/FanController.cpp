//Mumina implementation Code

#include "FanController.h"

// Static instance pointer for interrupt access
FanController* FanController::_instance = nullptr;

// Constructor: initialize pins, pulse count, and timing
FanController::FanController(uint8_t pwmPin, uint8_t tachPin, float pulsesPerRevolution) {
    _pwmPin = pwmPin;
    _tachPin = tachPin;
    _pulsesPerRevolution = (pulsesPerRevolution > 0.0f) ? pulsesPerRevolution : 2.0f;
    _tachPulseCount = 0;
    _lastMillis = 0;
    _lastPidMillis = 0;
}

// Interrupt handler: increment tachometer pulse counter
void IRAM_ATTR FanController::handleTachInterrupt() {
    if (_instance != nullptr) {
        // Noise filter (added by Member B): the PWM signal causes very short spikes
        // on the TACH wire. A real TACH pulse stays LOW for milliseconds and pulses
        // are more than 1 ms apart, so only count pulses that pass both checks.
        static unsigned long lastPulseMicros = 0;
        unsigned long now = micros();
        if (digitalRead(_instance->_tachPin) == LOW && now - lastPulseMicros > 1000) {
            lastPulseMicros = now;
            unsigned long count = _instance->_tachPulseCount;
            _instance->_tachPulseCount = count + 1;
        }
    }
}

// Initialize PWM output, tachometer interrupt, and PID timer
void FanController::begin() {
    _instance = this;
    unsigned long now = millis();
    _lastMillis = now;
    _lastPidMillis = now;
    _tachPulseCount = 0;
    _integral = 0.0f;
    _prevError = 0.0f;

    ledcAttach(_pwmPin, _pwmFreq, _pwmResolution);
    pinMode(_tachPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(_tachPin), handleTachInterrupt, FALLING);
}

// Set fan duty cycle directly (0-100%)
void FanController::setSpeed(uint8_t dutyCyclePercent) {
    _currentDuty = static_cast<uint8_t>(constrain(dutyCyclePercent, 0, 100));
    uint32_t pwmValue = map(_currentDuty, 0, 100, 0, 255);
    ledcWrite(_pwmPin, pwmValue);
}

// Calculate and return fan RPM once per second
float FanController::getRPM() {
    unsigned long currentMillis = millis();
    unsigned long elapsed = currentMillis - _lastMillis;

    if (elapsed >= 1000) {
        noInterrupts();
        unsigned long pulses = _tachPulseCount;
        _tachPulseCount = 0;
        interrupts();

        _lastMillis = currentMillis;

        float revolutions = static_cast<float>(pulses) / _pulsesPerRevolution;
        float seconds = static_cast<float>(elapsed) / 1000.0f;
        return (revolutions / seconds) * 60.0f;
    }

    return 0.0f;
}

// Configure proportional, integral, and derivative gains
void FanController::setPIDGains(float kp, float ki, float kd) {
    _kp = kp;
    _ki = ki;
    _kd = kd;
}

// Set target fan speed in RPM
void FanController::setTargetRPM(float targetRPM) {
    _targetRPM = (targetRPM > 0.0f) ? targetRPM : 0.0f;
}

// Enable or disable closed-loop PID control
void FanController::setPIDEnabled(bool enabled) {
    _pidEnabled = enabled;
    _integral = 0.0f;
    _prevError = 0.0f;
    _lastPidMillis = millis();
}

bool FanController::isPIDEnabled() const {
    return _pidEnabled;
}

float FanController::getTargetRPM() const {
    return _targetRPM;
}

// Compute PID duty cycle (0-100%) from measured RPM and apply it to the fan
uint8_t FanController::updatePID(float measuredRPM) {
    unsigned long now = millis();
    float dt = (now - _lastPidMillis) / 1000.0f;
    if (dt <= 0.0f) {
        dt = 0.001f;
    }
    _lastPidMillis = now;

    if (!_pidEnabled) {
        return _currentDuty;
    }

    // Stop the fan if target RPM is 0
    if (_targetRPM <= 0.0f) {
        _integral = 0.0f;
        _prevError = 0.0f;
        setSpeed(0);
        return 0;
    }

    float error = _targetRPM - measuredRPM;
    _integral = constrain(_integral + error * dt, -2000.0f, 2000.0f);
    float derivative = (error - _prevError) / dt;
    _prevError = error;

    float output = (_kp * error) + (_ki * _integral) + (_kd * derivative);
    uint8_t nextDuty = static_cast<uint8_t>(constrain(output, 0.0f, 100.0f));
    setSpeed(nextDuty);
    return nextDuty;
}