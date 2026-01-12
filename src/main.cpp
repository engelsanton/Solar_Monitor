
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_INA219.h>
#include "main.h"


Adafruit_INA219 ina219;

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(1); }
  Serial.println("INA219 Test auf ESP32");

  // I2C Pins für ESP32 setzen
  Wire.begin(21, 22); // SDA=21, SCL=22

  if (!ina219.begin()) {
    Serial.println("Fehler: INA219 nicht gefunden!");
    while (1) { delay(10); }
  }
  ina219.setCalibration_16V_400mA();
  Serial.println("INA219 initialisiert.");
}


void loop() {
  float busVoltage_V = ina219.getBusVoltage_V();
  float current_mA = ina219.getCurrent_mA();

  Serial.print("Busspannung: ");
  Serial.print(busVoltage_V, 3);
  Serial.print(" V\tStrom: ");
  Serial.print(current_mA, 3);
  Serial.println(" mA");

  delay(1000);
}
