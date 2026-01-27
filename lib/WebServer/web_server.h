#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include <WebServer.h>
#include <LittleFS.h>
#include "transistor.h"

class WebServerManager {
public:
    WebServerManager(Transistor* transistorRef);
    void begin();
    void handleClient();

private:
    WebServer server;
    Transistor* transistor;
    
    void handleRoot();
    void handleSetTransistor();
    void handleGetStatus();
    void handleNotFound();
};

#endif // WEB_SERVER_H
