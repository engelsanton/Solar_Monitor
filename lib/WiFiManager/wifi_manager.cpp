#include "wifi_manager.h"

WiFiManager::WiFiManager(const char* ssid, const char* password, IPAddress ip) 
    : apSSID(ssid), apPassword(password), apIP(ip) {
}

void WiFiManager::begin() {
    Serial.println("Starting WiFi Access Point...");
    
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    WiFi.softAP(apSSID, apPassword);
    
    delay(100);
    
    IPAddress IP = WiFi.softAPIP();
    Serial.print("Access Point started: ");
    Serial.println(apSSID);
    Serial.print("IP Address: ");
    Serial.println(IP);
}

IPAddress WiFiManager::getIP() {
    return WiFi.softAPIP();
}
