#include <semaphore.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#ifndef CONTREAUL_H
    #include "contreaul.h"
    #define CONTREAUL_H 1
#endif

int createFork(int *pidSensorManager, int *pidActuatorManager);
void initSignalHandler(void (*handlerFunction)(int, siginfo_t*),int count, ...);
void initSensorSem(sem_t **sensorSem);
void initActuatorSem(sem_t **actuatorSem);
void writeLogToFile(log *logToWrite);
void readLogFile(uint8_t **tabToFill,int *len);
void readContreaulConf();
void _put2Bytes(uint8_t **tabToFill,int *len,uint16_t value);
