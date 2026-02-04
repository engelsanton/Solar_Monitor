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
    void showStatus(int panel1, int panel2, int panel3, int panel4, float voltage, float current, bool inaAvailable);

private:
    Adafruit_SSD1306 oledDisplay;
    bool found;
};

#endif // OLED_H
