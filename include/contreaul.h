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
    float offset;
    float coeff;
} calibration;

typedef struct {
    uint8_t I2CAddress;
    uint8_t I2CPin;
} I2CAnalogSensorPin;


typedef struct {
    uint8_t hSensorsValue[255];
    uint16_t waterLevel;
    uint8_t phLevel;
    uint16_t temperature;
} log;

typedef struct {
    int humidityCalibration[255];
    calibration phCalibration;
    calibration pressureCalibration;
    calibration tempCalibration;
} sensorCalibration;

typedef struct{
    I2CAnalogSensorPin humidityPins[255];
    I2CAnalogSensorPin phPin;
    I2CAnalogSensorPin pressurePin;
    char tempSensorId[100];
} sensorsPinConfiguration;

typedef struct {
    uint8_t pumpPin;
    uint8_t waterValvePins[255];
} actuatorsPinConfiguration;