#ifndef SIMULATION_H
#define SIMULATION_H

#include <Arduino.h>
#include "ina.h"

struct SimulationData {
    float current;
    float voltage;
    float power;
    float batteryLevel;
    float load;
    int hour;
    int minute;
    bool isDaytime;
};

class Simulation {
public:
    Simulation();
    void begin();
    void start(int durationSeconds);
    void stop();
    void update();
    bool isRunning();
    SimulationData getCurrentData();
    String getDataAsJson();
    float getProgress(); // 0.0 bis 1.0
    
    void setPanelState(int panel, bool state); // panel 1-4
    void setCellState(int cell, bool state);   // cell 1-4
    void setLoadState(String load, bool state); // "light", "fridge", "ac", "washing"
    void setINA(INA* inaRef); // Set INA reference for real data
    
private:
    bool running;
    unsigned long startTime;
    unsigned long lastUpdateTime;
    int durationSeconds;
    float simCurrentHour;
    INA* ina;
    
    // Panel/Cell/Load states
    bool panels[4];
    bool cells[4];
    bool loads[4]; // light, fridge, ac, washing
    int loadWatts[4]; // 10, 150, 2000, 500
    
    SimulationData currentData;
    
    void calculateSolarData();
    void calculateBattery();
    void calculateLoad();
    float getSolarIntensity(float hour);
};

#endif // SIMULATION_H
