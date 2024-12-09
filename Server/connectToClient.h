#include "functions.h"
#include "../Utils/packet.h"
#include "Queue.h"

#define WORKING_THREADS 10

typedef struct TCPClientConn {
    struct sockaddr_in clientaddr;   
    socklen_t len;   
    int sockFD;
}TCPClientConn;

typedef struct TCPServerConn {
    struct sockaddr_in serveraddr; 
    int sockFD;
}TCPServerConn;

void chatWithClients(TCPServerConn* serverConn);
void closeServerConnection(int sockFD);
void* manageRequestsQueue(void* arg);