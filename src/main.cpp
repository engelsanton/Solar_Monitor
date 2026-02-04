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

void scanI2C() {
  Serial.println("Starting I2C scan...");
  int deviceCount = 0;
  
  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("I2C device found at address: 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      
      if (address == OLED_ADDR) {
        Serial.println("  -> OLED detected");
      }
      if (address == INA219_ADDR) {
        Serial.println("  -> INA219 detected");
      }
      deviceCount++;
    }
  }
  
  Serial.print("Total devices found: ");
  Serial.println(deviceCount);
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n\n=== Solar Monitor System ===");
  
  // Initialize transistors
  transistor.begin();
  
  // Initialize I2C
  Wire.begin(OLED_SDA, OLED_SCL);
  
  // Scan I2C bus
  scanI2C();
  
  // Initialize OLED
  if (oled.begin()) {
    oled.showBootScreen();
  } else {
    Serial.println("OLED initialization failed");
  }
  
  // Initialize INA219
  if (!ina.begin()) {
    Serial.println("INA219 initialization failed");
  }
  
  delay(1000);
  
  // Start WiFi Access Point
  wifiManager.begin();
  
  // Initialize simulation
  simulation.begin();
  
  // Start web server
  webServer.begin();
  
  Serial.println("System ready!");
}

void loop() {
  // Handle web server requests
  webServer.handleClient();
  
  // Update simulation
  simulation.update();
  
  // Update OLED display
  if (oled.isFound()) {
    float voltage = 0.0;
    float current = 0.0;
    
    if (ina.isFound()) {
      voltage = ina.getBusVoltage();
      current = ina.getCurrent();
    }
    
    oled.showStatus(
      transistor.getState1(), 
      transistor.getState2(), 
      transistor.getState3(), 
      transistor.getState4(),
      voltage,
      current,
      ina.isFound()
    );
  }
  
  delay(100);
}
