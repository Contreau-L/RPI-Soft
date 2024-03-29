#ifndef LIBUTILS_H
#include "../include/libUtils.h"
#define LIBUTILS_H 1
#endif

int NB_HUMIDITY_SENSORS;
sensorCalibration sensorCalib;
sensorsPinConfiguration sensorsPinConfig;
actuatorsPinConfiguration actuatorsPinConfig;
void initSignalHandler(void (*handlerFunction)(int, siginfo_t *), int count, ...)
{
    struct sigaction action;
    action.sa_sigaction = (void *)handlerFunction;
    action.sa_flags = SA_SIGINFO | SA_RESTART;

    sigemptyset(&action.sa_mask);
    sigprocmask(SIG_SETMASK, &action.sa_mask, NULL);
    va_list signalList;
    va_start(signalList, count);

    int currentSignal;
    for (int i = 0; i < count; i++)
    {
        int currentSignal = va_arg(signalList, int);
        sigaction(currentSignal, &action, NULL);
    }
    va_end(signalList);
}

int createForks(int *pidSensorManager, int *pidActuatorManager)
{
    *pidSensorManager = fork();
    if (*pidSensorManager == 0)
    {
        return SENSOR_MANAGER;
    }
    else
    {
        *pidActuatorManager = fork();
        if (*pidActuatorManager == 0)
        {
            return ACTUATOR_MANAGER;
        }
    }
    return STATE_MACHINE;
}

void writeLogToFile(log *logToWrite)
{
    FILE *file = fopen(LOG_FILE, "a");
    if (file == NULL)
    {
        perror("Error opening file!\n");
        exit(1);
    }
    fprintf(file, "%d,", NB_HUMIDITY_SENSORS);
    for (int i = 0; i < NB_HUMIDITY_SENSORS; i++)
    {
        fprintf(file, "%d,", logToWrite->hSensorsValue[i]);
    }
    fprintf(file, "%d,%d,%d", logToWrite->temperature, logToWrite->waterLevel, logToWrite->phLevel);
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    fprintf(file, ",%d/%d/%d-%d:%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min);
    fprintf(file, "\n");
    fclose(file);
}

void readLogFile(uint8_t **tabToFill, int *len)
{
    *len = 0;
    FILE *file = fopen(LOG_FILE, "r");
    uint16_t read2Bytes;
    uint8_t nbLinesInLog;
    if (file == NULL)
    {
        perror("Error opening file!\n");
        exit(1);
    }
    *tabToFill = NULL;
    do
    {
        fscanf(file, "%hhu,", &nbLinesInLog);
        *tabToFill = realloc(*tabToFill, (*len + 14 + nbLinesInLog) * sizeof(uint8_t));
        (*tabToFill)[(*len)++] = 0x00;         // type of frame
        (*tabToFill)[(*len)++] = nbLinesInLog; // nb of lines in log
        for (int i = *len; i < nbLinesInLog + *len; i++)
        {
            fscanf(file, "%hhu,", &((*tabToFill)[i]));
        }
        *len += NB_HUMIDITY_SENSORS;
        for (int i = 0; i < 2; i++)
        { // temperature and water level
            fscanf(file, "%hu,", &read2Bytes);
            _put2Bytes(tabToFill, len, read2Bytes);
        }
        fscanf(file, "%hhu,", &((*tabToFill)[(*len)++])); // ph level
        // date
        fscanf(file, "%hhu/%hhu/", &((*tabToFill)[(*len)++]), &((*tabToFill)[(*len)++]));
        fscanf(file, "%hu-", &read2Bytes); // year
        _put2Bytes(tabToFill, len, read2Bytes);
        fscanf(file, "%hhu:%hhu", &((*tabToFill)[(*len)++]), &((*tabToFill)[(*len)++])); // hour and minute
        fscanf(file, "\n");

        // print
        for (int i = 0; i < *len; i++)
        {
            printf("%d ", (*tabToFill)[i]);
        }
        printf("\n");

    } while (!feof(file));
    fclose(file);
    remove(LOG_FILE);
}

int fillWateringStatusFrame(wateringResult wateringR, uint8_t *lines, uint8_t **frame)
{
    int offset = 0;
    int i = 0;
    *frame = malloc((NB_HUMIDITY_SENSORS + 15) * sizeof(uint8_t));
    (*frame)[offset++] = 0x01;
    (*frame)[offset++] = NB_HUMIDITY_SENSORS;

    struct tm tm = *localtime(&wateringR.timeStamp);
    // print the date
    printf("Date : %d/%d/%d-%d:%d\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min);
    (*frame)[offset++] = tm.tm_mday;
    (*frame)[offset++] = tm.tm_mon + 1;
    _put2Bytes(frame, &offset, tm.tm_year + 1900);
    (*frame)[offset++] = tm.tm_hour;
    (*frame)[offset++] = tm.tm_min;
    memset(&((*frame)[offset]), 1, NB_HUMIDITY_SENSORS * sizeof(uint8_t));
    if (wateringR.wateringStatus == 0)
    {
        for (i = 0; i < NB_HUMIDITY_SENSORS; i++)
        {
            if (lines[i] == 1)
            {
                (*frame)[i + offset] = 0; // set to false
            }
        }
    }
    return offset + NB_HUMIDITY_SENSORS;
}

void _put2Bytes(uint8_t **tabToFill, int *len, uint16_t value)
{
    (*tabToFill)[*len] = value >> 8;
    (*tabToFill)[*len + 1] = value & 0xFF;
    *len = *len + 2;
}

void readContreaulConf()
{
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
    cJSON *id;

    cJSON *humiditySensor;
    cJSON *waterValve;
    int i = 0;

    fp = fopen(CONF_FILE, "r");
    if (fp == NULL)
    {
        perror("Error opening file");
        return;
    }
    fread(buffer, sizeof(char), sizeof(buffer) - 1, fp);
    fclose(fp);

    json = cJSON_Parse(buffer);
    if (json == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        return;
    }
    cJSON *JSON_NB_HUMIDITY_SENSORS = cJSON_GetObjectItemCaseSensitive(json, "NB_HUMIDITY_SENSORS");
    if (cJSON_IsNumber(JSON_NB_HUMIDITY_SENSORS))
    {
        NB_HUMIDITY_SENSORS = JSON_NB_HUMIDITY_SENSORS->valueint;
    }

    sensors = cJSON_GetObjectItemCaseSensitive(json, "sensors");

    cJSON *humiditySensors = cJSON_GetObjectItemCaseSensitive(sensors, "humiditySensors");
    cJSON_ArrayForEach(humiditySensor, humiditySensors)
    {
        I2Cpin = cJSON_GetObjectItemCaseSensitive(humiditySensor, "I2Cpin");
        sensorsPinConfig.humidityPins[i].I2CPin = I2Cpin->valueint;
        I2Caddress = cJSON_GetObjectItemCaseSensitive(humiditySensor, "I2Caddress");
        sscanf(I2Caddress->valuestring, "%hhx", &(sensorsPinConfig.humidityPins[i].I2CAddress));
        cJSON *MAX = cJSON_GetObjectItemCaseSensitive(humiditySensor, "MAX");
        sensorCalib.humidityCalibration[i] = MAX->valueint;
        i++;
    }

    cJSON *phMeter = cJSON_GetObjectItemCaseSensitive(sensors, "phMeter");
    I2Cpin = cJSON_GetObjectItemCaseSensitive(phMeter, "I2Cpin");
    sensorsPinConfig.phPin.I2CPin = I2Cpin->valueint;
    I2Caddress = cJSON_GetObjectItemCaseSensitive(phMeter, "I2Caddress");
    sscanf(I2Caddress->valuestring, "%hhx", &(sensorsPinConfig.phPin.I2CAddress));
    coeff = cJSON_GetObjectItemCaseSensitive(phMeter, "coeff");
    sensorCalib.phCalibration.coeff = coeff->valuedouble;
    offset = cJSON_GetObjectItemCaseSensitive(phMeter, "offset");
    sensorCalib.phCalibration.offset = offset->valuedouble;

    cJSON *pressureSensor = cJSON_GetObjectItemCaseSensitive(sensors, "pressureSensor");
    I2Cpin = cJSON_GetObjectItemCaseSensitive(pressureSensor, "I2Cpin");
    sensorsPinConfig.pressurePin.I2CPin = I2Cpin->valueint;
    I2Caddress = cJSON_GetObjectItemCaseSensitive(pressureSensor, "I2Caddress");
    sscanf(I2Caddress->valuestring, "%hhx", &(sensorsPinConfig.pressurePin.I2CAddress));
    coeff = cJSON_GetObjectItemCaseSensitive(pressureSensor, "coeff");
    sensorCalib.pressureCalibration.coeff = coeff->valuedouble;
    offset = cJSON_GetObjectItemCaseSensitive(pressureSensor, "offset");
    sensorCalib.pressureCalibration.offset = offset->valuedouble;

    cJSON *tempSensor = cJSON_GetObjectItemCaseSensitive(sensors, "temperatureSensor");
    id = cJSON_GetObjectItemCaseSensitive(tempSensor, "id");
    strcpy(sensorsPinConfig.tempSensorId, id->valuestring);
    coeff = cJSON_GetObjectItemCaseSensitive(tempSensor, "coeff");
    sensorCalib.tempCalibration.coeff = coeff->valuedouble;
    offset = cJSON_GetObjectItemCaseSensitive(tempSensor, "offset");
    sensorCalib.tempCalibration.offset = offset->valuedouble;

    // actuators now
    actuators = cJSON_GetObjectItemCaseSensitive(json, "actuators");

    cJSON *pump = cJSON_GetObjectItemCaseSensitive(actuators, "pump");
    pin = cJSON_GetObjectItemCaseSensitive(pump, "pin");
    actuatorsPinConfig.pumpPin = pin->valueint;

    // for each for water valves
    cJSON *waterValves = cJSON_GetObjectItemCaseSensitive(actuators, "waterValves");
    i = 0;
    cJSON_ArrayForEach(waterValve, waterValves)
    {
        pin = cJSON_GetObjectItemCaseSensitive(waterValve, "pin");
        actuatorsPinConfig.waterValvePins[i] = pin->valueint;
        i++;
    }
    // print everything
    printf("NB_HUMIDITY_SENSORS : %d\n", NB_HUMIDITY_SENSORS);
    printf("| TEMP SENSOR ID : %s | ", sensorsPinConfig.tempSensorId);
    printf("| TEMP SENSOR COEFF : %f| ", sensorCalib.tempCalibration.coeff);
    printf("| TEMP SENSOR OFFSET : %f| \n", sensorCalib.tempCalibration.offset);

    printf("| PRESSURE SENSOR COEFF : %f| ", sensorCalib.pressureCalibration.coeff);
    printf("| PRESSURE SENSOR OFFSET : %f| \n", sensorCalib.pressureCalibration.offset);

    printf("| PH SENSOR COEFF : %f| ", sensorCalib.phCalibration.coeff);
    printf("| PH SENSOR OFFSET : %f| \n", sensorCalib.phCalibration.offset);

    for (int i = 0; i < NB_HUMIDITY_SENSORS; i++)
    {
        printf("| HUMIDITY SENSOR %d I2C PIN : %d| ", i, sensorsPinConfig.humidityPins[i].I2CPin);
        printf("| HUMIDITY SENSOR %d I2C ADDRESS : %x| ", i, sensorsPinConfig.humidityPins[i].I2CAddress);
        printf("| HUMIDITY SENSOR %d MAX : %d| \n", i, sensorCalib.humidityCalibration[i]);
    }
    printf("| PUMP PIN : %d| \n", actuatorsPinConfig.pumpPin);

    for (int i = 0; i < NB_HUMIDITY_SENSORS; i++)
    {
        printf("| WATER VALVE %d PIN : %d| \n", i, actuatorsPinConfig.waterValvePins[i]);
    }
}

void registerThresholds(char *data)
{
    // todo register it in JSON
    FILE *fp;
    cJSON *jsonObj;
    cJSON *thresholds;

    jsonObj = cJSON_CreateObject();
    if (jsonObj == NULL)
        return;

    thresholds = cJSON_CreateArray();
    for (int i = 0; i < NB_HUMIDITY_SENSORS; i++)
    {
        printf("%02x ", data[i]);
        cJSON_AddItemToArray(thresholds, cJSON_CreateNumber(data[i]));
    }
    cJSON_AddItemToObject(jsonObj, "thresholds", thresholds);

    char *string = cJSON_Print(jsonObj);
    if (string == NULL)
    {
        perror("Failed to print monitor.\n");
        return;
    }
    fp = fopen(TH_FILE, "w");
    if (fp == NULL)
    {
        perror("Error opening file");
        return;
    }
    fprintf(fp, "");
    fprintf(fp, "%s", string);
    fclose(fp);
    return;
}

void readThresholds(uint8_t **thresholdsValues)
{
    FILE *fp;
    char buffer[1024];
    fp = fopen(TH_FILE, "r");
    if (fp == NULL)
    {
        perror("Error opening file");
        return;
    }
    fread(buffer, 1024, 1, fp);
    fclose(fp);
    cJSON *json = cJSON_Parse(buffer);
    *thresholdsValues = malloc(sizeof(uint8_t) * NB_HUMIDITY_SENSORS);
    cJSON *thresholds = cJSON_GetObjectItemCaseSensitive(json, "thresholds");
    int i = 0;
    cJSON *threshold;
    cJSON_ArrayForEach(threshold, thresholds)
    {
        (*thresholdsValues)[i] = threshold->valueint;
        i++;
    }
    printf("THRESHOLDS : ");
    for (int i = 0; i < NB_HUMIDITY_SENSORS; i++)
    {
        printf("%d ", (*thresholdsValues)[i]);
    }
    printf("\n");
}