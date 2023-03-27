#include <stdio.h>

#ifndef CONTREAUL_H
    #include "../include/contreaul.h"
    #define CONTREAUL_H 1 
#endif
#ifndef LIBUTILS_H
    #include "../include/libUtils.h"
    #define LIBUTILS_H 1
#endif
#ifndef STATEMACHINE_H
    #include "../include/stateMachine.h"
    #define STATEMACHINE_H 1
#endif
#ifndef LIBSHAREDMEMORY_H
    #include "../include/libSharedMemory.h"
    #define LIBSHAREDMEMORY_H 1
#endif
#ifndef SENSORMANAGER_H
    #include "../include/sensorManager.h"
    #define SENSORMANAGER_H 1
#endif
#ifndef ACTUATORMANAGER_H
    #include "../include/actuatorManager.h"
    #define ACTUATORMANAGER_H 1
#endif

int pidSensorManager = 0;
int pidActuatorManager = 0;
int idWaterShm;
int idLogShm;

int main() {
    printf("main of contreaul, let's create all processes\n");
    readContreaulConf();
    idLogShm = createlogSharedMemory();
    idWaterShm = createLineToWaterSharedMemory();
    
    switch(createFork(&pidSensorManager, &pidActuatorManager)){
        case STATE_MACHINE:
            printf("I'm the state machine %d\n",getpid());
            initStateMachine();
            break;
        case SENSOR_MANAGER:
            printf("I'm the sensor manager %d\n",getpid());
            initSensorManager();
            break;
        case ACTUATOR_MANAGER:
            printf("I'm the actuator manager %d\n",getpid());
            initActuatorManager();
            break;
    }
    return 0;
}
