#include "transistor.h"
#include "config.h"

Transistor::Transistor() : state1(1), state2(0), state3(0), state4(0) {
}

void Transistor::begin() {
    pinMode(TRANSISTOR_1, OUTPUT);
    pinMode(TRANSISTOR_2, OUTPUT);
    pinMode(TRANSISTOR_3, OUTPUT);
    pinMode(TRANSISTOR_4, OUTPUT);
    
    update();
    
    Serial.println("GPIO 15-18 (Transistoren 1-4) initialisiert");
}

void Transistor::setState(int t1, int t2, int t3, int t4) {
    Serial.println("[DEBUG] Transistor::setState() - Start");
    
    state1 = t1;
    state2 = t2;
    state3 = t3;
    state4 = t4;
    
    Serial.print("[DEBUG] Transistor::setState() - T1=");
    Serial.print(t1);
    Serial.print(" T2=");
    Serial.print(t2);
    Serial.print(" T3=");
    Serial.print(t3);
    Serial.print(" T4=");
    Serial.println(t4);
}

void Transistor::update() {
    Serial.println("[DEBUG] Transistor::update() - Start");
    
    digitalWrite(TRANSISTOR_1, state1 ? HIGH : LOW);
    digitalWrite(TRANSISTOR_2, state2 ? HIGH : LOW);
    digitalWrite(TRANSISTOR_3, state3 ? HIGH : LOW);
    digitalWrite(TRANSISTOR_4, state4 ? HIGH : LOW);
    
    Serial.print("[DEBUG] Transistor::update() - GPIO written - T1=");
    Serial.print(state1);
    Serial.print(" T2=");
    Serial.print(state2);
    Serial.print(" T3=");
    Serial.print(state3);
    Serial.print(" T4=");
    Serial.println(state4);
}

int Transistor::getState1() {
    return state1;
}

int Transistor::getState2() {
    return state2;
}

int Transistor::getState3() {
    return state3;
}

int Transistor::getState4() {
    return state4;
}
