#include "connectToClient.h"

savedPacket savedPackets[MAXSAVEDPACKET];
int savedPacketIndex = 0;

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
        break;
        
    case SENDASYNC:
        Packet *sendPacket = (Packet *)malloc(sizeof(Packet));
        int id = callAsyncFunction(sendPacket);
        sendBytes = write(sockFD, sendPacket, sizeof(*sendPacket));
        if (sendBytes < 0)
        {
            perror("[processPacket(Packet*)] - ");
            exit(EXIT_FAILURE);
        }
        closeClientConnection(sockFD);
        free(sendPacket);

        callFunction(packet);
        if (savedPacketIndex < MAXSAVEDPACKET)
        {
            savedPackets[savedPacketIndex].id = id;
            savedPackets[savedPacketIndex].packet = *packet;
            savedPacketIndex++;
        }
        break;

    case REQUEST:
        uint32_t extractedID;
        ExtractInt(packet, &extractedID);

        for (int i = 0; i < savedPacketIndex; i++)
        {
            if (savedPackets[i].id == extractedID)
            {
                int sendBytes = write(sockFD, &(savedPackets[i].packet), sizeof(savedPackets[i].packet));
                if (sendBytes < 0)
                {
                    exit(EXIT_FAILURE);
                }
                for (int j = i; j < savedPacketIndex - 1; j++)
                {
                    savedPackets[j] = savedPackets[j + 1];
                }
                savedPacketIndex--;
            }
        }
        break;

    default:
        break;
    }
}

void *threadRecv(void* argv)
{
    Packet packet;
    Clear(&packet);
    int sockFD = *(int *)argv;
    int readBytes = read(sockFD, &packet, sizeof(packet));
    packet.extractionOffset = ntohl(packet.extractionOffset);
    packet.currentSize = ntohl(packet.currentSize);
    
    if (readBytes < 0)
    {
        perror("[threadRecv(void*)] - ");
        exit(EXIT_FAILURE);
    }

    processPacket(&packet, sockFD);
    
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