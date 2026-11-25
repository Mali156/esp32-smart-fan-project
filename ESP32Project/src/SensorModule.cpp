// Member B: SHT30 temperature and humidity sensor
// Library: Sensirion arduino-sht (from the tutorial's useful links)

#include <Wire.h>
#include <SHTSensor.h>
#include "SensorModule.h"
#include "SharedData.h"

// AUTO_DETECT finds the sensor at 0x44 or 0x45 (also works for similar Sensirion sensors).
// Note: the sensor is searched only at start-up. If it was not connected then,
// fix the wiring and press RESET.
SHTSensor sht(SHTSensor::AUTO_DETECT);

void initSensor() {
  if (sht.init(Wire)) {
    Serial.println("SHT30 ready");
  } else {
    Serial.println("SHT30 not found! Check wiring, then press RESET. Type 'scan' to check the I2C bus.");
  }
}

void readSensor() {
  if (sht.readSample()) {
    // Reading worked
    temperature = sht.getTemperature();
    humidity = sht.getHumidity();
    sensorOK = true;
  } else {
    // Reading failed (sensor unplugged or broken)
    sensorOK = false;
    Serial.println("SHT30 read failed!");
  }
}
