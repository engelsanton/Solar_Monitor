#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>

class WiFiManager {
public:
    WiFiManager(const char* ssid, const char* password, IPAddress ip = IPAddress(192, 168, 4, 1));
    void begin();
    IPAddress getIP();

private:
    const char* apSSID;
    const char* apPassword;
    IPAddress apIP;
};

#endif // WIFI_MANAGER_H
