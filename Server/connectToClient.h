#include "libheaders.h"
#include "../Utils/packet.h"
#include "server_stub.h"
#include <pthread.h>

#define MAXSAVEDPACKET 100

typedef struct TCPClientConn {
    struct sockaddr_in clientaddr;     //stocheaza informatii despre adresa clientul conectat 
    socklen_t len;                  //dimensiunea structurii sockaddr_in     
    int sockFD;      //socketul unei conexiuni client-server acceptate 
}TCPClientConn;

typedef struct TCPServerConn {
    struct sockaddr_in serveraddr;     //stocheaza informatii despre adresa serverului 
    int sockFD;                    //socketul de ascultare 
}TCPServerConn;

void chatWithClients(TCPServerConn* serverConn);
void closeServerConnection(int sockFD);