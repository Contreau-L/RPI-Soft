#ifndef LIBSENSORMANAGER_H
    #include "../include/libSensorManager.h"
    #define LIBSENSORMANAGER_H 1
#endif

void sensorManagerSignalHandler(int signal, siginfo_t *info){
    switch(signal){
        case SIGALRM :
            //TODO take samples
            break;
        default :
            break;  
    }

}
