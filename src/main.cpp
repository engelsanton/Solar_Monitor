#include <Arduino.h>
#include <Wire.h>

#include "main.h"
#include "config.h"

#include "transistor.h"
#include "ina219.h"
#include "wifi_webserver.h"


bool transistorState = false;


void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(1); }
  Serial.println("INA219 Test auf ESP32");

  ina219Init();
  transistorInit();
  wifiWebserverInit();
}




void loop() {
  ina219PrintValues();
  wifiWebserverLoop();
  delay(1000);
}

