#include "simulation.h"
#include <math.h>

Simulation::Simulation() {
    running = false;
    startTime = 0;
    lastUpdateTime = 0;
    durationSeconds = 30;
    simCurrentHour = 6.0;
    ina = nullptr;
    
    // Initialize states
    for (int i = 0; i < 4; i++) {
        panels[i] = false;
        cells[i] = false;
        loads[i] = false;
    }
    
    // Panel 1 und Cell 1 standardmäßig an
    panels[0] = true;
    cells[0] = true;
    
    // Load watts: light, fridge, ac, washing
    loadWatts[0] = 10;
    loadWatts[1] = 150;
    loadWatts[2] = 2000;
    loadWatts[3] = 500;
    
    // Initialize data
    currentData.current = 0.0;
    currentData.voltage = 0.0;
    currentData.power = 0.0;
    currentData.batteryLevel = 75.0;
    currentData.load = 0.0;
    currentData.hour = 6;
    currentData.minute = 0;
    currentData.isDaytime = true;
}

void Simulation::begin() {
    Serial.println("Simulation initialized");
}

void Simulation::setINA(INA* inaRef) {
    ina = inaRef;
    Serial.println("[DEBUG] Simulation::setINA() - INA reference set");
}

void Simulation::start(int durationSeconds) {
    Serial.println("[DEBUG] Simulation::start() - Start");
    
    this->durationSeconds = durationSeconds;
    startTime = millis();
    lastUpdateTime = startTime;
    simCurrentHour = 6.0; // Start at 6 AM
    running = true;
    currentData.batteryLevel = 75.0;
    
    Serial.print("[DEBUG] Simulation::start() - Duration: ");
    Serial.print(durationSeconds);
    Serial.println(" seconds");
}

void Simulation::stop() {
    Serial.println("[DEBUG] Simulation::stop() - Stopping simulation");
    running = false;
    simCurrentHour = 6.0;
    Serial.println("[DEBUG] Simulation::stop() - Simulation stopped");
}

void Simulation::update() {
    if (!running) return;
    
    unsigned long currentTime = millis();
    unsigned long elapsed = (currentTime - startTime) / 1000; // seconds
    
    // Check if simulation should stop (24 hours completed)
    if (elapsed >= durationSeconds) {
        stop();
        return;
    }
    
    // Calculate current simulation hour (0-24)
    float hoursPerSecond = 24.0 / (float)durationSeconds;
    simCurrentHour = fmod((elapsed * hoursPerSecond + 6.0), 24.0);
    
    // Update time display (immer Tag, da Benutzer Licht selbst steuern)
    currentData.hour = (int)simCurrentHour;
    currentData.minute = (int)((simCurrentHour - currentData.hour) * 60.0);
    currentData.isDaytime = true; // Immer Tag, Licht wird manuell gesteuert
    
    // Calculate simulation data
    calculateSolarData();
    calculateBattery();
    calculateLoad();
    
    lastUpdateTime = currentTime;
}

void Simulation::calculateSolarData() {
    // Count active panels
    int activePanels = 0;
    for (int i = 0; i < 4; i++) {
        if (panels[i]) activePanels++;
    }
    
    // Wenn INA verfügbar ist, echte Werte mit Berechnungsvorschrift umrechnen
    if (ina != nullptr && ina->isFound()) {
        float measCurrent = ina->getCurrent(); // mA
        float measVoltage = ina->getBusVoltage(); // V
        float measPower = ina->getPower(); // mW
        
        // Berechnungsvorschrift anwenden:
        // I_Real = I_Mess × 125
        // U_Real = U_Mess × 160
        // P_Real = P_Mess × 10000
        currentData.current = measCurrent * 125.0; // mA
        currentData.voltage = measVoltage * 160.0 * 1000.0; // V × 160 in mV
        currentData.power = measPower * 10000.0; // mW
        
        // Skalierung mit aktiven Panels (1-4)
        float panelFactor = activePanels > 0 ? activePanels / 4.0 : 0.0;
        currentData.current *= panelFactor;
        currentData.voltage *= panelFactor;
        currentData.power *= panelFactor;
    } else {
        // Fallback: Wenn kein INA vorhanden
        currentData.current = 0.0;
        currentData.voltage = 0.0;
        currentData.power = 0.0;
    }
}

void Simulation::calculateBattery() {
    // Count active cells
    int activeCells = 0;
    for (int i = 0; i < 4; i++) {
        if (cells[i]) activeCells++;
    }
    
    if (activeCells == 0) return;
    
    // Count active panels
    int activePanels = 0;
    for (int i = 0; i < 4; i++) {
        if (panels[i]) activePanels++;
    }
    
    // Battery charge/discharge calculation basierend auf echter Leistung
    float batteryChange = 0.0;
    
    if (activePanels > 0 && activeCells > 0) {
        // Charging: Basierend auf tatsächlicher Solarleistung
        // Batterie-Kapazität angenommen: 100Wh
        // Ladeeffizienz: 85%
        float chargingPowerW = (currentData.power / 1000.0) * 0.85; // mW zu W, mit Effizienz
        float hoursPerSecond = 24.0 / (float)durationSeconds;
        float chargePercentPerUpdate = (chargingPowerW / 100.0) * hoursPerSecond * 100.0; // % Ladung
        batteryChange = chargePercentPerUpdate;
    } else if (activeCells > 0) {
        // Discharging: Basierend auf Last
        float dischargePowerW = currentData.load;
        float hoursPerSecond = 24.0 / (float)durationSeconds;
        float dischargePercentPerUpdate = (dischargePowerW / 100.0) * hoursPerSecond * 100.0;
        batteryChange = -dischargePercentPerUpdate;
    }
    
    currentData.batteryLevel += batteryChange;
    
    // Clamp between 0 and 100
    if (currentData.batteryLevel < 0) currentData.batteryLevel = 0;
    if (currentData.batteryLevel > 100) currentData.batteryLevel = 100;
}

void Simulation::calculateLoad() {
    float totalLoad = 0.0;
    
    // Summiere aktive Lasten
    for (int i = 0; i < 4; i++) {
        if (loads[i]) {
            totalLoad += loadWatts[i];
        }
    }
    
    // Realistische Variation: +/- 5%
    float variation = totalLoad * (random(-5, 5) / 100.0);
    totalLoad += variation;
    
    if (totalLoad < 0) totalLoad = 0;
    
    currentData.load = totalLoad;
}

float Simulation::getSolarIntensity(float hour) {
    // No sun at night (18:00 - 6:00)
    if (hour < 6.0 || hour >= 18.0) {
        return 0.0;
    }
    
    // Parabolic curve peaking at noon
    float hoursSinceSunrise = hour - 6.0;
    float normalizedTime = hoursSinceSunrise / 12.0; // 0 to 1
    return sin(normalizedTime * PI); // 0 to 1 to 0
}

bool Simulation::isRunning() {
    return running;
}

SimulationData Simulation::getCurrentData() {
    return currentData;
}

String Simulation::getDataAsJson() {
    String json = "{";
    json += "\"current\":" + String(currentData.current, 1) + ",";
    json += "\"voltage\":" + String(currentData.voltage, 1) + ",";
    json += "\"power\":" + String(currentData.power, 1) + ",";
    json += "\"batteryLevel\":" + String(currentData.batteryLevel, 1) + ",";
    json += "\"load\":" + String(currentData.load, 1) + ",";
    json += "\"hour\":" + String(currentData.hour) + ",";
    json += "\"minute\":" + String(currentData.minute) + ",";
    json += "\"isDaytime\":" + String(currentData.isDaytime ? "true" : "false") + ",";
    json += "\"isRunning\":" + String(running ? "true" : "false");
    json += "}";
    return json;
}

float Simulation::getProgress() {
    if (!running) return 0.0;
    
    unsigned long elapsed = (millis() - startTime) / 1000;
    return (float)elapsed / (float)durationSeconds;
}

void Simulation::setPanelState(int panel, bool state) {
    Serial.print("[DEBUG] Simulation::setPanelState() - Panel ");
    Serial.print(panel);
    Serial.print(", State: ");
    Serial.println(state ? "ON" : "OFF");
    
    if (panel >= 1 && panel <= 4) {
        panels[panel - 1] = state;
    }
}

void Simulation::setCellState(int cell, bool state) {
    Serial.print("[DEBUG] Simulation::setCellState() - Cell ");
    Serial.print(cell);
    Serial.print(", State: ");
    Serial.println(state ? "ON" : "OFF");
    
    if (cell >= 1 && cell <= 4) {
        cells[cell - 1] = state;
    }
}

void Simulation::setLoadState(String load, bool state) {
    Serial.print("[DEBUG] Simulation::setLoadState() - Load ");
    Serial.print(load);
    Serial.print(", State: ");
    Serial.println(state ? "ON" : "OFF");
    
    int index = -1;
    if (load == "light") index = 0;
    else if (load == "fridge") index = 1;
    else if (load == "ac") index = 2;
    else if (load == "washing") index = 3;
    
    if (index >= 0) {
        loads[index] = state;
    }
}
