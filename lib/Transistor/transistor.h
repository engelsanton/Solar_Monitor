#ifndef TRANSISTOR_H
#define TRANSISTOR_H

#include <Arduino.h>

class Transistor {
public:
    Transistor();
    void begin();
    void setState(int t1, int t2, int t3, int t4);
    void update();
    int getState1();
    int getState2();
    int getState3();
    int getState4();

private:
    int state1;
    int state2;
    int state3;
    int state4;
};

#endif // TRANSISTOR_H
