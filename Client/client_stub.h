#ifndef CLIENT_STUB_H
#define CLIENT_STUB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include "../Utils/packet.h"

#define MAXTHREADS 100

typedef struct IPEndpoint {
    int Port;
    char* IPAddress;
} IPEndpoint;

typedef struct TCPConnection {
    IPEndpoint* endpoint;
    int sockFD;
    Packet* recvPacket;   
}TCPConnection;

int add(TCPConnection* connection, int a, int b);
int addAsync(TCPConnection* connection, int a, int b);
int addREQUEST(TCPConnection* connection, int packetID);

char* removeDuplicates(TCPConnection* connection, char* buffer, int size);
int removeDuplicatesAsync(TCPConnection *connection, char *buffer, int size);
char* removeDuplicatesREQUEST(TCPConnection *connection, int id);

int longestAscendingDigitNumber(TCPConnection *connection, int *arr, int size);
int calculateWordFrequency(TCPConnection *connection, char *buffer, int bufferSize, char *word, int wordSize);
float* rotateArray(TCPConnection *connection, float* arr, int size, int rotations, char* direction, int directionSize);
#endif