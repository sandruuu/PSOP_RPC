#ifndef SERVER_STUB_H
#define SERVER_STUB_H

#include <stdio.h>
#include <stdlib.h>
#include <aio.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dlfcn.h>
#include <arpa/inet.h>
#include <stdbool.h>

#include "../Utils/packet.h"

typedef struct savedPacket {
    int id;
    Packet packet;
    bool ready;
} savedPacket;

void callFunction(Packet* packet);
int callAsyncFunction(Packet* packet);
#endif