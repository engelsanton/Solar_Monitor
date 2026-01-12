
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include "config.h"
#include "ina219.h"

// WLAN-Konfiguration
#define WIFI_SSID "ESP32_SolarMonitor"
#define WIFI_PASS "12345678"

WebServer server(80);

void handleRoot() {
    File file = LittleFS.open("/index.html", "r");
    if (!file) {
        server.send(500, "text/plain", "index.html nicht gefunden");
        return;
    }
    String html = file.readString();
    file.close();
    server.send(200, "text/html", html);
}


bool sp1State = false;
void handleSP1() {
    sp1State = !sp1State;
    digitalWrite(TRANSISTOR_PIN, sp1State ? HIGH : LOW);
    server.send(200, "text/plain", sp1State ? "on" : "off");
}

void handleOn() {
    digitalWrite(TRANSISTOR_PIN, HIGH);
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "Transistor EIN");
}

void handleOff() {
    digitalWrite(TRANSISTOR_PIN, LOW);
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "Transistor AUS");
}


void handleNotFound() {
    server.send(404, "text/plain", "404: Not Found");
}

void handleApiData() {
    float voltage = ina219_getBusVoltage();
    float current = ina219_getCurrent();
    
    // Negative Werte auf 0 setzen
    if (voltage < 0) voltage = 0;
    if (current < 0) current = 0;
    
    // Leistung in mW (Spannung in V * Strom in mA = mW)
    float power = voltage * current;
    
    String json = "{";
    json += "\"voltage\":" + String(voltage, 2) + ",";
    json += "\"current\":" + String(current, 2) + ",";
    json += "\"solar_total\":" + String(power, 2) + ",";
    json += "\"battery_soc\":50";
    json += "}";
    
    server.send(200, "application/json", json);
}


void wifiWebserverInit() {
    if (!LittleFS.begin()) {
        Serial.println("LittleFS mount fehlgeschlagen, versuche Format...");
        LittleFS.format();
        if (!LittleFS.begin()) {
            Serial.println("LittleFS konnte nicht gestartet werden!");
            return;
        }
    }

    WiFi.softAP(WIFI_SSID, WIFI_PASS);
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("WLAN gestartet. IP: ");
    Serial.println(myIP);

    pinMode(TRANSISTOR_PIN, OUTPUT);
    digitalWrite(TRANSISTOR_PIN, LOW);

    server.on("/", HTTP_GET, handleRoot);
    server.on("/api/data", HTTP_GET, handleApiData);
    server.on("/sp1", HTTP_POST, handleSP1);
    server.on("/on", HTTP_POST, handleOn);
    server.on("/off", HTTP_POST, handleOff);
    server.onNotFound(handleNotFound);
    server.begin();
    Serial.println("Webserver gestartet.");
}

void wifiWebserverLoop() {
    server.handleClient();
}
