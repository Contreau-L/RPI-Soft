#ifndef LIBACTUATOR_H
    #include "../include/libActuatorManager.h"
    #define LIBACTUATOR_H 1
#endif

extern int NB_HUMIDITY_SENSORS;
extern actuatorsPinConfiguration actuatorsPinConfig; 

void enableWatering(uint8_t *linesToWater){
    for(int i = 0; i < NB_HUMIDITY_SENSORS; i++){
        if(linesToWater[i] == 1){
            digitalWrite(actuatorsPinConfig.waterValvePins[i], HIGH);
        }
    }
    digitalWrite(actuatorsPinConfig.pumpPin, HIGH);
}

void stopWatering(){
    digitalWrite(actuatorsPinConfig.pumpPin, LOW);
    for(int i = 0; i < NB_HUMIDITY_SENSORS; i++){
        digitalWrite(actuatorsPinConfig.waterValvePins[i], LOW);
    }
}