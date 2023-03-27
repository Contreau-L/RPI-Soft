#ifndef LIBUTILS_H
    #include "libUtils.h"
    #define LIBUTILS_H 1
#endif

#define READ_DATA 1
#define SEND_DATA 2


void *networkManager(void *arg);
void stateMachineSignalHandler(int signal, siginfo_t *info);
void initStateMachine();
void stateMachine();
