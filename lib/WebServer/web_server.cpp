#include "web_server.h"

WebServerManager::WebServerManager(Transistor* transistorRef) 
    : server(80), transistor(transistorRef) {
}

void WebServerManager::begin() {
    // LittleFS initialisieren
    if (!LittleFS.begin(true)) {
        Serial.println("LittleFS Mount fehlgeschlagen!");
        return;
    }
    Serial.println("LittleFS erfolgreich gemountet");
    
    // Routen definieren
    server.on("/", [this]() { this->handleRoot(); });
    server.on("/set", HTTP_POST, [this]() { this->handleSetTransistor(); });
    server.on("/status", HTTP_GET, [this]() { this->handleGetStatus(); });
    server.onNotFound([this]() { this->handleNotFound(); });
    
    server.begin();
    Serial.println("Webserver gestartet auf Port 80");
}

void WebServerManager::handleClient() {
    server.handleClient();
}

void WebServerManager::handleRoot() {
    File file = LittleFS.open("/index.html", "r");
    if (!file) {
        server.send(404, "text/plain", "index.html nicht gefunden!");
        return;
    }
    
    server.streamFile(file, "text/html");
    file.close();
}

void WebServerManager::handleSetTransistor() {
    if (!server.hasArg("transistor") || !server.hasArg("state")) {
        server.send(400, "application/json", "{\"error\":\"Missing parameters\"}");
        return;
    }
    
    int transistorNum = server.arg("transistor").toInt();
    int state = server.arg("state").toInt();
    
    // Aktuellen Zustand abrufen
    int t1 = transistor->getState1();
    int t2 = transistor->getState2();
    int t3 = transistor->getState3();
    int t4 = transistor->getState4();
    
    // Gewünschten Transistor aktualisieren
    switch(transistorNum) {
        case 1: t1 = state; break;
        case 2: t2 = state; break;
        case 3: t3 = state; break;
        case 4: t4 = state; break;
        default:
            server.send(400, "application/json", "{\"error\":\"Invalid transistor number\"}");
            return;
    }
    
    // Neuen Zustand setzen
    transistor->setState(t1, t2, t3, t4);
    
    String response = "{\"success\":true,\"transistor\":" + String(transistorNum) + 
                     ",\"state\":" + String(state) + "}";
    server.send(200, "application/json", response);
    
    Serial.print("Transistor ");
    Serial.print(transistorNum);
    Serial.print(" auf ");
    Serial.println(state ? "AN" : "AUS");
}

void WebServerManager::handleGetStatus() {
    String json = "{";
    json += "\"t1\":" + String(transistor->getState1()) + ",";
    json += "\"t2\":" + String(transistor->getState2()) + ",";
    json += "\"t3\":" + String(transistor->getState3()) + ",";
    json += "\"t4\":" + String(transistor->getState4());
    json += "}";
    
    server.send(200, "application/json", json);
}

void WebServerManager::handleNotFound() {
    server.send(404, "text/plain", "404: Not Found");
}
