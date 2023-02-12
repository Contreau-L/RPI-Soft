#ifndef LIBSHAREDMEMORY_H
    #include "../include/libSharedMemory.h"
    #define LIBSHAREDMEMORY_H 1
#endif


int createlogSharedMemory(){
    key_t key = ftok("makefile",454545);
    int sharedMemoryId;
    sharedMemoryId = shmget(key, sizeof(log), IPC_CREAT | 0666);
    if(sharedMemoryId < 0){
        perror("Error in creating shared memory 1");
        return -1;
    }
    return sharedMemoryId;
}
int createLineToWaterSharedMemory(){
    key_t key = ftok("makefile",2222);
    int sharedMemoryId;
    sharedMemoryId = shmget(key, sizeof(lineToWater), IPC_CREAT | 0666);
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

int writeLineToWaterShm(int sharedMemoryId, lineToWater *lineToWaterToWrite){
    lineToWater *sharedMemory = (lineToWater*)shmat(sharedMemoryId, NULL, 0);

    if(sharedMemory == (lineToWater*)-1){
        perror("Error in attaching shared memory");
        return -1;
    }
    _fillLineToWater(sharedMemory, lineToWaterToWrite);
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

int readLineToWaterShm(int sharedMemoryId, lineToWater *lineToWaterToRead){
    lineToWater *sharedMemory = (lineToWater*)shmat(sharedMemoryId, NULL, 0);

    if(sharedMemory == (lineToWater*)-1){
        perror("Error in attaching shared memory");
        return -1;
    }
    _fillLineToWater(lineToWaterToRead, sharedMemory);
    shmdt(sharedMemory);
    return 0;
}

void eraseSharedMemory(int sharedMemoryId){
    shmctl(sharedMemoryId, IPC_RMID, NULL);
}


void _fillLog(log *logToFill, log *logToRead){
    logToFill->hSensors.nbLines = logToRead->hSensors.nbLines;
    for(int i = 0; i < logToRead->hSensors.nbLines; i++){
        logToFill->hSensors.value[i] = logToRead->hSensors.value[i];
    }
    logToFill->waterLevel = logToRead->waterLevel;
    logToFill->phLevel = logToRead->phLevel;
    logToFill->temperature = logToRead->temperature;
}

void _fillLineToWater(lineToWater *lineToWaterToFill, lineToWater *toRead){
    lineToWaterToFill->nbLines = toRead->nbLines;
    for(int i = 0; i < toRead->nbLines; i++){
        lineToWaterToFill->line[i] = toRead->line[i];
    }
}