// Member B: SHT30 temperature and humidity sensor

#ifndef SENSOR_MODULE_H
#define SENSOR_MODULE_H

void initSensor();   // call once in setup()
void readSensor();   // call every 2 seconds

#endif
