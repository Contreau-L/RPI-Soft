#ifndef LIBSTATEMACHINE_H
    #include "../include/libStateMachine.h"
    #define LIBSTATEMACHINE_H 1
#endif
extern int NB_HUMIDITY_SENSORS;
int checkLinesHumidity(log* log,uint8_t *actions){
    int i;
    int flag = 0;
    uint8_t *thresholds;
    readThresholds(&thresholds); 
    for(i = 0; i < NB_HUMIDITY_SENSORS; i++){
        if(log->hSensorsValue[i] < thresholds[i] && actions[i] == 1){ //todo replace 10 by thresholds
            actions[i] = 1;
            flag = 1;
        }
        else{
            actions[i] = 0;           
        }
    }
    return flag;
}

int mapActionsToLines(char *data, int len,char **actions){
    if(len == 0)
        return 0;
    *actions = malloc(NB_HUMIDITY_SENSORS*sizeof(char));
    memset(*actions,0, NB_HUMIDITY_SENSORS);
    for(int i = 0; i < len;i ++){
        (*actions)[data[i]] = 1;
    }
    return 1;
}

