#include "oled.h"
#include "config.h"

OLED::OLED() : oledDisplay(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1), found(false) {
}

bool OLED::begin() {
    if (!oledDisplay.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        Serial.println("OLED-Initialisierung fehlgeschlagen!");
        found = false;
    } else {
        Serial.println("OLED erfolgreich initialisiert!");
        oledDisplay.setRotation(2);  // 180 Grad Rotation
        found = true;
    }
    return found;
}

bool OLED::isFound() {
    return found;
}

void OLED::clear() {
    if (!found) return;
    oledDisplay.clearDisplay();
}

void OLED::display() {
    if (!found) return;
    this->oledDisplay.display();
}

void OLED::showBootScreen() {
    if (!found) return;
    
    oledDisplay.clearDisplay();
    oledDisplay.setTextSize(1);
    oledDisplay.setTextColor(SSD1306_WHITE);
    oledDisplay.setCursor(0, 0);
    oledDisplay.println("Booting...");
    oledDisplay.display();
}

void OLED::showStatus(int t1, int t2, int t3, int t4, float busV, float shuntV, float currentMA, float powerMW, bool inaAvailable) {
    if (!found) return;
    
    oledDisplay.clearDisplay();
    oledDisplay.setTextSize(1);
    oledDisplay.setTextColor(SSD1306_WHITE);
    oledDisplay.setCursor(0, 0);
    
    oledDisplay.println("=== GPIO Status ===");
    oledDisplay.print("T1:");
    oledDisplay.print(t1 ? "AN" : "AUS");
    oledDisplay.print(" T2:");
    oledDisplay.println(t2 ? "AN" : "AUS");
    oledDisplay.print("T3:");
    oledDisplay.print(t3 ? "AN" : "AUS");
    oledDisplay.print(" T4:");
    oledDisplay.println(t4 ? "AN" : "AUS");
    
    if (inaAvailable) {
        oledDisplay.println("");
        oledDisplay.println("=== INA219 ===");
        
        // Werte unter 1 werden auf 0 gesetzt
        if (busV < 1.0) busV = 0.0;
        if (shuntV < 1.0) shuntV = 0.0;
        if (currentMA < 1.0) currentMA = 0.0;
        if (powerMW < 1.0) powerMW = 0.0;
        
        oledDisplay.print("Bus: ");
        oledDisplay.print(busV, 2);
        oledDisplay.println("V");
        
        oledDisplay.print("Shunt: ");
        oledDisplay.print(shuntV, 2);
        oledDisplay.println("mV");
        
        oledDisplay.print("Strom: ");
        oledDisplay.print(currentMA, 2);
        oledDisplay.println("mA");
        
        oledDisplay.print("Leistung: ");
        oledDisplay.print(powerMW, 2);
        oledDisplay.println("mW");
    }
    
    oledDisplay.display();
}
