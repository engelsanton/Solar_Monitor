#include "wifi_manager.h"

WiFiManager::WiFiManager(const char* ssid, const char* password) 
    : apSSID(ssid), apPassword(password) {
}

void WiFiManager::begin() {
    Serial.println("Starte WiFi Access Point...");
    
    // Access Point starten
    WiFi.mode(WIFI_AP);
    WiFi.softAP(apSSID, apPassword);
    
    delay(100);
    
    IPAddress IP = WiFi.softAPIP();
    Serial.print("Access Point gestartet: ");
    Serial.println(apSSID);
    Serial.print("IP Adresse: ");
    Serial.println(IP);
}

bool WiFiManager::isConnected() {
    return WiFi.softAPgetStationNum() > 0;
}

IPAddress WiFiManager::getIP() {
    return WiFi.softAPIP();
}
