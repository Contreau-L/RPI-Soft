#ifndef CONTREAUL_H
    #include "contreaul.h"
    #define CONTREAUL_H 1
#endif

#include <wiringPi.h>

void enableWatering(uint8_t *linesToWater);
void stopWatering();