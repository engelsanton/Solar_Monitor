#include "ina.h"
#include "config.h"

INA::INA() : ina219(INA219_ADDR), found(false) {
}

bool INA::begin() {
    found = ina219.begin();
    if (found) {
        Serial.println("INA219 initialized successfully!");
    } else {
        Serial.println("INA219 initialization failed!");
    }
    return found;
}

bool INA::isFound() {
    return found;
}

float INA::getBusVoltage() {
    if (!found) return 0.0;
    float voltage = ina219.getBusVoltage_V();
    return voltage < 0.0 ? 0.0 : voltage;
}

float INA::getCurrent() {
    if (!found) return 0.0;
    float current = ina219.getCurrent_mA();
    return current < 0.0 ? 0.0 : current;
}

float INA::getPower() {
    if (!found) return 0.0;
    float power = ina219.getPower_mW();
    return power < 0.0 ? 0.0 : power;
}
