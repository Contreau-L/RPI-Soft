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
int state = READ_DATA;
int needToWater = 0;
log sensorsLog;
char *actionsHistory;
char *actionsToExecute;
int wateringState = -2;
int networkErrorCount = 0;
wateringResult wateringR;

void initStateMachine()
{
    sem_init(&networkSem, 0, 0);
    sem_init(&stateMachineSem, 0, 0);
    initSignalHandler(stateMachineSignalHandler, 3, SIGUSR1, SIGALRM, SIGINT);
    pthread_create(&network, NULL, networkManager, NULL);
    stateMachine();
}

void stateMachine()
{
    printf("state machine started\n");
    alarm(1);
    while (1)
    {
        sem_wait(&stateMachineSem);
        switch (state)
        {
        case READ_DATA:
            printf("read data %d\n", measureCount);
            kill(pidSensorManager, SIGUSR1);
            sem_wait(&stateMachineSem);        // wait for him to be done
            readLogShm(idLogShm, &sensorsLog); // read the log
            writeLogToFile(&sensorsLog);
            if (measureCount == 3)
            {
                measureCount = 0;
                sem_post(&networkSem);
            }
            else
            {
                measureCount++;
            }
            if (networkErrorCount == 3)
            {
                printf("Network error count reached 3, autonomous mod\n");
                actionsToExecute = malloc(NB_HUMIDITY_SENSORS * sizeof(char));
                memset(actionsToExecute, 1, NB_HUMIDITY_SENSORS); // we set all to 1 to water all the lines
                state = WATERING;
                networkErrorCount = 0;
            }
            alarm(1);
            break;
        case WATERING:
            printf("watering\n");
            kill(pidSensorManager, SIGUSR1);
            sem_wait(&stateMachineSem);        // wait for him to be done
            readLogShm(idLogShm, &sensorsLog); // read the log
            wateringState = checkLinesHumidity(&sensorsLog, actionsToExecute);
            if (wateringState == 1)
            {
                state = WATERING;
                alarm(5);
            }
            else
            {
                printf("Stop watering now \n");
                wateringR.wateringStatus = wateringState == -1 ? 0 : 1;
                copyExecutedAction(&actionsHistory, actionsToExecute);
                if (wateringState == -1)
                { // no water available so we set all to 0 to stop the watering
                    memset(actionsToExecute, 0, NB_HUMIDITY_SENSORS);
                }
                wateringR.timeStamp = time(NULL);
                state = READ_DATA;
                alarm(2);
            }
            writeLineToWaterShm(idWaterShm, actionsToExecute);
            kill(pidActuatorManager, SIGUSR1);
            break;
        }
    }
}

void *networkManager(void *arg)
{
    uint8_t *dataToSend;
    int len = 0;
    uint8_t end = END;
    char *buffer;
    int bufferLen;
    uint8_t ack = ACK;
    while (1)
    {
    waiting_for_connection:
        sem_wait(&networkSem);
        printf("network manager started\n");
        if (socketManager() == 1)
        {
            if (goToNextFrame())
            {
                if (wateringState != -2)
                {
                    wateringState = -2;
                    int len = fillWateringStatusFrame(wateringR, actionsHistory, &dataToSend);
                    if (sendToSocket(dataToSend, len))
                    {
                        printf("Watering status sent\n");
                    }
                    else
                    {
                        printf("Error while sending watering status\n");
                        closeSocket();
                        goto waiting_for_connection;
                    }
                    if (!goToNextFrame())
                    {
                        printf("Error in ACK !\n");
                        closeSocket();
                        free(actionsHistory);
                        actionsHistory = NULL;
                        free(dataToSend);
                        dataToSend = NULL;
                        goto waiting_for_connection;
                    }
                    free(actionsHistory);
                    actionsHistory = NULL;
                    free(dataToSend);
                    dataToSend = NULL;
                }
                readLogFile(&dataToSend, &len);
                printf("data to send : %d\n", len);
                for (int i = 0; i < len; i += CST_DATA_LEN + NB_HUMIDITY_SENSORS)
                {
                    if (sendToSocket(&dataToSend[i], CST_DATA_LEN + NB_HUMIDITY_SENSORS))
                    {
                        printf("Data sent\n");
                    }
                    else
                    {
                        printf("Error while sending data\n");
                    }
                    if (!goToNextFrame())
                    {
                        printf("Error in ACK !\n");
                    }
                }
                sendToSocket(&end, 1);
                if (readSocket(&buffer, &bufferLen))
                {
                    registerThresholds(buffer);
                }
                else
                {
                    printf("Error no data received");
                    closeSocket();
                    goto waiting_for_connection;
                }
                sendToSocket(&ack, 1);
                if (readSocket(&buffer, &bufferLen))
                {
                    if (mapActionsToLines(buffer, bufferLen, &actionsToExecute))
                    {
                        state = WATERING;
                    }
                    else
                    {
                        state = READ_DATA;
                    }
                }
                else
                {
                    printf("data received for the actions\n");
                    closeSocket();
                    goto waiting_for_connection;
                }
                closeSocket();
            }
            else
            {
                printf("Error no ack for the ID frame\n");
                closeSocket();
                free(dataToSend);
                dataToSend = NULL;
                goto waiting_for_connection;
            }
            free(dataToSend);
            dataToSend = NULL;
        }
        else
        {
            printf("Error while connecting to server\n");
            networkErrorCount++;
        }
    }
}

void stateMachineSignalHandler(int signal, siginfo_t *info)
{
    switch (signal)
    {
    case SIGUSR1:
        sem_post(&stateMachineSem);
        break;
    case SIGALRM:
        sem_post(&stateMachineSem);
        break;
    case SIGINT:
        kill(pidSensorManager, SIGINT);
        kill(pidActuatorManager, SIGINT);
        exit(0);
        break;
    default:
        break;
    }
}