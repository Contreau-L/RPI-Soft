#ifndef STATEMACHINE_H
    #include "../include/stateMachine.h"
    #define STATEMACHINE_H 1
#endif
#ifndef LIBSTATEMACHINE_H
    #include "../include/libStateMachine.h"
    #define LIBSTATEMACHINE_H 1
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
int needToWater = 0;
log *sensorsLog;
char *actions;
int wateringState = -1;
wateringResult wateringR;

void initStateMachine(){
    sem_init(&networkSem, 0, 0);
    sem_init(&stateMachineSem, 0, 0);
    initSignalHandler(stateMachineSignalHandler,3,SIGUSR1,SIGALRM,SIGINT);
    pthread_create(&network, NULL, networkManager, NULL);
    stateMachine();
}


void stateMachine() {
    printf("state machine started\n");
    alarm(10);
    while(1){
        sem_wait(&stateMachineSem);
        switch(state){
            case READ_DATA : 
                kill(pidSensorManager,SIGUSR1); 
                sem_wait(&stateMachineSem); //wait for him to be done
                readLogShm(idLogShm,sensorsLog); //read the log
                writeLogToFile(sensorsLog);
                if(measureCount == 3){
                    measureCount = 0;
                    sem_post(&networkSem);
                }
                else{
                    measureCount++;
                }
                alarm(10);
                break;
            case WATERING :
                printf("watering\n");
                kill(pidSensorManager,SIGUSR1); 
                sem_wait(&stateMachineSem); //wait for him to be done
                readLogShm(idLogShm,sensorsLog); //read the log
                wateringState = checkLinesHumidity(sensorsLog,actions);
                if(wateringState){
                    state = WATERING;
                    alarm(5);
                }
                else{
                    wateringR.wateringStatus = wateringState == -1 ? 0 : 1;
                    wateringR.timeStamp = time(NULL);
                    state = READ_DATA;
                    alarm(10);
                }
                writeLineToWaterShm(idWaterShm,actions);
                kill(pidActuatorManager,SIGUSR1);
                break;
        }
    }
}

void *networkManager(void *arg) {
    uint8_t *dataToSend;
    int len = 0;
    uint8_t end = END;
    char *buffer;
    int bufferLen;
    uint8_t ack = ACK;
    while(1) {
        sem_wait(&networkSem);
        printf("network manager started\n");
        printf("data to send : %d\n",len);
        if(socketManager()){
            if(goToNextFrame()){
                if(wateringState != -1){
                    int len = fillWateringStatusFrame(wateringR,actions,&dataToSend);
                    if(sendToSocket(buffer, len)){
                        printf("Watering status sent\n");
                    }
                    else{
                        printf("Error while sending watering status\n");
                    }
                    if(!goToNextFrame()){
                        printf("Error in ACK !\n");
                        closeSocket();
                        break;
                    }
                    free(dataToSend);
                }
                readLogFile(&dataToSend, &len);
                for(int i = 0; i < len; i += CST_DATA_LEN+NB_HUMIDITY_SENSORS){
                    if(sendToSocket(&dataToSend[i], CST_DATA_LEN+NB_HUMIDITY_SENSORS)){
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
                sendToSocket(&end,1);
                if(readSocket(&buffer, &bufferLen)){
                    registerThresholds(buffer);
                }
                else{
                    printf("Error no data received");
                    closeSocket();
                }
                sendToSocket(&ack,1);
                if(readSocket(&buffer, &bufferLen)){
                    if(mapActionsToLines(buffer,bufferLen,&actions)){
                        state = WATERING;
                    }
                    else{
                        state = READ_DATA;
                    }
                }
                else{
                    printf("data received for the actions\n");
                }
                closeSocket();
            }
            else{
                printf("Error no ack for the ID frame\n");
            }
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
        case SIGALRM :
            sem_post(&stateMachineSem);
            break;
        case SIGINT :
            kill(pidSensorManager,SIGKILL);
            kill(pidActuatorManager,SIGKILL);
            exit(0);
            break;
        default :
            break;  
    }
}
