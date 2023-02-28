#include <stdio.h>
#include <pthread.h>

#ifndef CONTREAUL_H
    #include "../include/contreaul.h"
    #define CONTREAUL_H 1 
#endif
#ifndef LIBSOCKETHANDLER_H
    #include "../include/libSocketHandler.h"
    #define LIBSOCKETHANDLER_H 1
#endif
#ifndef LIBUTILS_H
    #include "../include/libUtils.h"
    #define LIBUTILS_H 1
#endif
#ifndef LIBSENSORMANAGER_H
    #include "../include/libSensorManager.h"
    #define LIBSENSORMANAGER_H 1
#endif
#ifndef LIBACTUATORMANAGER_H
    #include "../include/libActuatorManager.h"
    #define LIBACTUATORMANAGER_H 1
#endif
#ifndef LIBSHAREDMEMORY_H
    #include "../include/libSharedMemory.h"
    #define LIBSHAREDMEMORY_H 1
#endif

int pidSensorManager = 0;
int pidActuatorManager = 0;
pthread_t network;
sem_t *semNetwork;
int main() {
    printf("main of contreaul, let's create all processes\n");
    int idLogShm = createlogSharedMemory();
    int idWaterShm = createLineToWaterSharedMemory();
    switch(createFork(&pidSensorManager, &pidActuatorManager)){
        case STATE_MACHINE:
            printf("I'm the state machine\n");
            initAllSemaphore(&semNetwork);
            pthread_create(&network, NULL, networkThread, NULL);
            break;
        case SENSOR_MANAGER:
            printf("I'm the sensor manager %d\n",getppid());
            initSignalHandler(sensorManagerSignalHandler,1,SIGALRM);
            break;
        case ACTUATOR_MANAGER:
            printf("I'm the actuator manager %d\n",getppid());
            initSignalHandler(actuatorManagerSignalHandler,1,SIGUSR1);
            break;
    }
    return 0;
}



void *networkThread(void *arg) {
    sem_wait(semNetwork);
}