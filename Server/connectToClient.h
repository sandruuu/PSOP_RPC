#include "../Utils/packet.h"
#include "../Utils/log.h"
#include "functions.h"
#include "queue.h"
#include <sys/time.h>

#define WORKING_THREADS 10

typedef struct TCPConnection {
    struct sockaddr_in addr; 
    int sockFD;
}TCPConnection;

void chatWithClients(TCPConnection* serverConn);
void closeServerConnection(int sockFD);
void* manageRequestsQueue(void* arg);