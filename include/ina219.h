#ifndef INA219_H
#define INA219_H


void ina219Init();
void ina219PrintValues();
float ina219_getBusVoltage();
float ina219_getCurrent();

#endif // INA219_H
