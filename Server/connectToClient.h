#include "libheaders.h"
#include "server_stub.h"



#define MAXSAVEDPACKET 100

typedef struct TCP_clientSocket {
    struct sockaddr_in clientaddr;     //stocheaza informatii despre adresa clientul conectat 
    socklen_t len;                  //dimensiunea structurii sockaddr_in     
    int connfd;      //socketul unei conexiuni client-server acceptate 
}TCP_clientSocket;

typedef struct TCP_serverSocket {
    struct sockaddr_in serveraddr;     //stocheaza informatii despre adresa serverului 
    int sockfd;                    //socketul de ascultare 
}TCP_serverSocket;

//extern pthread_mutex_t recv_mutex;



void createSocket(TCP_serverSocket *server);
void initSocket(TCP_serverSocket *server);
void Bind(TCP_serverSocket *server);
void closeClientConnection(int client_fd);
void closeServerConnection(int server_fd);
void Listen(TCP_serverSocket *server);
void Accept(TCP_serverSocket *server, TCP_clientSocket *client);
void *threadRecv(void *argv);
void createThreadRecv(TCP_clientSocket *client);
