#include <Arduino.h>
#include <Wire.h>

#include "main.h"
#include "config.h"

#include "transistor.h"
#include "ina219.h"

#include "wifi_webserver.h"
#include "oled.h"


bool transistorState = false;



void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(1); }
  Serial.println("INA219 Test auf ESP32");

  ina219Init();
  transistorInit();
  wifiWebserverInit();
  oledInit();
}






void loop() {
  float v = ina219_getBusVoltage();
  float i = ina219_getCurrent();
  Serial.print("Busspannung: ");
  Serial.print(v, 3);
  Serial.print(" V\tStrom: ");
  Serial.print(i, 3);
  Serial.println(" mA");
  oledShowValues(v, i);
  wifiWebserverLoop();
  delay(1000);
}

