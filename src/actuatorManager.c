#include <semaphore.h>
#ifndef ACTUATORMANAGER_H
    #include "../include/actuatorManager.h"
    #define ACTUATORMANAGER_H 1
#endif
#ifndef LIBSHAREDMEMORY_H
    #include "../include/libSharedMemory.h"
    #define LIBSHAREDMEMORY_H 1
#endif
#ifndef LIBACTUATOR_H
    #include "../include/libActuatorManager.h"
    #define LIBACTUATOR_H 1
#endif

extern int idWaterShm;
extern int NB_HUMIDITY_SENSORS;
sem_t actuatorSem;
uint8_t lines[255];
int isWatering = 0;
void initActuatorManager(){ 
    initSignalHandler(actuatorManagerSignalHandler,2,SIGUSR1,SIGALRM);
    sem_init(&actuatorSem,0,0);
    actuatorManager();

}
void actuatorManager(){
    while(1){
        sem_wait(&actuatorSem);
        if(isWatering == 0){
            //start watering
            isWatering = 1;
            readLineToWaterShm(idWaterShm,lines);
            enableWatering(lines);
        }
        else{
            //stop watering
            isWatering = 0;
            stopWatering();
        }
    }
}
void actuatorManagerSignalHandler(int signal, siginfo_t *info){
    switch(signal){
        case SIGUSR1 :
            isWatering = 0;
            sem_post(&actuatorSem);
            break;
        case SIGALRM :
            isWatering = 1;
            sem_post(&actuatorSem); //call to stop watering
            break;
        default :
            break;  
    }
}