#ifndef LIBSTATEMACHINE_H
#include "../include/libStateMachine.h"
#define LIBSTATEMACHINE_H 1
#endif

extern int NB_HUMIDITY_SENSORS;
int checkLinesHumidity(log *log, uint8_t *actions)
{
    int flag = 0;
    uint8_t *thresholds;
    readThresholds(&thresholds);
    for (int i = 0; i < NB_HUMIDITY_SENSORS; i++)
    {
        if (log->hSensorsValue[i] < thresholds[i] && actions[i] == 1)
        {
            actions[i] = 1;
            flag = 1;
        }
        else
        {
            actions[i] = 0;
        }
    }
    if (log->waterLevel < 5)
    {
        printf("water level too low\n");
        flag = -1;
    }
    return flag;
}

int mapActionsToLines(char *data, int len, char **actions)
{
    *actions = malloc(NB_HUMIDITY_SENSORS * sizeof(char));
    memset(*actions, 0, NB_HUMIDITY_SENSORS);
    if (len == 1 && data[0] == NOTHING)
    {
        printf("nothing to do\n");
        return 0;
    }
    for (int i = 0; i < len; i++)
    {
        (*actions)[data[i] - 1] = 1;
    }
    return 1;
}

void copyExecutedAction(char **actionsToFill, char *actionsToRead)
{
    *actionsToFill = malloc(NB_HUMIDITY_SENSORS * sizeof(char));
    memcpy(*actionsToFill, actionsToRead, NB_HUMIDITY_SENSORS);
}
