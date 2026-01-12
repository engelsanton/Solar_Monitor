#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "config.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void oledInit() {
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("OLED nicht gefunden!");
        while (1) { delay(10); }
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.println("OLED bereit!");
    display.display();
}

void oledShowValues(float voltage, float current) {
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextSize(2);
    display.print("U: ");
    display.print(voltage, 2);
    display.println(" V");
    display.print("I: ");
    display.print(current, 2);
    display.println(" mA");
    display.display();
}
