#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include <WebServer.h>
#include <LittleFS.h>
#include "transistor.h"
#include "simulation.h"
#include "ina.h"

class WebServerManager {
public:
    WebServerManager(Transistor* transistorRef, Simulation* simulationRef, INA* inaRef);
    void begin();
    void handleClient();

private:
    WebServer server;
    Transistor* transistor;
    Simulation* simulation;
    INA* ina;
    
    void handleRoot();
    void handleSetTransistor();
    void handleGetStatus();
    void handleSimulation();
    void handleSimulationData();
    void handleSetPanel();
    void handleSetCell();
    void handleSetLoad();
    void handleAutoToggleLoads();
    void handleRealData();
    void handleNotFound();
};

#endif // WEB_SERVER_H
