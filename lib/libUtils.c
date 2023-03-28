#ifndef LIBUTILS_H
    #include "../include/libUtils.h"
    #define LIBUTILS_H 1
#endif


int NB_HUMIDITY_SENSOR;
sensorCalibration sensorCalib;
sensorsPinConfiguration sensorsPinConfig;
actuatorsPinConfiguration actuatorsPinConfig;
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
        sigaction(currentSignal, &action, NULL);
    }
    va_end(signalList);
}

int createForks(int *pidSensorManager, int *pidActuatorManager){
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
        for(int i = *len; i < NB_HUMIDITY_SENSOR+*len ; i++){
            fscanf(file,"%hhu,",&((*tabToFill)[i]));
        }
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

    }while(!feof(file));
    fclose(file);
}

void _put2Bytes(uint8_t **tabToFill,int *len,uint16_t value){
    (*tabToFill)[*len] = value >> 8;
    (*tabToFill)[*len+1] = value & 0xFF;
    *len = *len + 2;
}


void readContreaulConf () {
    FILE *fp;
    char buffer[2000];
    cJSON *json;
    cJSON *sensors;
    cJSON *actuators;

    cJSON *I2Cpin;
    cJSON *I2Caddress;
    cJSON *coeff;
    cJSON *offset;
    cJSON *pin;

    cJSON *humiditySensor;
    cJSON *waterValve;
    int i = 0;

    fp = fopen(CONF_FILE, "r");
    if (fp == NULL) {
        perror("Error opening file");
        return;
    }
    fread(buffer, sizeof(char), sizeof(buffer) - 1, fp);
    fclose(fp);

    json = cJSON_Parse(buffer);
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        return;
    }
    cJSON *nb_humidity_sensor = cJSON_GetObjectItemCaseSensitive(json, "NB_HUMIDITY_SENSOR");
    if (cJSON_IsNumber(nb_humidity_sensor)) {
        NB_HUMIDITY_SENSOR = nb_humidity_sensor->valueint;
    }

    sensors = cJSON_GetObjectItemCaseSensitive(json, "sensors");


    cJSON *humiditySensors = cJSON_GetObjectItemCaseSensitive(sensors, "humiditySensors");
    cJSON_ArrayForEach(humiditySensor, humiditySensors) {
        I2Cpin = cJSON_GetObjectItemCaseSensitive(humiditySensor, "I2Cpin");
        sensorsPinConfig.humidityPins[i].I2CPin = I2Cpin->valueint;
        I2Caddress = cJSON_GetObjectItemCaseSensitive(humiditySensor, "I2Caddress");
        sscanf(I2Caddress->valuestring,"%hhx",&(sensorsPinConfig.humidityPins[i].I2CAddress));
        cJSON *MAX = cJSON_GetObjectItemCaseSensitive(humiditySensor, "MAX");
        sensorCalib.humidityCalibration[i] = MAX->valueint;
        i++;  
    }


    cJSON *phMeter = cJSON_GetObjectItemCaseSensitive(sensors, "phMeter");
    I2Cpin = cJSON_GetObjectItemCaseSensitive(phMeter, "I2Cpin");
    sensorsPinConfig.phPin.I2CPin = I2Cpin->valueint;
    I2Caddress = cJSON_GetObjectItemCaseSensitive(phMeter, "I2Caddress");
    sscanf(I2Caddress->valuestring,"%hhx",&(sensorsPinConfig.phPin.I2CAddress));
    coeff = cJSON_GetObjectItemCaseSensitive(phMeter, "coeff");
    sensorCalib.phCoeff = coeff->valuedouble;
    offset = cJSON_GetObjectItemCaseSensitive(phMeter, "offset");
    sensorCalib.phOffset = offset->valuedouble;


    cJSON *pressureSensor = cJSON_GetObjectItemCaseSensitive(sensors, "pressureSensor");
    I2Cpin = cJSON_GetObjectItemCaseSensitive(pressureSensor, "I2Cpin");
    sensorsPinConfig.pressurePin.I2CPin = I2Cpin->valueint;
    I2Caddress = cJSON_GetObjectItemCaseSensitive(pressureSensor, "I2Caddress");
    sscanf(I2Caddress->valuestring,"%hhx",&(sensorsPinConfig.pressurePin.I2CAddress));
    coeff = cJSON_GetObjectItemCaseSensitive(pressureSensor, "coeff");
    sensorCalib.pressureCoeff = coeff->valuedouble;
    offset = cJSON_GetObjectItemCaseSensitive(pressureSensor, "offset");
    sensorCalib.pressureOffset = offset->valuedouble;

    //actuators now
    actuators = cJSON_GetObjectItemCaseSensitive(json, "actuators");

    cJSON *pump = cJSON_GetObjectItemCaseSensitive(actuators, "pump");
    pin = cJSON_GetObjectItemCaseSensitive(pump, "pin");
    actuatorsPinConfig.pumpPin = I2Cpin->valueint;

    //for each for water valves
    cJSON *waterValves = cJSON_GetObjectItemCaseSensitive(actuators, "waterValves");
    i = 0;
    cJSON_ArrayForEach(waterValve, waterValves) {
        pin = cJSON_GetObjectItemCaseSensitive(waterValve, "pin");
        actuatorsPinConfig.waterValvePins[i] = pin->valueint;
        i++;
    }
    //print everything
    printf("NB_HUMIDITY_SENSOR: %d\n",NB_HUMIDITY_SENSOR);
    for(int i = 0 ; i < NB_HUMIDITY_SENSOR ; i++){
        printf("humiditySensor %d: I2Cpin: %d, I2Caddress: 0x%02x, MAX: %d\n",i,sensorsPinConfig.humidityPins[i].I2CPin,sensorsPinConfig.humidityPins[i].I2CAddress,sensorCalib.humidityCalibration[i]);
    }

    printf("phMeter: I2Cpin: %d, I2Caddress: 0x%02x, coeff: %f, offset: %f\n",sensorsPinConfig.phPin.I2CPin,sensorsPinConfig.phPin.I2CAddress,sensorCalib.phCoeff,sensorCalib.phOffset);

    printf("pressureSensor: I2Cpin: %d, I2Caddress: 0x%02x, coeff: %f, offset: %f\n",sensorsPinConfig.pressurePin.I2CPin,sensorsPinConfig.pressurePin.I2CAddress,sensorCalib.pressureCoeff,sensorCalib.pressureOffset);

    for(int i = 0; i < NB_HUMIDITY_SENSOR ; i++){
        printf("waterValve %d: pin: %d\n",i,actuatorsPinConfig.waterValvePins[i]);
    }
    printf("pump: pin: %d\n",actuatorsPinConfig.pumpPin);
}
