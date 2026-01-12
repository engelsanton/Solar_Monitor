#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 WROOM32 gestartet!");
}

void loop() {
  Serial.println("Running on ESP32 WROOM32");
  delay(1000);
}
