#include <semaphore.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>
#include <signal.h>

#ifndef CONTREAUL_H
    #include "contreaul.h"
    #define CONTREAUL_H 1
#endif

#define NETWORK_SEMAPHORE "network-semaphore"


void initAllSemaphore(sem_t **semNetwork);
void eraseAllSemaphore();
int createFork(int *pidSensorManager, int *pidActuatorManager);
void initSignalHandler(void (*handlerFunction)(int, siginfo_t*),int count, ...);
