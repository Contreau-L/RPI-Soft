#ifndef LIBACTUATOR_H
    #include "../include/libActuatorManager.h"
    #define LIBACTUATOR_H 1
#endif

extern int NB_HUMIDITY_SENSORS;
extern actuatorsPinConfiguration actuatorsPinConfig; 

int watering(uint8_t *lines){
    int nbLinesToWater = 0;
    for(int i = 0; i < NB_HUMIDITY_SENSORS; i++){
        if(lines[i] == 1){
            printf("watering line %d\n",i);
            digitalWrite(actuatorsPinConfig.waterValvePins[i], HIGH);
            nbLinesToWater++;
        }
        else if(lines[i] == 0){
            digitalWrite(actuatorsPinConfig.waterValvePins[i], LOW);   
        }
        else{
            printf("error in linesToWaterShm\n");
        }
    }
    if(nbLinesToWater == 0){
        digitalWrite(actuatorsPinConfig.pumpPin, LOW);
        printf("pump off\n");
        return 0;
    }
    else{
        digitalWrite(actuatorsPinConfig.pumpPin, HIGH);
        printf("pump on\n");
        return 1;
    }

}

void setupToOutput(){
    if (wiringPiSetup() == -1) {
        printf("Erreur d'initialisation de wiringPi\n");
        exit(1);
    }
    for(int i = 0; i < NB_HUMIDITY_SENSORS; i++){
        pinMode(actuatorsPinConfig.waterValvePins[i], OUTPUT);
        digitalWrite(actuatorsPinConfig.waterValvePins[i], LOW);
    }
    pinMode(actuatorsPinConfig.pumpPin, OUTPUT);
    digitalWrite(actuatorsPinConfig.pumpPin, LOW);
}