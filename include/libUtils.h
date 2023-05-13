
#ifndef CONTREAUL_H
    #include "contreaul.h"
    #define CONTREAUL_H 1
#endif


int createForks(int *pidSensorManager, int *pidActuatorManager);
void initSignalHandler(void (*handlerFunction)(int, siginfo_t*),int count, ...);
void initSensorSem(sem_t **sensorSem);
void initActuatorSem(sem_t **actuatorSem);
void writeLogToFile(log *logToWrite);
void readLogFile(uint8_t **tabToFill,int *len);
void readContreaulConf();
void _put2Bytes(uint8_t **tabToFill,int *len,uint16_t value);
void registerThresholds(char *data);
void readThresholds(uint8_t **thresholdsValues);
int fillWateringStatusFrame(wateringResult wateringR,uint8_t *lines,uint8_t **frame);