#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_INA219.h>
#include "config.h"

static Adafruit_INA219 ina219(INA219_ADDR);

void ina219Init() {
    Wire.begin(I2C_SDA, I2C_SCL);
    if (!ina219.begin()) {
        Serial.println("Fehler: INA219 nicht gefunden!");
        while (1) { delay(10); }
    }
    ina219.setCalibration_16V_400mA();
    Serial.println("INA219 initialisiert.");
}

void ina219PrintValues() {
    float busVoltage_V = ina219.getBusVoltage_V();
    float current_mA = ina219.getCurrent_mA();
    Serial.print("Busspannung: ");
    Serial.print(busVoltage_V, 3);
    Serial.print(" V\tStrom: ");
    Serial.print(current_mA, 3);
    Serial.println(" mA");
}
