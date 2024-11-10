#include "client_stub.h"

int Connect(IPEndpoint *endpoint)
{
    int sockFD;
    struct sockaddr_in serv_addr;

    sockFD = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFD < 0)
    {
        perror("[Connect(IPEndpoint*)] - ");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_port = htons(endpoint->Port);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(endpoint->IPAddress);

    if (connect(sockFD, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("[Connect(IPEndpoint*)] - ");
        close(sockFD);
        exit(EXIT_FAILURE);
    }

    return sockFD;
}

void callFunction(TCPConnection *connection, Packet *sendPacket)
{
    connection->sockFD = Connect(connection->endpoint);

    int sendBytes = send(connection->sockFD, sendPacket, sizeof(*sendPacket), 0);
    if (sendBytes < 0)
    {
        perror("[callFunction(TCPConnection*, Packet*)] - ");
        exit(EXIT_FAILURE);
    }

    Clear(connection->recvPacket);
    int recvBytes = recv(connection->sockFD, connection->recvPacket,
                         sizeof(*(connection->recvPacket)), 0);
    if (recvBytes < 0)
    {
        perror("[callFunction(TCPConnection*, Packet*)] - ");
        exit(EXIT_FAILURE);
    }

    shutdown(connection->sockFD, 2);
    close(connection->sockFD);
}

int callAsyncFunction(TCPConnection *connection, Packet *sendPacket)
{
    connection->sockFD = Connect(connection->endpoint);
    int sendBytes = send(connection->sockFD, sendPacket, sizeof(*(sendPacket)), 0);
    if (sendBytes < 0)
    {
        perror("[callAsyncFunction(TCPConnection*, Packet*)] - ");
        exit(EXIT_FAILURE);
    }
   
    int recvBytes = recv(connection->sockFD, connection->recvPacket,
                         sizeof(*(connection->recvPacket)), 0);
    if (recvBytes < 0)
    {
        perror("[callAsyncFunction(TCPConnection*, Packet*)] - ");
        exit(EXIT_FAILURE);
    }

    uint32_t id;
    ExtractInt(connection->recvPacket, &id);
    shutdown(connection->sockFD, 2);
    close(connection->sockFD);
    free(sendPacket);
    
    return id;
}

int addAsync(TCPConnection *connection, int a, int b)
{
    Packet* sendPacket = (Packet *)malloc(sizeof(Packet));
    Clear(sendPacket);
    AppendString(sendPacket, "add", strlen("add"));
    AppendInt(sendPacket, a);
    AppendInt(sendPacket, b);
    sendPacket->packetType = SENDASYNC;
    sendPacket->extractionOffset = htonl(sendPacket->extractionOffset);
    sendPacket->currentSize = htonl(sendPacket->currentSize);
    return callAsyncFunction(connection, sendPacket);
}

int add(TCPConnection *connection, int a, int b)
{
    Packet *sendPacket = (Packet *)malloc(sizeof(Packet));
    Clear(sendPacket);
    sendPacket->packetType = SENDSYNC;
    AppendString(sendPacket, "add", strlen("add"));
    AppendInt(sendPacket, a);
    AppendInt(sendPacket, b);
    sendPacket->extractionOffset = htonl(sendPacket->extractionOffset);
    sendPacket->currentSize = htonl(sendPacket->currentSize);
    callFunction(connection, sendPacket);

    uint32_t sum = 0;
    ExtractInt(connection->recvPacket, &sum);

    free(sendPacket);
    return sum;
}

int addREQUEST(TCPConnection* connection, int id){
    Packet *sendPacket = (Packet *)malloc(sizeof(Packet));
    Clear(sendPacket);
    sendPacket->packetType = REQUEST;
    AppendInt(sendPacket, id);
    callFunction(connection, sendPacket);
    uint32_t sum = 0;
    ExtractInt(connection->recvPacket, &sum);

    free(sendPacket);
    return sum;
}