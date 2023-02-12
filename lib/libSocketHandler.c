#ifndef LIBSOCKETHANDLER_H
    #include "../include/libSocketHandler.h"
    #define LIBSOCKETHANDLER_H 1
#endif
int socketFd;
struct sockaddr_in server;


int initSocket () {
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd < 0) {
        perror("Error opening socket");
        return -1;
    }
    server.sin_family = AF_INET;
    if(DNSResolution() < 0) {
        perror("Error resolving host");
        return -1;
    }
    server.sin_port = htons(IOT_PORT);
    if (connect(socketFd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Error connecting to server");
        return -1;
    }
    return 0;

}


int DNSResolution () {
    struct hostent *host;
    host = gethostbyname(IOT_URL);
    if (host == NULL) {
        return -1;
    }
    printf("IP: %s\n", inet_ntoa(*((struct in_addr *)host->h_addr)));
    memcpy(&server.sin_addr.s_addr, host->h_addr_list[0], host->h_length);
    return 1;
}


int socketManager () {
    if(initSocket() < 0) {
        perror("Error initializing socket");
        return -1;
    }
    if(sendIndentificationFrame() < 0) {
        perror("Error sending identification frame");
        return -1;
    }
    return 1;
}


int sendIndentificationFrame () {
    char macAddr[6];
    char idFrame[7];

    if(getMacAddr(macAddr) == -1) {
        perror("Error getting MAC address");
        return -1;
    }
    memcpy(idFrame, macAddr, 6);
    idFrame[6] = NB_HUMIDITY_SENSOR; //TODO get the number of humidity sensors 
    if (sendToSocket(idFrame, ID_FRAME_LEN) == -1){
        perror("Error sending id Frame");
        return -1;
    }
    return 1;
}


int getMacAddr (char *macAddr) {
    struct ifreq s;
    int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
    strcpy(s.ifr_name, INTERFACE);
    if (ioctl(fd, SIOCGIFHWADDR, &s) == 0) {
        int i;
        for (i = 0; i < 6; i++){
            macAddr[i] = (unsigned char) s.ifr_addr.sa_data[i];
            printf("%0x",(unsigned char) macAddr[i]);
        }
        printf("\n");
    }
    else{
        return -1;
    }
    return 1;
}

int sendToSocket (char *msg, int len) {
    return write(socketFd, msg, len);
}