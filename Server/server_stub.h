#ifndef SERVER_STUB_H
#define SERVER_STUB_H

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
#include "asincCall.h"

// typedef struct savedPacket {
//     int id;
//     Packet packet;
//     bool ready;

// } savedPacket;
extern pthread_mutex_t dlopen_mtx;

void callFunction(Packet* packet);
void generateAsyncPacket(sendAsync* sendStruct,Packet* packet);
void callSumFunction(Packet *packet);
void callMultiplyFunction(Packet *packet);
void callMaxArrayFunction(Packet *packet);
#endif