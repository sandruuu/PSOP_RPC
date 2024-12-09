#include "connectToClient.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int activeThreads = 0;

Queue requestsQueue;
int requestQueue_size = 0;
pthread_mutex_t requestsQueue_mutex = PTHREAD_MUTEX_INITIALIZER;

Queue processedQueue;
int processedQueue_size = 0;
pthread_mutex_t processedQueue_mutex = PTHREAD_MUTEX_INITIALIZER;

int generateID()
{
    static int id = 0;
    id++;
    return id;
}

int callAsyncFunction(Packet *packet)
{
    int id = generateID();
    Clear(packet);
    packet->packetType = ACK;
    AppendInt(packet, id);
    return id;
}

void callSumFunction(Packet *packet)
{
    uint32_t value_1;
    uint32_t value_2;
    ExtractInt(packet, &value_1);
    ExtractInt(packet, &value_2);

    int returnVal = add((int)value_1, (int)value_2);
    printf("%d\n", returnVal);
    Clear(packet);
    AppendInt(packet, returnVal);
}

void callFunction(Packet *packet)
{
    char *data;
    ExtractString(packet, &data);
    if (strcmp(data, "add") == 0)
    {
        callSumFunction(packet);
    }
    packet->packetType = ACK;
}

void createSocket(TCPServerConn* serverConn)
{
    serverConn->sockFD = socket(AF_INET, SOCK_STREAM, 0);
    if (serverConn->sockFD == -1)
    {
        perror("[createSocket(TCPServerConn*)] - ");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Socket successfully created..\n");
        bzero(&serverConn->serveraddr, sizeof(serverConn->serveraddr));
    }
}
void initSocket(TCPServerConn* serverConn)
{
    serverConn->serveraddr.sin_family = AF_INET;
    serverConn->serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverConn->serveraddr.sin_port = htons(PORT);
}
void Bind(TCPServerConn* serverConn)
{
    if (bind(serverConn->sockFD, (struct sockaddr *)&serverConn->serveraddr,
             sizeof(serverConn->serveraddr)) != 0)
    {
        perror("[Bind(TCPServerConn*)] - ");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Socket successfully binded..\n");
    }
}
void closeClientConnection(int sockFD)
{
    close(sockFD);
}
void closeServerConnection(int sockFD)
{
    close(sockFD);
}
void Listen(TCPServerConn* serverConn)
{
    if (listen(serverConn->sockFD, 5) != 0)
    {
        perror("[Listen(TCPServerConn*)] - ");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Server listening...\n");
    }
}
void Accept(TCPServerConn* serverConn, TCPClientConn* clientConn)
{
    clientConn->len = sizeof(clientConn->clientaddr);
    clientConn->sockFD = accept(serverConn->sockFD,
                            (struct sockaddr *)&clientConn->clientaddr, &(clientConn->len));
    if (clientConn->sockFD < 0)
    {
        perror("[Accept(TCPServerConn*, TCPClientConn*)] - ");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Server accepted connection...\n");
    }
}
void processPacket(Packet *packet, int sockFD)
{
    switch (packet->packetType)
    {
    case SENDSYNC:
        callFunction(packet);
        int sendBytes = write(sockFD, packet, sizeof(*packet));
        if (sendBytes < 0)
        {
            perror("[processPacket(Packet*)] - ");
            exit(EXIT_FAILURE);
        }
        closeClientConnection(sockFD);
        free(packet);
        break;
        
    case SENDASYNC:
        Packet *sendPacket = (Packet *)malloc(sizeof(Packet));
        int id = callAsyncFunction(sendPacket);
        sendPacket->packetType = ACK;
        sendBytes = send(sockFD, sendPacket, sizeof(Packet), 0);
        if (sendBytes < 0)
        {
            perror("[processPacket(Packet*)] - ");
            exit(EXIT_FAILURE);
        }

        closeClientConnection(sockFD);
        free(sendPacket);
        
        pthread_mutex_lock(&requestsQueue_mutex);
        printf("pachet primit: %d\n", packet->currentSize);
        pushQueue(&requestsQueue, packet, id, &requestQueue_size);
        printf("%d req queue size\n", requestQueue_size);
        pthread_mutex_unlock(&requestsQueue_mutex);

        break;
    case REQUEST:
        uint32_t extractedID;
        ExtractInt(packet, &extractedID);
        printf("%d id extr\n", extractedID);
        pthread_mutex_lock(&processedQueue_mutex);
        Packet *extractedPacket = extractPacketById(&processedQueue, extractedID, &processedQueue_size);
        pthread_mutex_unlock(&processedQueue_mutex);

        if (extractedPacket != NULL)
        {
            printf("am extras pachetul\n");
            fflush(stdout);
            extractedPacket->packetType = ACK;
            int sendBytes = write(sockFD, extractedPacket, sizeof(Packet));
            if (sendBytes < 0)
            {
                perror("[processPacket(Packet*)] - ");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            fflush(stdout);
            printf("nu exista pachet");
        }

        break;
    default:
        break;
    }
}
void *threadRecv(void* argv)
{
    Packet* packet = (Packet*)malloc(sizeof(Packet));
    Clear(packet);
    int sockFD = *(int *)argv;
    int readBytes = read(sockFD, packet, sizeof(Packet));
    packet->extractionOffset = ntohl(packet->extractionOffset);
    packet->currentSize = ntohl(packet->currentSize);
    
    if (readBytes < 0)
    {
        perror("[threadRecv(void*)] - ");
        exit(EXIT_FAILURE);
    }
    processPacket(packet, sockFD);
    
    return NULL;
}
void createThreadRecv(TCPClientConn* clientConn)
{
    pthread_t id;
    int *sockFDPtr = malloc(sizeof(int));
    *sockFDPtr = clientConn->sockFD;
    pthread_create(&id, NULL, threadRecv, (void *)sockFDPtr);
}
void chatWithClients(TCPServerConn* serverConn)
{
    createSocket(serverConn);
    initSocket(serverConn);
    Bind(serverConn);
    Listen(serverConn);
    while (1)
    {
        TCPClientConn *clientConn = (TCPClientConn *)malloc(sizeof(TCPClientConn));
        Accept(serverConn, clientConn);
        createThreadRecv(clientConn);
    }
}

void *threadProcessPacket(void *arg)
{
    Node * queueNode = (Node*)arg;
    if (queueNode != NULL)
    {
        callFunction(queueNode->packet);
        pthread_mutex_lock(&processedQueue_mutex);
        pushQueue(&processedQueue, queueNode->packet, queueNode->id, &processedQueue_size);
        pthread_mutex_unlock(&processedQueue_mutex);
        printf("pachet procesat\n");
    }
    else
    {
        printf("pachet neprocesat\n");
    }

    pthread_mutex_lock(&mutex);
    activeThreads--;
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void* manageRequestsQueue(void* arg){
    requestsQueue.firstNode=NULL;
    processedQueue.firstNode=NULL;
    while (1)
    {
        pthread_mutex_lock(&mutex);
        pthread_mutex_lock(&requestsQueue_mutex);
        if (requestQueue_size > 0 && activeThreads < WORKING_THREADS)
        {
            Node *extractedNode = popQueue(&requestsQueue, &requestQueue_size);
            pthread_t id;
            pthread_create(&id, NULL, threadProcessPacket, extractedNode);
            activeThreads++;
            printf("if\n");
        }
        pthread_mutex_unlock(&requestsQueue_mutex);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}