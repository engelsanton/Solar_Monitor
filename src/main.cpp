#include <Arduino.h>
#include <Wire.h>
#include "config.h"
#include "ina.h"
#include "oled.h"
#include "transistor.h"
#include "wifi_manager.h"
#include "web_server.h"
#include "simulation.h"

INA ina;
OLED oled;
Transistor transistor;
Simulation simulation(&ina);
WiFiManager wifiManager("Solar_Monitor", "12345678");
WebServerManager webServer(&transistor, &simulation, &ina);

bool oled_found = false;
bool ina219_found = false;

void scanI2C() {
  Serial.println("I2C Scan starten...");
  int nDevices = 0;
  
  for (byte i = 1; i < 127; i++) {
    Wire.beginTransmission(i);
    byte error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("I2C Gerät gefunden bei Adresse: 0x");
      if (i < 16) Serial.print("0");
      Serial.println(i, HEX);
      
      if (i == OLED_ADDR) {
        oled_found = true;
        Serial.println("  -> OLED gefunden!");
      }
      if (i == INA219_ADDR) {
        ina219_found = true;
        Serial.println("  -> INA219 gefunden!");
      }
      nDevices++;
    }
  }
  
  Serial.print("Gesamt gefunden: ");
  Serial.println(nDevices);
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n\n=== ESP32-S3 INA219 Test ===");
  
  // Transistoren initialisieren
  transistor.begin();
  
  Wire.begin(OLED_SDA, OLED_SCL);
  
  // I2C Scan durchführen
  scanI2C();
  
  // OLED initialisieren
  if (oled_found) {
    if (oled.begin()) {
      oled.showBootScreen();
    } else {
      oled_found = false;
    }
  } else {
    Serial.println("OLED nicht auf I2C gefunden!");
  }
  
  // INA219 initialisieren
  if (ina219_found) {
    if (ina.begin()) {
      // Erfolgreich initialisiert
    } else {
      ina219_found = false;
    }
  } else {
    Serial.println("INA219 nicht auf I2C gefunden!");
  }
  
  delay(1000);
  
  // WiFi Access Point starten
  wifiManager.begin();
  
  // Simulation initialisieren
  simulation.begin();
  
  // Webserver starten
  webServer.begin();
  
  Serial.println("System bereit!");
}

void loop() {
  // Webserver Anfragen verarbeiten
  webServer.handleClient();
  
  // Simulation aktualisieren
  simulation.update();
  
  // OLED aktualisieren
  if (oled_found) {
    float busV = 0.0, shuntV = 0.0, currentMA = 0.0, powerMW = 0.0;
    
    if (ina219_found) {
      busV = ina.getBusVoltage();
      shuntV = ina.getShuntVoltage();
      currentMA = ina.getCurrent();
      powerMW = ina.getPower();
    }
    
    oled.showStatus(
      transistor.getState1(), 
      transistor.getState2(), 
      transistor.getState3(), 
      transistor.getState4(),
      busV, currentMA,
      ina219_found
    );
  } else {
    Serial.println("OLED nicht verfügbar!");
  }
  
  delay(100); // Faster loop for smoother simulation
}
