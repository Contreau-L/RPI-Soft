#ifndef LIBUTILS_H
    #include "../include/libUtils.h"
    #define LIBUTILS_H 1
#endif


int NB_HUMIDITY_SENSOR;

void initSignalHandler(void (*handlerFunction)(int, siginfo_t*),int count, ...) {
    struct sigaction action;
    action.sa_sigaction = (void *)handlerFunction;
    action.sa_flags = SA_SIGINFO;

    sigemptyset(&action.sa_mask);
    sigprocmask(SIG_SETMASK, &action.sa_mask,NULL); 
    va_list signalList;
    va_start(signalList, count);

    int currentSignal;
    for (int i = 0; i < count; i++) {
        int currentSignal = va_arg(signalList, int);
        printf("signal %d\n",currentSignal);
        sigaction(currentSignal, &action, NULL);
    }
    va_end(signalList);
}

int createFork(int *pidSensorManager, int *pidActuatorManager){
    *pidSensorManager = fork();
    if(*pidSensorManager == 0) {
        return SENSOR_MANAGER;
    }
    else{
        *pidActuatorManager = fork();
        if(*pidActuatorManager == 0) {
            return ACTUATOR_MANAGER;
        }    
    }
    return STATE_MACHINE;
}


void writeLogToFile(log *logToWrite){
    FILE *file = fopen(LOG_FILE, "a");
    if(file == NULL){
        printf("Error opening file!\n");
        exit(1);
    }
    fprintf(file,"%d,",NB_HUMIDITY_SENSOR);
    for(int i = 0; i < logToWrite->hSensors.nbLines ; i++){
        fprintf(file,"%d,",logToWrite->hSensors.value[i]);
    }
    fprintf(file,"%d,%d,%d",logToWrite->temperature,logToWrite->waterLevel,logToWrite->phLevel);
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    fprintf(file,",%d/%d/%d-%d:%d",tm.tm_mday, tm.tm_mon + 1, tm.tm_year+1900, tm.tm_hour, tm.tm_min);
    fprintf(file,"\n");
    fclose(file);
}


void readLogFile(uint8_t **tabToFill,int *len){
    *len = 0;
    FILE *file = fopen(LOG_FILE, "r");
    uint16_t read2Bytes;
    if(file == NULL){
        printf("Error opening file!\n");
        exit(1);
    }
    do{

        *tabToFill = realloc(*tabToFill, (*len+14+NB_HUMIDITY_SENSOR)*sizeof(uint8_t));
        (*tabToFill)[(*len)++] = 0x00; //type of frame
        fscanf(file,"%hhu,",&((*tabToFill)[(*len)++]));
        printf("len %d\n",*len);
        for(int i = *len; i < NB_HUMIDITY_SENSOR+*len ; i++){
            fscanf(file,"%hhu,",&((*tabToFill)[i]));
            printf("%d " , (*tabToFill)[i]);

        }
        printf("\n");
        *len += NB_HUMIDITY_SENSOR;
        for(int i = 0 ; i < 2 ; i++){ //temperature and water level
            fscanf(file,"%hu,",&read2Bytes);
            _put2Bytes(tabToFill,len,read2Bytes);
        }
        fscanf(file,"%hhu,",&((*tabToFill)[(*len)++])); // ph level
        //date 
        fscanf(file,"%hhu/%hhu/",&((*tabToFill)[(*len)++]),&((*tabToFill)[(*len)++]));
        fscanf(file,"%hu-",&read2Bytes); // year
        _put2Bytes(tabToFill,len,read2Bytes);
        fscanf(file,"%hhu:%hhu",&((*tabToFill)[(*len)++]),&((*tabToFill)[(*len)++])); // hour and minute
        fscanf(file,"\n");
        for(int i = 0 ; i < *len ; i++){
            printf("%d ",(*tabToFill)[i]);
            if(i % 15 == 0)
                printf("\n");
        }
        printf("len = %d\n",*len);

    }while(!feof(file));
    fclose(file);
}

void _put2Bytes(uint8_t **tabToFill,int *len,uint16_t value){
    (*tabToFill)[*len] = value >> 8;
    (*tabToFill)[*len+1] = value & 0xFF;
    *len = *len + 2;
}


void readContreaulConf () {
    FILE *file = fopen(CONF_FILE, "r");
    if(file == NULL){
        printf("Error opening file!\n");
        exit(1);
    }
    fscanf(file,"%d",&NB_HUMIDITY_SENSOR);
    printf("NB_HUMIDITY_SENSOR = %d\n",NB_HUMIDITY_SENSOR);
    fclose(file);
}