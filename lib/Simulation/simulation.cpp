#include "simulation.h"
#include <math.h>

Simulation::Simulation() {
    running = false;
    startTime = 0;
    lastUpdateTime = 0;
    durationSeconds = 30;
    simCurrentHour = 0.0;
    ina = nullptr;
    simulateSun = false;
    
    // Initialize states
    for (int i = 0; i < 4; i++) {
        panels[i] = false;
        cells[i] = false;
    }
    
    for (int i = 0; i < 8; i++) {
        loads[i] = false;
    }
    
    // Panel 1 und Cell 1 standardmäßig an
    panels[0] = true;
    cells[0] = true;
    
    // Load watts: light, fridge, ac, washing, wallbox, heatpump, dishwasher, tv
    loadWatts[0] = 10;      // Light
    loadWatts[1] = 150;     // Fridge
    loadWatts[2] = 2000;    // AC
    loadWatts[3] = 500;     // Washing Machine
    loadWatts[4] = 11000;   // Wallbox E-Auto
    loadWatts[5] = 3000;    // Heat Pump
    loadWatts[6] = 2000;    // Dishwasher
    loadWatts[7] = 300;     // TV
    
    // Initialize data
    currentData.current = 0.0;
    currentData.voltage = 0.0;
    currentData.power = 0.0;
    currentData.batteryLevel = 25.0; // Start bei 25%
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

void Simulation::start(int durationSeconds, bool simulateSun) {
    Serial.println("[DEBUG] Simulation::start() - Start");
    
    this->durationSeconds = durationSeconds;
    this->simulateSun = simulateSun;
    startTime = millis();
    lastUpdateTime = startTime;
    simCurrentHour = 0.0; // Start at 00:00 (midnight)
    running = true;
    currentData.batteryLevel = 25.0; // Start bei 25%
    
    Serial.print("[DEBUG] Simulation::start() - Duration: ");
    Serial.print(durationSeconds);
    Serial.print(" seconds, Simulate Sun: ");
    Serial.println(simulateSun ? "ON" : "OFF");
    Serial.println(" seconds");
}

void Simulation::stop() {
    Serial.println("[DEBUG] Simulation::stop() - Stopping simulation");
    running = false;
    simCurrentHour = 0.0;
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
    
    // Calculate current simulation hour (0-24), starting at 00:00
    float hoursPerSecond = 24.0 / (float)durationSeconds;
    simCurrentHour = fmod((elapsed * hoursPerSecond), 24.0);
    
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
    
    // Wenn simulateSun aktiviert ist oder kein INA vorhanden
    if (simulateSun || (ina == nullptr || !ina->isFound())) {
        // Synthetische Sonnensimulation basierend auf Tageszeit
        float solarIntensity = getSolarIntensity(simCurrentHour);
        
        // Basis-Werte für ein 2.5kWp Panel bei voller Sonne
        // Annahme: 2500W peak, ca. 10V, 250A bei Spitzenlast
        // Skaliert mit Sonnenintensität (0-1)
        float baseCurrent = 250.0 * solarIntensity; // mA
        float baseVoltage = 10000.0 * solarIntensity; // mV (10V)
        
        currentData.current = baseCurrent;
        currentData.voltage = baseVoltage;
        currentData.power = (baseVoltage * baseCurrent) / 1000.0; // mW
        
        // Skalierung mit aktiven Panels (1-4)
        float panelFactor = activePanels > 0 ? activePanels / 4.0 : 0.0;
        currentData.current *= panelFactor;
        currentData.voltage *= panelFactor;
        currentData.power *= panelFactor;
        
    } else {
        // Echte INA219-Werte mit Berechnungsvorschrift
        float measCurrent = ina->getCurrent(); // mA
        float measVoltage = ina->getBusVoltage(); // V
        
        // Berechnungsvorschrift anwenden:
        // I_Real = I_Mess × 125
        // U_Real = U_Mess × 160
        // P_Real = U_Real * I_Real (berechnet, nicht aus INA)
        currentData.current = measCurrent * 125.0; // mA
        currentData.voltage = measVoltage * 160.0 * 1000.0; // V × 160 in mV
        
        // Leistung berechnen: P = U * I
        // U in mV, I in mA -> P = (U * I) / 1000 in mW
        currentData.power = (currentData.voltage * currentData.current) / 1000.0; // mW
        
        // Skalierung mit aktiven Panels (1-4)
        float panelFactor = activePanels > 0 ? activePanels / 4.0 : 0.0;
        currentData.current *= panelFactor;
        currentData.voltage *= panelFactor;
        currentData.power *= panelFactor;
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
    
    // Berechne P_Netto = P_Solar - P_Last
    float solarPowerW = currentData.power / 1000.0; // mW zu W
    float loadPowerW = currentData.load; // W
    float netPowerW = solarPowerW - loadPowerW; // P_Netto in W
    
    // Zeitfaktor: 1 Sekunde Echtzeit = wie viele Stunden Simulation?
    float hoursPerSecond = 24.0 / (float)durationSeconds;
    
    // Update-Intervall in Sekunden (100ms = 0.1s)
    float deltaTimeSeconds = 0.1; // 100ms Loop
    
    // Formel: ΔE(Wh) = (P_Netto × Zeitfaktor × Δt(in Sek)) / 3600
    float deltaEnergyWh = (netPowerW * hoursPerSecond * deltaTimeSeconds) / 3600.0;
    
    // Batterie-Kapazität: 4 Zellen × 5 kWh = 20 kWh
    float batteryCapacityWh = activeCells * 5000.0; // Wh
    
    // ΔE in Prozent umrechnen
    float deltaPercent = (deltaEnergyWh / batteryCapacityWh) * 100.0;
    
    // Ladeeffizienz: 85% beim Laden
    if (netPowerW > 0) {
        deltaPercent *= 0.85;
    }
    
    currentData.batteryLevel += deltaPercent;
    
    // Clamp between 0 and 100
    if (currentData.batteryLevel < 0) currentData.batteryLevel = 0;
    if (currentData.batteryLevel > 100) currentData.batteryLevel = 100;
}

void Simulation::calculateLoad() {
    float totalLoad = 0.0;
    
    // Summiere aktive Lasten
    for (int i = 0; i < 8; i++) {
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
    else if (load == "wallbox") index = 4;
    else if (load == "heatpump") index = 5;
    else if (load == "dishwasher") index = 6;
    else if (load == "tv") index = 7;
    
    if (index >= 0) {
        loads[index] = state;
    }
}
