#ifndef CONTREAUL_H
    #include "contreaul.h"
    #define CONTREAUL_H 1
#endif

#include <wiringPi.h>

int watering(uint8_t *lines);
void setupToOutput();