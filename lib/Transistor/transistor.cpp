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
    
    Serial.println("GPIO 15-18 (Transistors 1-4) initialized");
}

void Transistor::setState(int t1, int t2, int t3, int t4) {
    state1 = t1;
    state2 = t2;
    state3 = t3;
    state4 = t4;
}

void Transistor::update() {
    digitalWrite(TRANSISTOR_1, state1 ? HIGH : LOW);
    digitalWrite(TRANSISTOR_2, state2 ? HIGH : LOW);
    digitalWrite(TRANSISTOR_3, state3 ? HIGH : LOW);
    digitalWrite(TRANSISTOR_4, state4 ? HIGH : LOW);
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
