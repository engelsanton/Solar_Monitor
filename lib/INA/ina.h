#ifndef INA_H
#define INA_H

#include <Arduino.h>
#include <Adafruit_INA219.h>

class INA {
public:
    INA();
    bool begin();
    bool isFound();
    float getBusVoltage();
    float getShuntVoltage();
    float getCurrent();
    float getPower();

private:
    Adafruit_INA219 ina219;
    bool found;
};

#endif // INA_H
