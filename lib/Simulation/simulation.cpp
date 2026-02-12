#include "simulation.h"

Simulation::Simulation(INA* inaRef) : ina(inaRef) {
    // Initialize all states to false
    for (int i = 0; i < 4; i++) {
        panels[i] = false;
        cells[i] = false;
    }
    
    for (int i = 0; i < 6; i++) {
        loads[i] = false;
    }
    
    // Default: Panel 1 and Cell 1 active
    panels[0] = true;
    cells[0] = true;
    
    // Load watts: light, fridge, ac, dryer, dishwasher, tv
    loadWatts[0] = 100;
    loadWatts[1] = 150;
    loadWatts[2] = 2000;
    loadWatts[3] = 500;
    loadWatts[4] = 1000;
    loadWatts[5] = 300;
    
    // Initialize simulation state
    running = false;
    simulateSun = false;
    autoToggleLoads = false;
    currentMultiplier = 1000.0;  // Default calibration multiplier
    startTime = 0;
    lastUpdateTime = 0;
    durationSeconds = 48;
    simCurrentHour = 6.0;  // Start at 6:00 AM
    lastCalculatedStep = -1;  // Force calculation on first update
    activePanelsSnapshot = 0;
    activeCellsSnapshot = 0;
    
    // Initialize energy tracking
    totalEnergyFromGrid = 0.0;
    totalEnergyToGrid = 0.0;
    totalEnergyConsumed = 0.0;
    
    // Initialize data
    currentData.voltage = 0.0;
    currentData.current = 0.0;
    currentData.powerGenerated = 0.0;
    currentData.powerLoad = 0.0;
    currentData.powerNet = 0.0;
    currentData.batteryLevel = 0.0;  // Start at 0%
    currentData.hour = 6;
    currentData.minute = 0;
    currentData.irradiance = 0.0;
}

void Simulation::begin() {
    Serial.println("Simulation initialized");
}

void Simulation::start(int durationSeconds, bool simulateSun) {
    this->durationSeconds = durationSeconds;
    this->simulateSun = simulateSun;
    this->startTime = millis();
    this->lastUpdateTime = startTime;
    this->simCurrentHour = 6.0;  // Reset to 6:00 AM
    this->lastCalculatedStep = -1;  // Force calculation
    
    // Count and lock active panels
    activePanelsSnapshot = 0;
    for (int i = 0; i < 4; i++) {
        if (panels[i]) activePanelsSnapshot++;
    }
    
    // Count and lock active cells
    activeCellsSnapshot = 0;
    for (int i = 0; i < 4; i++) {
        if (cells[i]) activeCellsSnapshot++;
    }
    
    // Reset energy tracking
    totalEnergyFromGrid = 0.0;
    totalEnergyToGrid = 0.0;
    totalEnergyConsumed = 0.0;
    
    this->running = true;
    
    // Reset battery to 0%
    currentData.batteryLevel = 0.0;
    
    Serial.println("=== Simulation Started ===");
    Serial.print("Duration: ");
    Serial.print(durationSeconds);
    Serial.println(" seconds");
    Serial.print("Simulate Sun: ");
    Serial.println(simulateSun ? "ON" : "OFF");
    Serial.print("Active Panels: ");
    Serial.println(activePanelsSnapshot);
    Serial.print("Active Cells: ");
    Serial.println(activeCellsSnapshot);
    Serial.print("Time step: 24h in ");
    Serial.print(durationSeconds);
    Serial.print("s -> 1h = ");
    Serial.print(durationSeconds / 24.0);
    Serial.println("s");
}

void Simulation::stop() {
    running = false;
    simCurrentHour = 0.0;
    Serial.println("=== Simulation Stopped ===");
}

void Simulation::update() {
    if (!running) return;
    
    unsigned long currentTime = millis();
    unsigned long elapsedMs = currentTime - startTime;
    float elapsedSeconds = elapsedMs / 1000.0;
    
    // Check if 24h simulation completed
    if (elapsedSeconds >= durationSeconds) {
        stop();
        return;
    }
    
    // Calculate current simulation hour (6-30), starting at 06:00 (6 AM)
    float hoursPerSecond = 24.0 / (float)durationSeconds;
    float totalHours = elapsedSeconds * hoursPerSecond;
    simCurrentHour = 6.0 + totalHours;  // Start at 6:00 and run for 24h to 30:00
    
    // Update time display (wrap to 0-23 for display)
    float displayHour = fmod(simCurrentHour, 24.0);
    currentData.hour = (int)displayHour;
    currentData.minute = (int)((displayHour - currentData.hour) * 60.0);
    
    // Calculate current simulation step (48 steps for 24h = 2 steps per hour)
    int currentStep = (int)(simCurrentHour * 2.0);
    
    // Only recalculate if we moved to a new step
    if (currentStep != lastCalculatedStep) {
        lastCalculatedStep = currentStep;
        
        // Calculate delta time since last update
        float deltaTimeSeconds = (currentTime - lastUpdateTime) / 1000.0;
        
        // Update simulation
        calculateSolarData();
        calculateLoad();
        calculateBattery(deltaTimeSeconds);
        
        lastUpdateTime = currentTime;
    }
}

void Simulation::calculateSolarData() {
    // Use snapshot of panels from simulation start
    int activePanels = activePanelsSnapshot;
    
    if (activePanels == 0) {
        currentData.voltage = 0.0;
        currentData.current = 0.0;
        currentData.powerGenerated = 0.0;
        currentData.irradiance = 0.0;
        return;
    }
    
    // Get time of day (0-24) for voltage/current calculations
    float timeOfDay = fmod(simCurrentHour, 24.0);
    
    float baseVoltage = 0.0;
    float currentPerPanel = 0.0;
    
    // Calibration mode: Use real INA219 current measurements
    if (!simulateSun) {
        // Read real current from INA219
        float realCurrent = ina->getCurrent();  // mA
        
        // Apply calibration factor for current - multiply by number of active panels
        // Because INA measures total current from all panels in parallel
        currentPerPanel = (realCurrent * currentMultiplier) / 1000.0 / activePanels;  // Convert mA to A, scale, and divide by panels
        
        // Simulate voltage based on time of day (same as simulation mode)
        if (timeOfDay >= 18.0 || timeOfDay < 6.0) {
            // Night (18:00 - 6:00): Complete darkness
            baseVoltage = 0.0;
        } 
        else if (timeOfDay >= 6.0 && timeOfDay < 7.0) {
            // Sunrise (6:00 - 7:00): Voltage rises from 0V to 200V
            float sunriseProgress = (timeOfDay - 6.0) / 1.0;  // 0 to 1
            baseVoltage = 200.0 * sunriseProgress;  // 0V -> 200V
        }
        else if (timeOfDay >= 7.0 && timeOfDay < 12.0) {
            // Morning (7:00 - 12:00): Voltage drops from 200V to 180V
            float morningProgress = (timeOfDay - 7.0) / 5.0;  // 0 to 1
            baseVoltage = 200.0 - (20.0 * morningProgress);  // 200V -> 180V
        }
        else if (timeOfDay >= 12.0 && timeOfDay < 17.0) {
            // Afternoon (12:00 - 17:00): Voltage rises from 180V to 200V
            float afternoonProgress = (timeOfDay - 12.0) / 5.0;  // 0 to 1
            baseVoltage = 180.0 + (20.0 * afternoonProgress);  // 180V -> 200V
        }
        else if (timeOfDay >= 17.0 && timeOfDay < 18.0) {
            // Sunset (17:00 - 18:00): Voltage drops from 200V to 0V
            float sunsetProgress = (timeOfDay - 17.0) / 1.0;  // 0 to 1
            baseVoltage = 200.0 * (1.0 - sunsetProgress);  // 200V -> 0V
        }
        
        // Set irradiance based on measured current (for display)
        currentData.irradiance = min(1.0f, currentPerPanel / 12.0f);
        
        // Apply jitter to voltage (same as simulation mode)
        if (baseVoltage > 0.0) {
            baseVoltage = applyJitter(baseVoltage, 5.0);
            if (baseVoltage < 0.0) baseVoltage = 0.0;
            if (baseVoltage > 220.0) baseVoltage = 220.0;
        }
        
        // Calculate total current and power
        float totalCurrent = currentPerPanel * activePanels;
        currentData.voltage = baseVoltage;
        currentData.current = totalCurrent;
        currentData.powerGenerated = baseVoltage * totalCurrent;
        return;
    }
    
    // Simulation mode: Use solar profile
    // Get base irradiance from solar profile
    float irradiance = getSolarIrradiance(timeOfDay);
    
    // Apply jitter (±12% random noise)
    irradiance = applyJitter(irradiance, 12.0);
    
    // Apply cloud effect (random brief drops)
    irradiance = applyCloudEffect(irradiance);
    
    // Clamp between 0 and 1
    if (irradiance < 0.0) irradiance = 0.0;
    if (irradiance > 1.0) irradiance = 1.0;
    
    currentData.irradiance = irradiance;
    
    // Realistic day-night cycle (Updated):
    // Night (18:00-6:00): Complete darkness, U=0V, I=0A
    // Sunrise (6:00-7:00): Voltage rises from 0V to 200V (peak at 7am), minimal current
    // Morning (7:00-12:00): Voltage drops 200V->180V, current rises
    // Noon (12:00): Current maximum, voltage at 180V
    // Afternoon (12:00-17:00): Voltage rises 180V->200V, current drops
    // Sunset (17:00-18:00): Voltage drops from 200V to 0V
    // Night (18:00+): Complete darkness
    
    if (timeOfDay >= 18.0 || timeOfDay < 6.0) {
        // Night (18:00 - 6:00): Complete darkness
        baseVoltage = 0.0;
        currentPerPanel = 0.0;
    } 
    else if (timeOfDay >= 6.0 && timeOfDay < 7.0) {
        // Sunrise (6:00 - 7:00): Voltage rises from 0V to 200V
        float sunriseProgress = (timeOfDay - 6.0) / 1.0;  // 0 to 1
        baseVoltage = 200.0 * sunriseProgress;  // 0V -> 200V
        currentPerPanel = 0.0;  // Almost no current yet
    }
    else if (timeOfDay >= 7.0 && timeOfDay < 12.0) {
        // Morning (7:00 - 12:00): Voltage drops, current rises
        float morningProgress = (timeOfDay - 7.0) / 5.0;  // 0 to 1
        baseVoltage = 200.0 - (20.0 * morningProgress);  // 200V -> 180V
        currentPerPanel = 12.0 * irradiance * morningProgress;  // 0A -> 12A
    }
    else if (timeOfDay >= 12.0 && timeOfDay < 17.0) {
        // Afternoon (12:00 - 17:00): Voltage rises, current drops
        float afternoonProgress = (timeOfDay - 12.0) / 5.0;  // 0 to 1
        baseVoltage = 180.0 + (20.0 * afternoonProgress);  // 180V -> 200V
        currentPerPanel = 12.0 * irradiance * (1.0 - afternoonProgress);  // 12A -> 0A
    }
    else if (timeOfDay >= 17.0 && timeOfDay < 18.0) {
        // Sunset (17:00 - 18:00): Voltage drops from 200V to 0V
        float sunsetProgress = (timeOfDay - 17.0) / 1.0;  // 0 to 1
        baseVoltage = 200.0 * (1.0 - sunsetProgress);  // 200V -> 0V
        currentPerPanel = 0.0;  // No current anymore
    }
    
    // Apply jitter to voltage (±5% noise)
    if (baseVoltage > 0.0) {
        baseVoltage = applyJitter(baseVoltage, 5.0);
        if (baseVoltage < 0.0) baseVoltage = 0.0;
        if (baseVoltage > 220.0) baseVoltage = 220.0;  // Clamp to reasonable range
    }
    
    // Total current = currentPerPanel × number of active panels
    float totalCurrent = currentPerPanel * activePanels;
    
    // Power = V × I (per panel)
    float power = baseVoltage * totalCurrent;  // Total power in Watts
    
    // Store values
    currentData.voltage = baseVoltage;  // Voltage (constant across panels in series)
    currentData.current = totalCurrent;  // Total current (sum of all panels)
    currentData.powerGenerated = power;
}

void Simulation::calculateLoad() {
    // Auto toggle loads based on time if enabled
    if (autoToggleLoads && running) {
        // Get time of day (0-23) for load scheduling
        int hour = (int)fmod(simCurrentHour, 24.0);
        
        // Light (100W): 6-9 and 18-24
        loads[0] = (hour >= 6 && hour < 9) || (hour >= 18 && hour < 24);
        
        // Fridge (150W): 24h
        loads[1] = true;
        
        // AC (1500W): 10-22
        loads[2] = (hour >= 10 && hour < 22);
        
        // Dryer (500W): 16-17
        loads[3] = (hour >= 16 && hour < 17);
        
        // Dishwasher (2000W): 10-11 and 17-18
        loads[4] = (hour >= 10 && hour < 11) || (hour >= 17 && hour < 18);
        
        // TV (300W): 19-22
        loads[5] = (hour >= 19 && hour < 22);
    }
    
    float totalLoad = 0.0;
    
    // Sum all active loads
    for (int i = 0; i < 6; i++) {
        if (loads[i]) {
            totalLoad += loadWatts[i];
        }
    }
    
    // Apply ±3% fluctuation
    totalLoad = applyJitter(totalLoad, 3.0);
    
    if (totalLoad < 0.0) totalLoad = 0.0;
    
    currentData.powerLoad = totalLoad;
}

void Simulation::calculateBattery(float deltaTimeSeconds) {
    // Use snapshot of cells from simulation start
    int activeCells = activeCellsSnapshot;
    
    // Calculate net power: P_net = P_gen - P_load
    currentData.powerNet = currentData.powerGenerated - currentData.powerLoad;
    
    // Time scaling: real time -> simulation time
    float hoursPerSecond = 24.0 / (float)durationSeconds;
    float simulatedHours = deltaTimeSeconds * hoursPerSecond;
    
    // Track energy consumption (always, regardless of battery)
    float energyConsumedWh = currentData.powerLoad * simulatedHours;
    totalEnergyConsumed += energyConsumedWh / 1000.0;  // Convert Wh to kWh
    
    if (activeCells == 0) {
        // No battery available - direct grid interaction
        currentData.batteryLevel = 0.0;
        
        // All surplus/deficit goes directly to/from grid
        if (currentData.powerNet > 0) {
            // Surplus: all generated power above load goes to grid
            float excessEnergyWh = currentData.powerNet * simulatedHours;
            totalEnergyToGrid += excessEnergyWh / 1000.0;  // Convert to kWh
        } else if (currentData.powerNet < 0) {
            // Deficit: missing power comes from grid
            float missingEnergyWh = -currentData.powerNet * simulatedHours;
            totalEnergyFromGrid += missingEnergyWh / 1000.0;  // Convert to kWh
        }
        return;
    }
    
    // Battery capacity: 5kWh per cell
    float batteryCapacityWh = activeCells * 5000.0;  // Wh
    
    // Energy change: ΔE = P_net * time (in hours)
    float deltaEnergyWh = currentData.powerNet * simulatedHours;
    
    // Apply charging efficiency (85% when charging)
    if (deltaEnergyWh > 0) {
        deltaEnergyWh *= 0.85;
    }
    
    // Convert to percentage: ΔSoC% = (ΔE / Capacity) * 100
    float deltaSoC = (deltaEnergyWh / batteryCapacityWh) * 100.0;
    
    // Calculate new SoC (before clamping)
    float newSoC = currentData.batteryLevel + deltaSoC;
    
    // Track grid energy flow BEFORE clamping
    // If would go below 0%, we need grid power
    if (newSoC < 0.0) {
        float missingSoC = 0.0 - newSoC;  // How much below 0%
        float missingEnergyWh = (missingSoC / 100.0) * batteryCapacityWh;
        totalEnergyFromGrid += missingEnergyWh / 1000.0;  // Convert to kWh
        currentData.batteryLevel = 0.0;
    } 
    // If would go above 100%, excess goes to grid
    else if (newSoC > 100.0) {
        float excessSoC = newSoC - 100.0;  // How much above 100%
        float excessEnergyWh = (excessSoC / 100.0) * batteryCapacityWh;
        totalEnergyToGrid += excessEnergyWh / 1000.0;  // Convert to kWh
        currentData.batteryLevel = 100.0;
    } else {
        currentData.batteryLevel = newSoC;
    }
}

float Simulation::getSolarIrradiance(float hour) {
    // Half-sine wave from 06:00 to 18:00
    // Peak at 12:00 (noon)
    
    if (hour < 6.0 || hour >= 18.0) {
        return 0.0;  // No sun at night
    }
    
    // Map hour to sine wave (0 to π)
    float hoursSinceSunrise = hour - 6.0;  // 0 to 12
    float angle = (hoursSinceSunrise / 12.0) * PI;  // 0 to π
    
    return sin(angle);  // 0 to 1 to 0
}

float Simulation::applyJitter(float value, float percentage) {
    // Add random noise: ±percentage%
    float jitterFactor = (random(-100, 101) / 100.0) * (percentage / 100.0);
    return value * (1.0 + jitterFactor);
}

float Simulation::applyCloudEffect(float irradiance) {
    // Random cloud events: 10% chance per update
    // When cloud passes, drop irradiance by 40-80%
    
    if (random(0, 100) < 10) {
        float dropPercentage = random(40, 81) / 100.0;
        return irradiance * (1.0 - dropPercentage);
    }
    
    return irradiance;
}

bool Simulation::isRunning() {
    return running;
}

float Simulation::getProgress() {
    if (!running) return 0.0;
    
    unsigned long elapsed = (millis() - startTime) / 1000;
    float progress = (float)elapsed / (float)durationSeconds;
    return progress > 1.0 ? 1.0 : progress;
}

SimulationData Simulation::getCurrentData() {
    return currentData;
}

String Simulation::getDataAsJson() {
    String json = "{";
    json += "\"voltage\":" + String(currentData.voltage, 2) + ",";
    json += "\"current\":" + String(currentData.current, 2) + ",";
    json += "\"powerGenerated\":" + String(currentData.powerGenerated, 2) + ",";
    json += "\"powerLoad\":" + String(currentData.powerLoad, 2) + ",";
    json += "\"powerNet\":" + String(currentData.powerNet, 2) + ",";
    json += "\"batteryLevel\":" + String(currentData.batteryLevel, 2) + ",";
    json += "\"hour\":" + String(currentData.hour) + ",";
    json += "\"minute\":" + String(currentData.minute) + ",";
    json += "\"irradiance\":" + String(currentData.irradiance, 3) + ",";
    json += "\"isRunning\":" + String(running ? "true" : "false") + ",";
    json += "\"autoToggleLoads\":" + String(autoToggleLoads ? "true" : "false") + ",";
    json += "\"loads\":{";
    json += "\"light\":" + String(loads[0] ? "true" : "false") + ",";
    json += "\"fridge\":" + String(loads[1] ? "true" : "false") + ",";
    json += "\"ac\":" + String(loads[2] ? "true" : "false") + ",";
    json += "\"dryer\":" + String(loads[3] ? "true" : "false") + ",";
    json += "\"dishwasher\":" + String(loads[4] ? "true" : "false") + ",";
    json += "\"tv\":" + String(loads[5] ? "true" : "false");
    json += "},";
    json += "\"progress\":" + String(getProgress(), 3);
    json += "}";
    return json;
}

String Simulation::getOverviewJson() {
    // Calculate autarky level (self-sufficiency)
    float autarky = 0.0;
    if (totalEnergyConsumed > 0.0) {
        autarky = ((totalEnergyConsumed - totalEnergyFromGrid) / totalEnergyConsumed) * 100.0;
        if (autarky < 0.0) autarky = 0.0;
        if (autarky > 100.0) autarky = 100.0;
    }
    
    // Calculate costs and revenue
    float costZAR = totalEnergyFromGrid * 3.50;
    float costEUR = totalEnergyFromGrid * 0.20;
    float revenueZAR = totalEnergyToGrid * 1.17;
    float revenueEUR = totalEnergyToGrid * 0.06;
    
    String json = "{";
    json += "\"autarky\":" + String(autarky, 1) + ",";
    json += "\"energyFromGrid\":" + String(totalEnergyFromGrid, 3) + ",";
    json += "\"energyToGrid\":" + String(totalEnergyToGrid, 3) + ",";
    json += "\"energyConsumed\":" + String(totalEnergyConsumed, 3) + ",";
    json += "\"costZAR\":" + String(costZAR, 2) + ",";
    json += "\"costEUR\":" + String(costEUR, 2) + ",";
    json += "\"revenueZAR\":" + String(revenueZAR, 2) + ",";
    json += "\"revenueEUR\":" + String(revenueEUR, 2);
    json += "}";
    return json;
}

void Simulation::setPanelState(int panel, bool state) {
    if (panel >= 1 && panel <= 4) {
        panels[panel - 1] = state;
        Serial.print("Panel ");
        Serial.print(panel);
        Serial.println(state ? " ON" : " OFF");
        
        // Note: Changes during simulation won't take effect until next start
        if (running) {
            Serial.println("  (Changes will apply on next simulation start)");
        }
    }
}

void Simulation::setCellState(int cell, bool state) {
    if (cell >= 1 && cell <= 4) {
        cells[cell - 1] = state;
        Serial.print("Cell ");
        Serial.print(cell);
        Serial.println(state ? " ON" : " OFF");
    }
}

void Simulation::setLoadState(String load, bool state) {
    int index = -1;
    if (load == "light") index = 0;
    else if (load == "fridge") index = 1;
    else if (load == "ac") index = 2;
    else if (load == "dryer") index = 3;
    else if (load == "dishwasher") index = 4;
    else if (load == "tv") index = 5;
    
    if (index >= 0) {
        // Ignore manual changes if auto toggle is enabled
        if (autoToggleLoads) {
            Serial.println("Auto toggle loads enabled - ignoring manual change");
            return;
        }
        
        loads[index] = state;
        Serial.print("Load ");
        Serial.print(load);
        Serial.println(state ? " ON" : " OFF");
    }
}

void Simulation::setAutoToggleLoads(bool enable) {
    autoToggleLoads = enable;
    Serial.print("Auto toggle loads: ");
    Serial.println(enable ? "ENABLED" : "DISABLED");
}

void Simulation::setCurrentMultiplier(float multiplier) {
    currentMultiplier = multiplier;
    Serial.print("Current multiplier set to: ");
    Serial.println(multiplier);
}
