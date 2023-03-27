



#define SENSOR_MANAGER 1
#define ACTUATOR_MANAGER 2
#define STATE_MACHINE 3

#define LOG_FILE "contreaul.log"
#define CONF_FILE "contreaul.conf"
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