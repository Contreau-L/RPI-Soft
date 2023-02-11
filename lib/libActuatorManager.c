#ifndef LIBACTUATORMANAGER_H
    #include "../include/libActuatorManager.h"
    #define LIBACTUATORMANAGER_H 1
#endif

void actuatorManagerSignalHandler(int signal, siginfo_t *info){
    switch(signal){
        case SIGUSR1 :
            //TODO read shared mem to watered lines then
            break;
        default :
            break;  
    }

}