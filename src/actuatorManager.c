#include <semaphore.h>
#ifndef ACTUATORMANAGER_H
    #include "../include/actuatorManager.h"
    #define ACTUATORMANAGER_H 1
#endif
#ifndef LIBSHAREDMEMORY_H
    #include "../include/libSharedMemory.h"
    #define LIBSHAREDMEMORY_H 1
#endif

sem_t actuatorSem;
lineToWater line;
extern int idWaterShm;

void initActuatorManager(){ 
    initSignalHandler(actuatorManagerSignalHandler,2,SIGUSR1,SIGALRM);
    sem_init(&actuatorSem,0,0);
    actuatorManager();

}
void actuatorManager(){
    while(1){
        sem_wait(&actuatorSem);
        readLineToWaterShm(idWaterShm,&line);
        //TODO read shared mem to watered lines then
        
    }
}
void actuatorManagerSignalHandler(int signal, siginfo_t *info){
    switch(signal){
        case SIGUSR1 :
            sem_post(&actuatorSem);
            //TODO read shared mem to watered lines then
            break;
        case SIGALRM :
            //stop watering
            break;
        default :
            break;  
    }
}