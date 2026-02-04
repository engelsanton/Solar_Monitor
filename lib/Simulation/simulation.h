#ifndef SIMULATION_H
#define SIMULATION_H

#include <Arduino.h>

struct SimulationData {
    float voltage;          // V
    float current;          // A
    float powerGenerated;   // W
    float powerLoad;        // W
    float powerNet;         // W
    float batteryLevel;     // %
    int hour;               // 0-23
    int minute;             // 0-59
    float irradiance;       // 0-1 (fraction of max)
};

class Simulation {
public:
    Simulation();
    void begin();
    
    // Control methods
    void start(int durationSeconds, bool simulateSun);
    void stop();
    void update();
    bool isRunning();
    float getProgress();  // 0.0 to 1.0
    void setAutoToggleLoads(bool enable);  // Enable/disable auto toggle
    
    // State setters
    void setPanelState(int panel, bool state);    // panel 1-4
    void setCellState(int cell, bool state);      // cell 1-4
    void setLoadState(String load, bool state);   // load types
    
    // Data getters
    SimulationData getCurrentData();
    String getDataAsJson();
    String getOverviewJson();  // Get daily overview statistics
    
private:
    // State arrays
    bool panels[4];
    bool cells[4];
    bool loads[6];
    int loadWatts[6];
    
    // Simulation state
    bool running;
    bool simulateSun;
    bool autoToggleLoads;  // Auto toggle loads based on time
    unsigned long startTime;
    unsigned long lastUpdateTime;
    int durationSeconds;
    float simCurrentHour;  // 0.0 to 24.0
    int lastCalculatedStep;  // Track last calculated simulation step
    
    // Simulation snapshot (fixed at start)
    int activePanelsSnapshot;  // Number of panels when simulation started
    int activeCellsSnapshot;   // Number of cells when simulation started
    
    // Energy tracking for daily overview
    float totalEnergyFromGrid;    // kWh drawn from grid
    float totalEnergyToGrid;      // kWh fed into grid
    float totalEnergyConsumed;    // kWh total consumption
    
    // Current data
    SimulationData currentData;
    
    // Calculation methods
    void calculateSolarData();
    void calculateBattery(float deltaTimeSeconds);
    void calculateLoad();
    float getSolarIrradiance(float hour);
    float applyJitter(float value, float percentage);
    float applyCloudEffect(float irradiance);
};

#endif // SIMULATION_H
