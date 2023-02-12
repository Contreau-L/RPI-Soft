#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#ifndef CONTREAUL_H
    #include "../include/contreaul.h"
    #define CONTREAUL_H 1 
#endif

int createlogSharedMemory();
int createLineToWaterSharedMemory();

void _fillLog(log *logToFill, log *logToRead);
void _fillLineToWater(lineToWater *lineToWaterToFill, lineToWater *toRead);
int writeLogShm(int sharedMemoryId, log *logToWrite);
int writeLineToWaterShm(int sharedMemoryId, lineToWater *lineToWaterToWrite);
int readLogShm(int sharedMemoryId, log *logToRead);
int readLineToWaterShm(int sharedMemoryId, lineToWater *lineToWaterToRead);
void eraseSharedMemory(int sharedMemoryId);