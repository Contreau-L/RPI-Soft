#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <net/if.h>

#ifndef CONTREAUL_H
#include "contreaul.h"
#define CONTREAUL_H 1
#endif

#define IOT_PORT 5000
#define IOT_URL "mariusc.best"
#define INTERFACE "wlan0" // to be changed for the raspberry
#define ID_FRAME_LEN 7
#define ACK 'a'
#define NOTHING 0xFF
#define END 0xFF
int DNSResolution();
int initSocket();
int getMacAddr(char *macAddr);
int sendToSocket(char *msg, int len);
int socketManager();
int sendIndentificationFrame();
void closeSocket();
int readSocket(char **buffer, int *len);
int goToNextFrame();