
#ifndef CONTREAUL_H
    #include "contreaul.h"
    #define CONTREAUL_H 1
#endif

#include "libI2C.h"

#define SAMPLES 40

#define PATH "/sys/bus/w1/devices/28-00000e6680a5/w1_slave"


double avergearray(int* arr, int number);
float readPhValue();
float readWaterLevelValue();
float readTemperatureValue();
void readHumidityValues(uint8_t* humiditySensors);

