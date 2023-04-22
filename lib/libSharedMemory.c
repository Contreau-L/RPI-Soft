#ifndef LIBSHAREDMEMORY_H
    #include "../include/libSharedMemory.h"
    #define LIBSHAREDMEMORY_H 1
#endif

extern int NB_HUMIDITY_SENSORS;
int createlogSharedMemory(){
    key_t key = ftok("makefile",5555);
    int sharedMemoryId;
    sharedMemoryId = shmget(key, sizeof(log), IPC_CREAT | 0666);
    if(sharedMemoryId < 0){
        perror("Error in creating shared memory 1");
        return -1;
    }
    return sharedMemoryId;
}
int createLineToWaterSharedMemory(){
    key_t key = ftok("makefile",3333);
    int sharedMemoryId;
    sharedMemoryId = shmget(key, sizeof(uint8_t), IPC_CREAT | 0666);
    if(sharedMemoryId < 0){
        perror("Error in creating shared memory 2");
        return -1;
    }
    return sharedMemoryId;
}

int writeLogShm(int sharedMemoryId, log *logToWrite){
    log *sharedMemory = (log*)shmat(sharedMemoryId, NULL, 0);

    if(sharedMemory == (log*)-1){
        perror("Error in attaching shared memory");
        return -1;
    }
    _fillLog(sharedMemory, logToWrite);
    shmdt(sharedMemory);
    return 0;
}

int writeLineToWaterShm(int sharedMemoryId, uint8_t *lineToWaterToWrite){
    uint8_t *sharedMemory = (uint8_t*)shmat(sharedMemoryId, NULL, 0);

    if(sharedMemory == (uint8_t*)-1){
        perror("Error in attaching shared memory");
        return -1;
    }
    memcpy(sharedMemory, lineToWaterToWrite, NB_HUMIDITY_SENSORS);
    shmdt(sharedMemory);
    return 0;
}

int readLogShm(int sharedMemoryId, log *logToFill){
    log *sharedMemory = (log*)shmat(sharedMemoryId, NULL, 0);

    if(sharedMemory == (log*)-1){
        perror("Error in attaching shared memory");
        return -1;
    }
    _fillLog(logToFill, sharedMemory);
    shmdt(sharedMemory);
    return 0;
}

int readLineToWaterShm(int sharedMemoryId, uint8_t *lineToWaterToRead){
    uint8_t *sharedMemory = (uint8_t*)shmat(sharedMemoryId, NULL, 0);

    if(sharedMemory == (uint8_t*)-1){
        perror("Error in attaching shared memory");
        return -1;
    }
    memcpy(lineToWaterToRead, sharedMemory, NB_HUMIDITY_SENSORS);
    shmdt(sharedMemory);
    return 0;
}

void eraseSharedMemory(int sharedMemoryId){
    shmctl(sharedMemoryId, IPC_RMID, NULL);
}


void _fillLog(log *logToFill, log *logToRead){
    for(int i = 0; i < NB_HUMIDITY_SENSORS; i++){
        logToFill->hSensorsValue[i] = logToRead->hSensorsValue[i] ;
    }
    logToFill->waterLevel = logToRead->waterLevel;
    logToFill->phLevel = logToRead->phLevel;
    logToFill->temperature = logToRead->temperature;
}