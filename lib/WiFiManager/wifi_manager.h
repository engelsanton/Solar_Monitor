#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>

class WiFiManager {
public:
    WiFiManager(const char* ssid, const char* password);
    void begin();
    bool isConnected();
    IPAddress getIP();

private:
    const char* apSSID;
    const char* apPassword;
};

#endif // WIFI_MANAGER_H
