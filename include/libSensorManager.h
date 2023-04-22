
#ifndef CONTREAUL_H
    #include "contreaul.h"
    #define CONTREAUL_H 1
#endif

#include "libI2C.h"

#define SAMPLES 40

#define PRE_PATH "/sys/bus/w1/devices"
#define POST_PATH "w1_slave"


double avergearray(int* arr, int number);
float readPhValue();
float readWaterLevelValue();
float readTemperatureValue();
void readHumidityValues(uint8_t* humiditySensors);

