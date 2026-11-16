#include "FanController.h"
#include "SensorModule.h"
#include "WebServerModule.h"
// Member D / Integration: Main setup & non-blocking loop
// Instantiate using test pins
FanController fan(16, 4);
SensorModule sensors;
WebServerModule webServer(&fan);

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("Folder structure test initialized!");
    
    fan.begin();
    sensors.begin();
    webServer.begin();
}

void loop() {
    // Empty loop for compile test
}