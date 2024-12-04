#include "connectToClient.h"


//pthread_mutex_t recv_mutex = PTHREAD_MUTEX_INITIALIZER;

void createSocket(TCP_serverSocket *server)
{
    server->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server->sockfd == -1)
    {
        perror("[createSocket(TCP_serverSocket *server)] - ");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Socket successfully created..\n");
        bzero(&server->serveraddr, sizeof(server->serveraddr));
    }
}

void initSocket(TCP_serverSocket *server)
{
    server->serveraddr.sin_family = AF_INET;
    server->serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    server->serveraddr.sin_port = htons(PORT);
}

void Bind(TCP_serverSocket *server)
{

    if (bind(server->sockfd, (struct sockaddr *)&server->serveraddr,
             sizeof(server->serveraddr)) != 0)
    {
        perror("[Bind(TCP_serverSocket* server)] - ");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Socket successfully binded..\n");
    }
}

void closeClientConnection(int client_fd)
{
    shutdown(client_fd,SHUT_RDWR);
    close(client_fd);
    printf("Connection closed for fd: %d\n", client_fd);
}

void closeServerConnection(int server_fd)
{
    close(server_fd);
}

void Listen(TCP_serverSocket *server)
{
    //serverul poate accespta maxim 5 cereri, celelalte vor fi refuzate 
    if (listen(server->sockfd, 5) != 0)
    {
        perror("[Listen(TCP_serverSocket* server)] - ");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Server listening...\n");
    }
}

void Accept(TCP_serverSocket *server, TCP_clientSocket *client)
{
    client->len = sizeof(client->clientaddr);
    client->connfd = accept(server->sockfd,
                            (struct sockaddr *)&client->clientaddr, &(client->len));
    if (client->connfd < 0)
    {
        perror("[Accept(TCP_serverSocket* server, TCP_clientSocket* client )] - ");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Server accepted connection...\n");
    }
}

void *threadRecv(void *argv)
{
    while(1)
    {
        Packet *packet = (Packet*)malloc(sizeof(Packet));
        if(packet == NULL)
        {
            perror("malloc:\n");
            exit(EXIT_FAILURE);
        }
        Clear(packet);

        //se citesc datele transmise de client prin socket 

        int connfd = *(int *)argv;
        //pthread_mutex_lock(&recv_mutex);
        int readBytes = read(connfd, packet, sizeof(*packet));
        //pthread_mutex_unlock(&recv_mutex);

        packet->extractionOffset = ntohl(packet->extractionOffset);
        packet->currentSize = ntohl(packet->currentSize);

        if (readBytes < 0)
        {
            perror("[threadRecv(void *argv)] - ");
            exit(EXIT_FAILURE);
        }

        processPacket(packet, connfd);
    }
    pthread_exit(NULL);
    
}

void createThreadRecv(TCP_clientSocket *client)
{
    
    pthread_t id;
    int *sockFDPtr = malloc(sizeof(int));
    *sockFDPtr = client->connfd;

    pthread_create(&id, NULL, threadRecv, (void *)sockFDPtr);
    pthread_detach(id); 
}
