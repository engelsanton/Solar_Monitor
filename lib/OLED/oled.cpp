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

void OLED::showStatus(int panel1, int panel2, int panel3, int panel4, float voltage, float current, bool inaAvailable) {
    if (!found) return;
    
    oledDisplay.clearDisplay();
    oledDisplay.setTextSize(1);
    oledDisplay.setTextColor(SSD1306_WHITE);
    oledDisplay.setCursor(0, 0);
    
    // Panel Status
    oledDisplay.println("=== PANEL STATUS ===");
    oledDisplay.print("P1:");
    oledDisplay.print(panel1 ? "ON " : "OFF");
    oledDisplay.print(" P2:");
    oledDisplay.println(panel2 ? "ON " : "OFF");
    oledDisplay.print("P3:");
    oledDisplay.print(panel3 ? "ON " : "OFF");
    oledDisplay.print(" P4:");
    oledDisplay.println(panel4 ? "ON " : "OFF");
    
    if (inaAvailable) {
        oledDisplay.println("");
        oledDisplay.println("=== SENSOR DATA ===");
        
        // Werte unter 1 werden auf 0 gesetzt

        if (current < 1.0) current = 0.0;
        

        // Current
        oledDisplay.print("Current: ");
        oledDisplay.print(current, 1);
        oledDisplay.println(" mA");
    } else {
        oledDisplay.println("");
        oledDisplay.println("INA219 not found");
    }
    
    oledDisplay.display();
}
