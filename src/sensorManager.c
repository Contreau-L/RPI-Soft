#include <semaphore.h>
#ifndef SENSORMANAGER_H
    #include "../include/sensorManager.h"
    #define SENSORMANAGER_H 1
#endif
#ifndef LIBSHAREDMEMORY_H
    #include "../include/libSharedMemory.h"
    #define LIBSHAREDMEMORY_H 1
#endif

log logToWrite;
sem_t sensorSem;
extern int idLogShm;

void initSensorManager () { 
    initSignalHandler(sensorManagerSignalHandler,1,SIGALRM);
    sem_init(&sensorSem,0,0);
    sensorManager();

}
void sensorManager () {
    while(1){
        sem_wait(&sensorSem);
        //TODO take samples


        writeLogShm(idLogShm,&logToWrite);
    }
}

void sensorManagerSignalHandler(int signal, siginfo_t *info){
    switch(signal){
        case SIGALRM :
            sem_post(&sensorSem);
            //TODO take samples
            break;
        default :
            break;  
    }

}