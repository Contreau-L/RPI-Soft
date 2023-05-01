#ifndef LIBUTILS_H
    #include "libUtils.h"
    #define LIBUTILS_H 1
#endif
#ifndef LIBSOCKETHANDLER_H
    #include "libSocketHandler.h"
    #define LIBSOCKETHANDLER_H 1
#endif
int checkLinesHumidity(log* log,uint8_t *actions);
int mapActionsToLines(char *data, int len,char **actions);