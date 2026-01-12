
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_INA219.h>
#include "main.h"


Adafruit_INA219 ina219;


// Transistor an GPIO4
const int TRANSISTOR_PIN = 4;
bool transistorState = false;
unsigned long lastToggle = 0;
const unsigned long toggleInterval = 5000; // 5 Sekunden

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

  pinMode(TRANSISTOR_PIN, OUTPUT);
  digitalWrite(TRANSISTOR_PIN, LOW);
  lastToggle = millis();
}



void loop() {
  // INA219 Messung und Ausgabe jede Sekunde
  float busVoltage_V = ina219.getBusVoltage_V();
  float current_mA = ina219.getCurrent_mA();

  Serial.print("Busspannung: ");
  Serial.print(busVoltage_V, 3);
  Serial.print(" V\tStrom: ");
  Serial.print(current_mA, 3);
  Serial.println(" mA");

  // Transistor an GPIO4 alle 5 Sekunden toggeln
  unsigned long now = millis();
  if (now - lastToggle >= toggleInterval) {
    transistorState = !transistorState;
    digitalWrite(TRANSISTOR_PIN, transistorState ? HIGH : LOW);
    Serial.print("Transistor GPIO4 ist jetzt: ");
    Serial.println(transistorState ? "HIGH" : "LOW");
    lastToggle = now;
  }

  delay(1000); // 1 Sekunde Pause für die Messwertausgabe
}
