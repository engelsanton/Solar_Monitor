#include "wifi_manager.h"

WiFiManager::WiFiManager(const char* ssid, const char* password) 
    : apSSID(ssid), apPassword(password) {
}

void WiFiManager::begin() {
    Serial.println("Starting WiFi Access Point...");
    
    WiFi.mode(WIFI_AP);
    WiFi.softAP(apSSID, apPassword);
    
    delay(100);
    
    IPAddress IP = WiFi.softAPIP();
    Serial.print("Access Point started: ");
    Serial.println(apSSID);
    Serial.print("IP Address: ");
    Serial.println(IP);
}

bool WiFiManager::isConnected() {
    return WiFi.softAPgetStationNum() > 0;
}

IPAddress WiFiManager::getIP() {
    return WiFi.softAPIP();
}
