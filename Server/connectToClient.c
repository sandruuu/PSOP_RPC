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
    struct timeval tv;
    gettimeofday(&tv, NULL);

    srand((unsigned int)(tv.tv_sec + tv.tv_usec));
    
    int length = (rand() % 2) + 5;
    int id = 0;

    for (int i = 0; i < length; i++)
    {
        id = id * 10 + (rand() % 10);
    }

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
    Clear(packet);
    AppendInt(packet, returnVal);
}
void callRemoveDuplicatesFunction(Packet *packet)
{
    char *buffer = NULL;
    uint32_t size;
    ExtractString(packet, &buffer);
    ExtractInt(packet, &size);
    removeDuplicates(buffer, (int *)&size);
    Clear(packet);

    AppendString(packet, buffer, size);

    free(buffer);
}
void callLongestAscendingDigitNumber(Packet *packet)
{
    int *arr = NULL;
    uint32_t size;
    ExtractIntArray(packet, &arr, &size);
    int data = longestAscendingDigitNumber(arr, (int)size);
    Clear(packet);

    AppendInt(packet, data);

    free(arr);
}
void callCalculateWordFrequency(Packet *packet)
{
    char *buffer = NULL;
    ExtractString(packet, &buffer);
    char *word = NULL;
    ExtractString(packet, &word);
    Clear(packet);

    int data = calculateWordFrequency(buffer, word);
    AppendInt(packet, data);

    free(buffer);
    free(word);
}
void callRotateArray(Packet *packet)
{
    float *arr = NULL;
    uint32_t size;
    ExtractFloatArray(packet, &arr, &size);
    uint32_t positions;
    ExtractInt(packet, &positions);
    char *direction = NULL;
    ExtractString(packet, &direction);

    rotateArray(arr, (int)size, positions, direction);

    Clear(packet);

    AppendFloatArray(packet, arr, size);

    free(arr);
    free(direction);
}

void callFunction(Packet *packet)
{
    char *data = NULL;
    ExtractString(packet, &data);

    if (strcmp(data, "add") == 0)
    {
        callSumFunction(packet);
    }

    if (strcmp(data, "removeDuplicates") == 0)
    {
        callRemoveDuplicatesFunction(packet);
    }

    if (strcmp(data, "longestAscendingDigitNumber") == 0)
    {
        callLongestAscendingDigitNumber(packet);
    }

    if (strcmp(data, "calculateWordFrequency") == 0)
    {
        callCalculateWordFrequency(packet);
    }

    if (strcmp(data, "rotateArray") == 0)
    {
        callRotateArray(packet);
    }

    free(data);
    packet->packetType = ACK;
}

void createSocket(TCPConnection *serverConn)
{
    serverConn->sockFD = socket(AF_INET, SOCK_STREAM, 0);
    if (serverConn->sockFD == -1)
    {
        log_error("[ createSocket(TCPServerConn*) ] - Socket failed to create.");
        exit(EXIT_FAILURE);
    }
    else
    {
        log_info("Socket successfully created..");
        bzero(&serverConn->addr, sizeof(serverConn->addr));
    }
}
void initSocket(TCPConnection *serverConn)
{
    serverConn->addr.sin_family = AF_INET;
    serverConn->addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverConn->addr.sin_port = htons(PORT);
}
void Bind(TCPConnection *serverConn)
{
    if (bind(serverConn->sockFD, (struct sockaddr *)&serverConn->addr,
             sizeof(serverConn->addr)) != 0)
    {
        log_error("[ Bind(TCPServerConn*) ] - Socket failed to bind.");
        exit(EXIT_FAILURE);
    }
    else
    {
        log_info("Socket successfully binded..");
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
void Listen(TCPConnection *serverConn)
{
    if (listen(serverConn->sockFD, 5) != 0)
    {
        log_error("[ Listen(TCPServerConn*) ] - Server failed to listen");
        exit(EXIT_FAILURE);
    }
    else
    {
        log_info("Server listening...");
    }
}
void Accept(TCPConnection *serverConn, TCPConnection *clientConn)
{
    socklen_t len = sizeof(clientConn->addr);
    clientConn->sockFD = accept(serverConn->sockFD,
                                (struct sockaddr *)&clientConn->addr, &len);
    if (clientConn->sockFD < 0)
    {
        log_error("[ Accept(TCPServerConn*, TCPClientConn*) ] - Server failed to accept connection.");
        exit(EXIT_FAILURE);
    }
    else
    {
        log_info("Server accepted connection...");
    }
}
void processPacket(Packet *packet, int sockFD)
{
    switch (packet->packetType)
    {
    case SYNC:
        log_info("Server received sync mode packet.");
        callFunction(packet);
        int sendBytes = send(sockFD, packet, sizeof(*packet), 0);
        if (sendBytes < 0)
        {
            log_error("[ processPacket(Packet*) ] - Server failed to send packet.");
            exit(EXIT_FAILURE);
        } else {
            log_info("Server sent packet.");
        }
        closeClientConnection(sockFD);
        free(packet);
        break;

    case ASYNC:
        log_info("Server received async mode packet.");
        Packet *sendPacket = (Packet *)malloc(sizeof(Packet));
        int id = callAsyncFunction(sendPacket);
        sendPacket->packetType = ACK;
        sendBytes = send(sockFD, sendPacket, sizeof(Packet), 0);
        if (sendBytes < 0)
        {
            log_error("[ processPacket(Packet*) ] - Server failed to send ACK packet containing id %d.", id);
            exit(EXIT_FAILURE);
        } else {
            log_info("Server sent an ACK packet containing ID %d.", id);
        }

        closeClientConnection(sockFD);
        free(sendPacket);

        pthread_mutex_lock(&requestsQueue_mutex);
        pushQueue(&requestsQueue, packet, id, &requestQueue_size);
        log_info("Packet with id %d added to requests queue.", id);
        pthread_mutex_unlock(&requestsQueue_mutex);

        break;
    case REQUEST:
        uint32_t extractedID;
        ExtractInt(packet, &extractedID);
        log_info("Server received request packet with id %d.", (int)extractedID);
        pthread_mutex_lock(&processedQueue_mutex);
        Packet *extractedPacket = extractPacketById(&processedQueue, extractedID, &processedQueue_size);
        pthread_mutex_unlock(&processedQueue_mutex);

        if (extractedPacket != NULL)
        {
            log_info("Packet with id %d extracted from processed queue.", (int)extractedID);
            extractedPacket->packetType = ACK;
            int sendBytes = write(sockFD, extractedPacket, sizeof(Packet));
            if (sendBytes < 0)
            {
                log_error("[ processPacket(Packet*) ] - Server failed to send packet with id %d.", (int)extractedID);
                exit(EXIT_FAILURE);
            } else {
                log_info("Server sent packet with id %d.", (int)extractedID);
            }
        }
        else
        {
            log_info("Packet with id %d not found in processed queue.", (int)extractedID);
        }

        break;
    default:
        break;
    }
}

void *threadRecv(void *argv)
{
    Packet *packet = (Packet *)malloc(sizeof(Packet));
    Clear(packet);
    int sockFD = *(int *)argv;
    int readBytes = read(sockFD, packet, sizeof(Packet));
    packet->extractionOffset = ntohl(packet->extractionOffset);
    packet->currentSize = ntohl(packet->currentSize);

    if (readBytes < 0)
    {
        log_error("[ threadRecv(void*) ] - Server failed to receive incoming packet.");
        exit(EXIT_FAILURE);
    }
    processPacket(packet, sockFD);

    return NULL;
}
void createThreadRecv(TCPConnection *clientConn)
{
    pthread_t id;
    int *sockFDPtr = malloc(sizeof(int));
    *sockFDPtr = clientConn->sockFD;
    pthread_create(&id, NULL, threadRecv, (void *)sockFDPtr);
}
void chatWithClients(TCPConnection *serverConn)
{
    createSocket(serverConn);
    initSocket(serverConn);
    Bind(serverConn);
    Listen(serverConn);
    while (1)
    {
        TCPConnection *clientConn = (TCPConnection *)malloc(sizeof(TCPConnection));
        Accept(serverConn, clientConn);
        createThreadRecv(clientConn);
    }
}

void *threadProcessPacket(void *arg)
{
    Node *queueNode = (Node *)arg;
    if (queueNode != NULL)
    {
        callFunction(queueNode->packet);
        pthread_mutex_lock(&processedQueue_mutex);
        log_info("Packet with id %d added in processed queue.", queueNode->id);
        pushQueue(&processedQueue, queueNode->packet, queueNode->id, &processedQueue_size);
        pthread_mutex_unlock(&processedQueue_mutex);
    }

    pthread_mutex_lock(&mutex);
    activeThreads--;
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void *manageRequestsQueue(void *arg)
{
    requestsQueue.firstNode = NULL;
    processedQueue.firstNode = NULL;
    while (1)
    {
        pthread_mutex_lock(&mutex);
        pthread_mutex_lock(&requestsQueue_mutex);
        if (requestQueue_size > 0 && activeThreads < WORKING_THREADS)
        {
            Node *extractedNode = popQueue(&requestsQueue, &requestQueue_size);
            log_info("Packet extracted from requests queue.");
            pthread_t id;
            pthread_create(&id, NULL, threadProcessPacket, extractedNode);
            activeThreads++;
        }
        pthread_mutex_unlock(&requestsQueue_mutex);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}