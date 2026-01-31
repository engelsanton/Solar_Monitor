#include "ina.h"
#include "config.h"

INA::INA() : ina219(INA219_ADDR), found(false) {
}

bool INA::begin() {
    found = ina219.begin();
    if (found) {
        Serial.println("INA219 erfolgreich initialisiert!");
    } else {
        Serial.println("INA219-Initialisierung fehlgeschlagen!");
    }
    return found;
}

bool INA::isFound() {
    return found;
}

float INA::getBusVoltage() {
    if (!found) return 0.0;
    return ina219.getBusVoltage_V();
}

float INA::getShuntVoltage() {
    if (!found) return 0.0;
    return ina219.getShuntVoltage_mV();
}

float INA::getCurrent() {
    if (!found) return 0.0;
    return ina219.getCurrent_mA();
}

float INA::getPower() {
    if (!found) return 0.0;
    return ina219.getPower_mW();
}
