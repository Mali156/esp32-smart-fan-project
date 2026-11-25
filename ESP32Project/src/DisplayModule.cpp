// Member B: OLED display (SSD1306, 128x64)
// Same setup as the tutorial demo

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DisplayModule.h"
#include "SharedData.h"

// OLED settings
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1    // No reset pin
#define OLED_ADDRESS  0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int displayPage = 0;   // 0 = fan page, 1 = environment page

void initDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("OLED allocation failed!");
    return;   // keep running without the screen
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(10, 20);
  display.println("SMART FAN");
  display.display();
  Serial.println("OLED ready");
}

void nextPage() {
  displayPage = displayPage + 1;
  if (displayPage > 1) {
    displayPage = 0;
  }
}

// Page 1: fan information
void drawFanPage() {
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("FAN   Mode: ");
  if (fanMode == MODE_MANUAL) display.println("MANUAL");
  if (fanMode == MODE_PID)    display.println("PID");
  if (fanMode == MODE_AUTO)   display.println("AUTO");

  // RPM in big text
  display.setTextSize(2);
  display.setCursor(0, 14);
  display.print((int)fanRPM);
  display.setTextSize(1);
  display.print(" RPM");

  // Target RPM (only in PID mode)
  display.setCursor(0, 34);
  if (fanMode == MODE_PID) {
    display.print("Target: ");
    display.print((int)targetRPM);
  }

  // Duty as text and as a bar
  display.setCursor(0, 46);
  display.print("Duty: ");
  display.print(fanDuty);
  display.print("%");
  display.drawRect(0, 56, 128, 8, SSD1306_WHITE);
  display.fillRect(0, 56, fanDuty * 128 / 100, 8, SSD1306_WHITE);
}

// Page 2: temperature, humidity, Wi-Fi
void drawEnvPage() {
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("ENVIRONMENT");

  if (sensorOK) {
    display.setTextSize(2);
    display.setCursor(0, 14);
    display.print(temperature, 1);
    display.println(" C");
    display.setCursor(0, 34);
    display.print(humidity, 1);
    display.println(" %");
  } else {
    display.setTextSize(2);
    display.setCursor(0, 20);
    display.println("SENSOR");
    display.println("ERROR");
  }

  display.setTextSize(1);
  display.setCursor(0, 56);
  if (wifiConnected) {
    display.print("WiFi: ");
    display.print(wifiIP);
  } else {
    display.print("WiFi: offline");
  }
}

void updateDisplay() {
  display.clearDisplay();
  if (displayPage == 0) {
    drawFanPage();
  } else {
    drawEnvPage();
  }
  display.display();
}
