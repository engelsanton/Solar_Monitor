#include <WiFi.h>
#include <WebServer.h>
#include "config.h"

// WLAN-Konfiguration
#define WIFI_SSID "ESP32_SolarMonitor"
#define WIFI_PASS "12345678"

WebServer server(80);

void handleRoot() {
    String html = "<html><head><title>Transistor Control</title></head><body>";
    html += "<h1>Transistor Steuerung</h1>";
    html += "<form action='/on' method='post'><button type='submit'>Transistor EIN</button></form>";
    html += "<form action='/off' method='post'><button type='submit'>Transistor AUS</button></form>";
    html += "</body></html>";
    server.send(200, "text/html", html);
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

void wifiWebserverInit() {
    WiFi.softAP(WIFI_SSID, WIFI_PASS);
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("WLAN gestartet. IP: ");
    Serial.println(myIP);

    pinMode(TRANSISTOR_PIN, OUTPUT);
    digitalWrite(TRANSISTOR_PIN, LOW);

    server.on("/", HTTP_GET, handleRoot);
    server.on("/on", HTTP_POST, handleOn);
    server.on("/off", HTTP_POST, handleOff);
    server.onNotFound(handleNotFound);
    server.begin();
    Serial.println("Webserver gestartet.");
}

void wifiWebserverLoop() {
    server.handleClient();
}
