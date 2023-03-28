#include <semaphore.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include <cjson/cJSON.h>



#define SENSOR_MANAGER 1
#define ACTUATOR_MANAGER 2
#define STATE_MACHINE 3

#define LOG_FILE "contreaul.log"
#define CONF_FILE "contreaulConf.json"

typedef struct {
    uint8_t I2CAddress;
    uint8_t I2CPin;
} I2CAnalogSensorPin;

typedef struct {
    int nbLines;
    unsigned char value[255]; //this is static because with shared memory we can't use malloc
} humiditySensors;

typedef struct {
    humiditySensors hSensors;
    unsigned char waterLevel;
    unsigned char phLevel;
    unsigned char temperature;
} log;

typedef struct {
    int nbLines;
    unsigned char line[255];
} lineToWater;

typedef struct {
    int humidityCalibration[255];
    float phCoeff;
    float phOffset;
    float pressureCoeff;
    float pressureOffset; 
} sensorCalibration;

typedef struct{
    I2CAnalogSensorPin humidityPins[255];
    I2CAnalogSensorPin phPin;
    I2CAnalogSensorPin pressurePin;
} sensorsPinConfiguration;

typedef struct {
    uint8_t pumpPin;
    uint8_t waterValvePins[255];
} actuatorsPinConfiguration;