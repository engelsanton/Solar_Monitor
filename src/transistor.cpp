#include <Arduino.h>
#include "config.h"

static bool transistorState = false;
static unsigned long lastToggle = 0;

void transistorInit() {
    pinMode(TRANSISTOR_PIN, OUTPUT);
    digitalWrite(TRANSISTOR_PIN, LOW);
    lastToggle = millis();
}

void transistorToggleIfNeeded() {
    unsigned long now = millis();
    if (now - lastToggle >= TOGGLE_INTERVAL) {
        transistorState = !transistorState;
        digitalWrite(TRANSISTOR_PIN, transistorState ? HIGH : LOW);
        Serial.print("Transistor GPIO4 ist jetzt: ");
        Serial.println(transistorState ? "HIGH" : "LOW");
        lastToggle = now;
    }
}
