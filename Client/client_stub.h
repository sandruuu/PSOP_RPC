#ifndef CLIENT_STUB_H
#define CLIENT_STUB_H
#define _GNU_SOURCE 500
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include "asincCall.h"

#define MAXTHREADS 100

typedef struct IPEndpoint {
    int Port;
    char* IPAddress;
} IPEndpoint;

typedef struct TCPConnection {
    IPEndpoint* endpoint;
    int sockfd;
    Packet* recvPacket;   
}TCPConnection;

typedef struct ThreadArgv{
    TCPConnection* connection;
    Packet* sendPacket;
    void (*callback)(Packet* recvPacket);
}ThreadArgv;

extern pthread_mutex_t recv_mutex;
extern uint32_t ClientId;


int add(TCPConnection* connection, int a, int b);
int multiply(TCPConnection* connection, int a, int b);
float max_array(TCPConnection* connection, float* array, int size);
void add_async(TCPConnection* connection, int a, int b);
void multiply_async(TCPConnection* connection, int a, int b);
void max_array_async(TCPConnection* connection, float* array, int size);
void initConnection(TCPConnection **connection);
int Connect(IPEndpoint *endpoint);
void CloseConnection(TCPConnection* connection);
void callAsyncFunction(TCPConnection *connection, Packet *sendPacket);
sendAsync* request_result(TCPConnection *connection,Packet* sendPck);

#endif