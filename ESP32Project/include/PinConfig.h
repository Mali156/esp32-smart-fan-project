// Member B: all pins in one place (ESP32-S3 Super Mini)

#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

// Fan (Member A)
// PWM: GPIO 16 is only a solder pad on this board, so GPIO 10 is used.
// GPIO 10 -> resistor -> 2N3904 BASE, COLLECTOR -> fan PWM (blue), EMITTER -> GND
#define FAN_PWM_PIN   10
#define FAN_TACH_PIN  4    // fan TACH (yellow), 10k pull-up to 3V3

// I2C: OLED and SHT30 share these two pins (same as the tutorial)
#define I2C_SDA_PIN   13
#define I2C_SCL_PIN   12

// On-board RGB LED (GPIO 21 on our board, not 48 as in the tutorial)
#define LED_PIN       21

// Buttons K1-K4 on the OLED module (pressed = connected to GND)
#define BTN_UP_PIN    5    // K1
#define BTN_DOWN_PIN  6    // K2
#define BTN_MODE_PIN  7    // K3
#define BTN_PAGE_PIN  8    // K4

#endif
