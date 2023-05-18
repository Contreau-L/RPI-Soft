
#ifndef LIBSENSOR_
    #include "../include/libSensorManager.h"
#define LIBSENSOR_H 1
#endif

extern int NB_HUMIDITY_SENSORS;
extern sensorCalibration sensorCalib;
extern sensorsPinConfiguration sensorsPinConfig;

double avergearray(int16_t* arr, int number){
  int i;
  int max,min;
  double avg;
  long amount=0;
  if(number<=0){
    printf("Error number for the array to averaging!\n");
    return 0;
  }
  if(number<5){   //less than 5, calculated directly statistics
    for(i=0;i<number;i++){
      amount+=arr[i];
    }
    avg = amount/number;
    return avg;
  }else{
    if(arr[0]<arr[1]){
      min = arr[0];max=arr[1];
    }
    else{
      min=arr[1];max=arr[0];
    }
    for(i=2;i<number;i++){
      if(arr[i]<min){
        amount+=min;        //arr<min
        min=arr[i];
      }else {
        if(arr[i]>max){
          amount+=max;    //arr>max
          max=arr[i];
        }else{
          amount+=arr[i]; //min<=arr<=max
        }
      }//if
    }//for
    avg = (double)amount/(number-2);
  }//if
  return avg;
}


float readPhValue() {
    int16_t pHArray[SAMPLES];
    int pHArrayIndex=0;
    float voltage,pHValue;
    setupI2C(sensorsPinConfig.phPin.I2CAddress);
    while(pHArrayIndex<SAMPLES)
        pHArray[pHArrayIndex++]=readADC_SingleEnded(sensorsPinConfig.phPin.I2CPin,GAIN_ONE,sensorsPinConfig.phPin.I2CAddress);

    voltage = avergearray(pHArray, SAMPLES)*GAIN_ONE_VALUE/MAX_VALUE;
    pHValue = 3.5*voltage;
    pHValue = sensorCalib.phCalibration.coeff*pHValue + sensorCalib.phCalibration.offset; //fonction affine, on a calibré avec un pH 7.0 et un pH 4.0
    printf("tension : %f / phValue: %f\n",voltage, pHValue);
    return pHValue;
}

float readWaterLevelValue() {
    int16_t pressureSensor[SAMPLES];
    int pressureSensorIndex=0;
    float voltage,level;
    setupI2C(sensorsPinConfig.pressurePin.I2CAddress);
    while(pressureSensorIndex<SAMPLES)
        pressureSensor[pressureSensorIndex++]=readADC_SingleEnded(sensorsPinConfig.pressurePin.I2CPin,GAIN_TWO,sensorsPinConfig.pressurePin.I2CAddress);
    voltage = avergearray(pressureSensor, SAMPLES)*GAIN_TWO_VALUE/MAX_VALUE;
    level = voltage * sensorCalib.pressureCalibration.coeff + sensorCalib.pressureCalibration.offset;
    printf("tension : %f / level: %f\n",voltage, level);
    return level;
}


void readHumidityValues(uint8_t* humiditySensors) {
    int humiditySensorIndex=0;
    double humidity = 0;
    int16_t samplesHumiditySensor[SAMPLES];
    for(int i = 0 ; i < NB_HUMIDITY_SENSORS;i++){
        setupI2C(sensorsPinConfig.humidityPins[i].I2CAddress);
        while(humiditySensorIndex<SAMPLES)
            samplesHumiditySensor[humiditySensorIndex++]=readADC_SingleEnded(sensorsPinConfig.humidityPins[i].I2CPin,GAIN_ONE,sensorsPinConfig.humidityPins[i].I2CAddress);
        humidity = avergearray(samplesHumiditySensor, SAMPLES);
        humiditySensors[i] = humidity/sensorCalib.humidityCalibration[i] * 100; //trasnformation in percentage
        if(humiditySensors[i] < 0){
            humiditySensors[i] = 0;
        }
        humiditySensorIndex = 0;
        printf("humidity sensor %d : %d\n",i, humiditySensors[i]);
    }
}

float readTemperatureValue() {
    char buffer[1000];
    int len = 0;
    char temp[10];
    float temperature;
    int fd = 0;
    char path[100];
    sprintf(path, "%s/%s/%s",PRE_PATH,sensorsPinConfig.tempSensorId,POST_PATH);
    if ((fd = open(path, O_RDONLY)) < 0) 
    {
        perror("Error when opening w1_slave");
        exit(1);
    }

    len=read(fd, buffer, sizeof(buffer));
    strncpy(temp, buffer+len-6, 5);	
    temperature=atof(temp)/1000;
    temperature = temperature * sensorCalib.tempCalibration.coeff + sensorCalib.tempCalibration.offset;
    close(fd);
    printf("Temp = %2.3f°C\n", temperature);
    return temperature;
        
}

