#include <semaphore.h>
#ifndef SENSORMANAGER_H
    #include "../include/sensorManager.h"
    #define SENSORMANAGER_H 1
#endif
#ifndef LIBSHAREDMEMORY_H
    #include "../include/libSharedMemory.h"
    #define LIBSHAREDMEMORY_H 1
#endif
#ifndef LIBSENSOR_
    #include "../include/libSensorManager.h"
#define LIBSENSOR_H 1
#endif

log newLog;
sem_t sensorSem;
extern int idLogShm;

void initSensorManager () { 
    initSignalHandler(sensorManagerSignalHandler,1,SIGALRM);
    sem_init(&sensorSem,0,0);
    sensorManager();

}
void sensorManager () {
    while(1){
        alarm(10);
        sem_wait(&sensorSem);
        newLog.phLevel = 10*readPhValue();
        printf("ph level : %d\n",newLog.phLevel);
        newLog.waterLevel = 10*readWaterLevelValue();
        printf("water level : %d\n",newLog.waterLevel);
        newLog.temperature = 10*readTemperatureValue();
        readHumidityValues(newLog.hSensorsValue);
        writeLogShm(idLogShm,&newLog);
    }
}

void sensorManagerSignalHandler(int signal, siginfo_t *info){
    switch(signal){
        case SIGALRM :
            sem_post(&sensorSem);
            break;
        default :
            break;  
    }

}