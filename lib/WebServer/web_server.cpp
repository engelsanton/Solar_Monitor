#include "web_server.h"

WebServerManager::WebServerManager(Transistor* transistorRef, Simulation* simulationRef, INA* inaRef) 
    : server(80), transistor(transistorRef), simulation(simulationRef), ina(inaRef) {
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
    
    // Simulation endpoints
    server.on("/simulation", HTTP_POST, [this]() { this->handleSimulation(); });
    server.on("/simulation/data", HTTP_GET, [this]() { this->handleSimulationData(); });
    server.on("/simulation/panel", HTTP_POST, [this]() { this->handleSetPanel(); });
    server.on("/simulation/cell", HTTP_POST, [this]() { this->handleSetCell(); });
    server.on("/simulation/load", HTTP_POST, [this]() { this->handleSetLoad(); });
    server.on("/simulation/autotoggle", HTTP_POST, [this]() { this->handleAutoToggleLoads(); });
    
    // Real data endpoint
    server.on("/real/data", HTTP_GET, [this]() { this->handleRealData(); });
    
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
    Serial.println("[DEBUG] handleSetTransistor() - Start");
    
    if (!server.hasArg("transistor") || !server.hasArg("state")) {
        Serial.println("[DEBUG] handleSetTransistor() - Missing parameters");
        server.send(400, "application/json", "{\"error\":\"Missing parameters\"}");
        return;
    }
    
    int transistorNum = server.arg("transistor").toInt();
    int state = server.arg("state").toInt();
    Serial.print("[DEBUG] handleSetTransistor() - Transistor ");
    Serial.print(transistorNum);
    Serial.print(", State: ");
    Serial.println(state);
    
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
    // WICHTIG: GPIO-Pins aktualisieren!
    transistor->update();
    
    String response = "{\"success\":true,\"transistor\":" + String(transistorNum) + 
                     ",\"state\":" + String(state) + "}";
    
    server.sendHeader("Connection", "close");
    server.send(200, "application/json", response);
    
    Serial.print("Transistor ");
    Serial.print(transistorNum);
    Serial.print(" auf ");
    Serial.print(state ? "AN" : "AUS");
    Serial.println(" geschaltet (GPIO aktualisiert)");
}

void WebServerManager::handleGetStatus() {
    String json = "{";
    json += "\"t1\":" + String(transistor->getState1()) + ",";
    json += "\"t2\":" + String(transistor->getState2()) + ",";
    json += "\"t3\":" + String(transistor->getState3()) + ",";
    json += "\"t4\":" + String(transistor->getState4());
    json += "}";
    
    server.sendHeader("Connection", "close");
    server.send(200, "application/json", json);
}

void WebServerManager::handleNotFound() {
    server.sendHeader("Connection", "close");
    server.send(404, "text/plain", "404: Not Found");
}

void WebServerManager::handleSimulation() {
    Serial.println("[DEBUG] handleSimulation() - Start");
    
    if (!server.hasArg("action")) {
        Serial.println("[DEBUG] handleSimulation() - Missing action parameter");
        server.sendHeader("Connection", "close");
        server.send(400, "application/json", "{\"error\":\"Missing action parameter\"}");
        return;
    }
    
    String action = server.arg("action");
    
    if (action == "start") {
        int duration = 30; // default
        if (server.hasArg("duration")) {
            duration = server.arg("duration").toInt();
        }
        
        bool simulateSun = false; // default
        if (server.hasArg("simulateSun")) {
            simulateSun = server.arg("simulateSun").toInt() == 1;
        }
        
        simulation->start(duration, simulateSun);
        server.sendHeader("Connection", "close");
        server.send(200, "application/json", "{\"success\":true,\"action\":\"start\"}");
    } 
    else if (action == "stop") {
        simulation->stop();
        server.sendHeader("Connection", "close");
        server.send(200, "application/json", "{\"success\":true,\"action\":\"stop\"}");
    }
    else {
        server.sendHeader("Connection", "close");
        server.send(400, "application/json", "{\"error\":\"Invalid action\"}");
    }
}

void WebServerManager::handleSimulationData() {
    String json = simulation->getDataAsJson();
    server.sendHeader("Connection", "close");
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.send(200, "application/json", json);
}

void WebServerManager::handleSetPanel() {
    Serial.println("[DEBUG] handleSetPanel() - Start");
    
    if (!server.hasArg("panel") || !server.hasArg("state")) {
        Serial.println("[DEBUG] handleSetPanel() - Missing parameters");
        server.sendHeader("Connection", "close");
        server.send(400, "application/json", "{\"error\":\"Missing parameters\"}");
        return;
    }
    
    int panel = server.arg("panel").toInt();
    bool state = server.arg("state").toInt() == 1;
    Serial.print("[DEBUG] handleSetPanel() - Panel ");
    Serial.print(panel);
    Serial.print(", State: ");
    Serial.println(state ? "ON" : "OFF");
    
    simulation->setPanelState(panel, state);
    
    String response = "{\"success\":true,\"panel\":" + String(panel) + 
                     ",\"state\":" + String(state ? "true" : "false") + "}";
    server.sendHeader("Connection", "close");
    server.send(200, "application/json", response);
}

void WebServerManager::handleSetCell() {
    if (!server.hasArg("cell") || !server.hasArg("state")) {
        server.sendHeader("Connection", "close");
        server.send(400, "application/json", "{\"error\":\"Missing parameters\"}");
        return;
    }
    
    int cell = server.arg("cell").toInt();
    bool state = server.arg("state").toInt() == 1;
    
    simulation->setCellState(cell, state);
    
    String response = "{\"success\":true,\"cell\":" + String(cell) + 
                     ",\"state\":" + String(state ? "true" : "false") + "}";
    server.sendHeader("Connection", "close");
    server.send(200, "application/json", response);
}

void WebServerManager::handleSetLoad() {
    if (!server.hasArg("load") || !server.hasArg("state")) {
        server.sendHeader("Connection", "close");
        server.send(400, "application/json", "{\"error\":\"Missing parameters\"}");
        return;
    }
    
    String load = server.arg("load");
    bool state = server.arg("state").toInt() == 1;
    
    simulation->setLoadState(load, state);
    
    String response = "{\"success\":true,\"load\":\"" + load + 
                     "\",\"state\":" + String(state ? "true" : "false") + "}";
    server.sendHeader("Connection", "close");
    server.send(200, "application/json", response);
}

void WebServerManager::handleRealData() {
    Serial.println("[DEBUG] handleRealData() - Start");
    
    // Echte INA219-Daten abrufen
    float busV = ina->getBusVoltage();
    float currentMA = ina->getCurrent();
    float powerMW = ina->getPower();
    
    Serial.print("[DEBUG] handleRealData() - Reading: V=");
    Serial.print(busV);
    Serial.print("V, I=");
    Serial.print(currentMA);
    Serial.print("mA, P=");
    Serial.print(powerMW);
    Serial.println("mW");
    
    // JSON-Response erstellen
    String json = "{";
    json += "\"voltage\":" + String(busV, 2) + ",";
    json += "\"current\":" + String(currentMA, 2) + ",";
    json += "\"power\":" + String(powerMW, 2);
    json += "}";
    
    server.sendHeader("Connection", "close");
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.send(200, "application/json", json);
    Serial.println("[DEBUG] handleRealData() - Data sent to client");
}

void WebServerManager::handleAutoToggleLoads() {
    if (!server.hasArg("enable")) {
        server.send(400, "application/json", "{\"success\":false,\"error\":\"Missing enable parameter\"}");
        return;
    }
    
    bool enable = server.arg("enable") == "1" || server.arg("enable") == "true";
    simulation->setAutoToggleLoads(enable);
    
    String json = "{\"success\":true,\"autoToggleLoads\":";
    json += enable ? "true" : "false";
    json += "}";
    
    server.sendHeader("Connection", "close");
    server.send(200, "application/json", json);
}
