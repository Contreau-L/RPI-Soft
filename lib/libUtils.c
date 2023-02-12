#ifndef LIBUTILS_H
    #include "../include/libUtils.h"
    #define LIBUTILS_H 1
#endif



void initAllSemaphore(sem_t **semNetwork){
    *semNetwork = sem_open(NETWORK_SEMAPHORE, O_CREAT, 0666, 0);
}

void eraseAllSemaphore(){
    sem_unlink(NETWORK_SEMAPHORE);
}

void initSignalHandler(void (*handlerFunction)(int, siginfo_t*),int count, ...) {
    struct sigaction action;
    action.sa_sigaction = (void *)handlerFunction;
    action.sa_flags = SA_SIGINFO;

    sigemptyset(&action.sa_mask);
    sigprocmask(SIG_SETMASK, &action.sa_mask,NULL); 
    va_list signalList;
    va_start(signalList, count);

    int currentSignal;
    for (int i = 0; i < count; i++) {
        int currentSignal = va_arg(signalList, int);
        sigaction(currentSignal, &action, NULL);
    }
    va_end(signalList);
}

int createFork(int *pidSensorManager, int *pidActuatorManager){
    *pidSensorManager = fork();
    if(*pidSensorManager == 0) {
        return SENSOR_MANAGER;
    }
    else{
        *pidActuatorManager = fork();
        if(*pidActuatorManager == 0) {
            return ACTUATOR_MANAGER;
        }    
    }
    return STATE_MACHINE;
}


