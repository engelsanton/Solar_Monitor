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
    float voltage = ina219.getBusVoltage_V();
    return voltage < 0.0 ? 0.0 : voltage;  // Ignoriere negative Werte
}

float INA::getShuntVoltage() {
    if (!found) return 0.0;
    float voltage = ina219.getShuntVoltage_mV();
    return voltage < 0.0 ? 0.0 : voltage;  // Ignoriere negative Werte
}

float INA::getCurrent() {
    if (!found) return 0.0;
    float current = ina219.getCurrent_mA();
    return current < 0.0 ? 0.0 : current;  // Ignoriere negative Werte
}

float INA::getPower() {
    if (!found) return 0.0;
    float power = ina219.getPower_mW();
    return power < 0.0 ? 0.0 : power;  // Ignoriere negative Werte
}
