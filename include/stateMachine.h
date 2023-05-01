#include <pthread.h>
#include <unistd.h>
#ifndef LIBUTILS_H
    #include "libUtils.h"
    #define LIBUTILS_H 1
#endif
#ifndef LIBSHAREDMEMORY_H
    #include "libSharedMemory.h"
    #define LIBSHAREDMEMORY_H 1
#endif
#ifndef LIBSOCKETHANDLER_H
    #include "libSocketHandler.h"
    #define LIBSOCKETHANDLER_H 1
#endif
#define READ_DATA  1
#define WATERING 2

void *networkManager(void *arg);
void stateMachineSignalHandler(int signal, siginfo_t *info);
void initStateMachine();
void stateMachine();
