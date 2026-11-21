#include <Arduino.h>
#include "FanController.h"

// Fan controller instance (PWM pin 16, TACH pin 4, 2 pulses per revolution)
FanController fan(16, 4, 2.0f);

// Fan speed state and 1-second telemetry timer
uint8_t currentDuty = 0;
unsigned long previousMillis = 0;
const unsigned long interval = 1000;

// Temporary serial speed override (0-100%) until the screen and sensor subsystem is set up
void handleSerialSpeedCommand() {
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim();

        if (input.length() == 0) {
            return;
        }

        if (input.startsWith("rpm ") || input.startsWith("RPM ")) {
            float target = input.substring(4).toFloat();
            fan.setTargetRPM(target);
            fan.setPIDEnabled(true);
            Serial.printf("[Terminal] PID enabled with target RPM: %.1f\n", target);
            return;
        }

        bool isNumber = true;
        size_t startIdx = (input.charAt(0) == '-' || input.charAt(0) == '+') ? 1 : 0;
        if (startIdx >= input.length()) {
            isNumber = false;
        }
        for (size_t i = startIdx; i < input.length(); i++) {
            if (!isDigit(input.charAt(i))) {
                isNumber = false;
                break;
            }
        }

        int requestedDuty = input.toInt();
        if (isNumber && requestedDuty >= 0 && requestedDuty <= 100) {
            fan.setPIDEnabled(false);
            currentDuty = static_cast<uint8_t>(requestedDuty);
            fan.setSpeed(currentDuty);
            Serial.printf("[Terminal] Manual fan duty cycle set to %u%%\n", currentDuty);
        } else {
            Serial.println("[Terminal] Enter 0-100 for duty cycle or 'rpm <value>' for PID.");
        }
    }
}

// Initialize serial communication and fan hardware
void setup() {
    Serial.begin(115200);
    delay(1500);
    Serial.println("Fan Subsystem Initializing...");

    fan.begin();
    fan.setPIDGains(0.05f, 0.02f, 0.005f);
    fan.setSpeed(currentDuty);

    Serial.printf("Fan set to %u%% duty cycle. Type 0-100 or 'rpm <target>'.\n\n", currentDuty);

}

// Process temporary serial speed commands and print RPM telemetry every second
void loop() {
    handleSerialSpeedCommand();

    unsigned long currentMillis = millis();

    // Periodic RPM logging
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        float currentRPM = fan.getRPM();
        if (fan.isPIDEnabled()) { 
          currentDuty = fan.updatePID(currentRPM);
        }

        Serial.printf("[Fan Telemetry] RPM: %.2f | Target RPM: %.1f | Duty: %u%%\n", currentRPM, fan.getTargetRPM(), currentDuty);
    }
}