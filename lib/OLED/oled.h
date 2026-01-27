#ifndef OLED_H
#define OLED_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class OLED {
public:
    OLED();
    bool begin();
    bool isFound();
    void clear();
    void display();
    void showBootScreen();
    void showStatus(int t1, int t2, int t3, int t4, float busV, float shuntV, float currentMA, float powerMW, bool inaAvailable);

private:
    Adafruit_SSD1306 oledDisplay;
    bool found;
};

#endif // OLED_H
