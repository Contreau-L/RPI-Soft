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

void initActuatorManager(){ 
    initSignalHandler(actuatorManagerSignalHandler,1,SIGUSR1);
    sem_init(&actuatorSem,0,0);
    setupToOutput();
    actuatorManager();

}
void actuatorManager(){
    while(1){
        sem_wait(&actuatorSem);
        printf("actuator manager RECEIVED THE SIGNAL\n");
        readLineToWaterShm(idWaterShm,lines);
        watering(lines);
    }
}
void actuatorManagerSignalHandler(int signal, siginfo_t *info){
    switch(signal){
        case SIGUSR1 :
            sem_post(&actuatorSem);
            break;
        default :
            break;  
    }
}