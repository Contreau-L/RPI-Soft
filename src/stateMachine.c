#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
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
#ifndef LIBSOCKETHANDLER_H
    #include "../include/libSocketHandler.h"
    #define LIBSOCKETHANDLER_H 1
#endif

extern int pidSensorManager;
extern int pidActuatorManager;
extern int idWaterShm;
extern int idLogShm;
extern int NB_HUMIDITY_SENSORS;
pthread_t network;
sem_t networkSem;
sem_t stateMachineSem;
int measureCount = 0;
int state = 0;
log *logToFill;


void initStateMachine(){
    sem_init(&networkSem, 0, 0);
    sem_init(&stateMachineSem, 0, 0);
    initSignalHandler(stateMachineSignalHandler,1,SIGUSR1);
    pthread_create(&network, NULL, networkManager, NULL);
    stateMachine();
}



void stateMachine() {
    printf("state machine started\n");
    sem_post(&networkSem);
    while(1){
        sem_wait(&stateMachineSem);
        switch(state){
            case READ_DATA:
                printf("time to read data\n");
                readLogShm(idLogShm,logToFill);
                writeLogToFile(logToFill);
                if(measureCount == 3){
                    state = SEND_DATA;
                    measureCount = 0;
                    sem_post(&stateMachineSem);
                }
                else{
                    state = READ_DATA;
                    measureCount++;
                }
                break;
            case SEND_DATA:
                printf("time to send data\n");
                sem_post(&networkSem);
                state = READ_DATA;
                break;
            default:
                break;
        }
    }
}

void *networkManager(void *arg) {
    uint8_t *dataToSend;
    int len = 0;
    uint8_t end = 0xFF;
    char *buffer;
    int bufferLen;
    char ack = ACK;
    while(1) {
        sem_wait(&networkSem);
        printf("network manager started\n");
        readLogFile(&dataToSend, &len);
        printf("data to send : %d\n",len);
        if(socketManager()){
            if(goToNextFrame()){
                for(int i = 0; i < len; i += 13+NB_HUMIDITY_SENSORS){
                    if(sendToSocket(&dataToSend[i], 13+NB_HUMIDITY_SENSORS)){
                        printf("Data sent\n");
                    }
                    else{
                        printf("Error while sending data\n");
                    }
                    if(!goToNextFrame()){
                        printf("Error in ACK !\n");
                        break;
                    }
                }
            }
            else{
                printf("Error while sending the ID frame\n");
            }
            sendToSocket(&end,1);
            if(readSocket(&buffer, &bufferLen))
                registerThresholds(buffer);
            else{
                printf("Error no data received");
                closeSocket();
            }
            sendToSocket(&ack,1);
            if(readSocket(&buffer, &bufferLen)){
                //todo parse and launch actuators
            }
            closeSocket();

        }
        else{
            printf("Error while connecting to server\n");
        }
        break;
    }
}


void stateMachineSignalHandler(int signal, siginfo_t *info){
    switch(signal){
        case SIGUSR1 :
            sem_post(&stateMachineSem);
            break;
        default :
            break;  
    }
}
