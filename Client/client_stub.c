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
    int sendBytes = send(connection->sockFD, sendPacket, sizeof(Packet), 0);
    if (sendBytes < 0)
    {
        perror("[callFunction(TCPConnection*, Packet*)] - ");
        exit(EXIT_FAILURE);
    }
    Clear(connection->recvPacket);
    int recvBytes = recv(connection->sockFD, connection->recvPacket,
                         sizeof(Packet), 0);
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
    int sendBytes = send(connection->sockFD, sendPacket, sizeof(Packet), 0);
    if (sendBytes < 0)
    {
        perror("[callAsyncFunction(TCPConnection*, Packet*)] - ");
        exit(EXIT_FAILURE);
    }

    Clear(connection->recvPacket);

    int recvBytes = recv(connection->sockFD, connection->recvPacket,
                         sizeof(Packet), 0);
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
    Packet *sendPacket = (Packet *)malloc(sizeof(Packet));
    Clear(sendPacket);
    AppendString(sendPacket, "add", strlen("add"));
    AppendInt(sendPacket, a);
    AppendInt(sendPacket, b);
    sendPacket->packetType = ASYNC;
    sendPacket->extractionOffset = htonl(sendPacket->extractionOffset);
    sendPacket->currentSize = htonl(sendPacket->currentSize);

    return callAsyncFunction(connection, sendPacket);
}
int addRequest(TCPConnection *connection, int id)
{
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
int add(TCPConnection *connection, int a, int b)
{
    Packet *sendPacket = (Packet *)malloc(sizeof(Packet));
    Clear(sendPacket);
    sendPacket->packetType = SYNC;
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

char *removeDuplicates(TCPConnection *connection, char *buffer, int size)
{
    Packet *sendPacket = (Packet *)malloc(sizeof(Packet));
    Clear(sendPacket);
    sendPacket->packetType = SYNC;
    AppendString(sendPacket, "removeDuplicates", strlen("removeDuplicates"));
    AppendString(sendPacket, buffer, size);
    AppendInt(sendPacket, size);
    sendPacket->extractionOffset = htonl(sendPacket->extractionOffset);
    sendPacket->currentSize = htonl(sendPacket->currentSize);

    callFunction(connection, sendPacket);

    char *data;
    ExtractString(connection->recvPacket, &data);

    free(sendPacket);
    return data;
}
int removeDuplicatesAsync(TCPConnection *connection, char *buffer, int size)
{
    Packet *sendPacket = (Packet *)malloc(sizeof(Packet));
    Clear(sendPacket);
    sendPacket->packetType = ASYNC;
    AppendString(sendPacket, "removeDuplicates", strlen("removeDuplicates"));
    AppendString(sendPacket, buffer, size);
    AppendInt(sendPacket, size);
    sendPacket->extractionOffset = htonl(sendPacket->extractionOffset);
    sendPacket->currentSize = htonl(sendPacket->currentSize);

    return callAsyncFunction(connection, sendPacket);
}
char *removeDuplicatesRequest(TCPConnection *connection, int id)
{
    Packet *sendPacket = (Packet *)malloc(sizeof(Packet));
    Clear(sendPacket);
    sendPacket->packetType = REQUEST;
    AppendInt(sendPacket, id);
    callFunction(connection, sendPacket);

    char *data;
    ExtractString(connection->recvPacket, &data);

    free(sendPacket);
    return data;
}

int longestAscendingDigitNumber(TCPConnection *connection, int *arr, int size)
{
    Packet *sendPacket = (Packet *)malloc(sizeof(Packet));
    Clear(sendPacket);
    sendPacket->packetType = SYNC;
    AppendString(sendPacket, "longestAscendingDigitNumber", strlen("longestAscendingDigitNumber"));
    AppendIntArray(sendPacket, arr, size);
    sendPacket->extractionOffset = htonl(sendPacket->extractionOffset);
    sendPacket->currentSize = htonl(sendPacket->currentSize);

    callFunction(connection, sendPacket);

    uint32_t value = 0;
    ExtractInt(connection->recvPacket, &value);

    free(sendPacket);
    return value;
}
int longestAscendingDigitNumberAsync(TCPConnection *connection, int *arr, int size)
{
    Packet *sendPacket = (Packet *)malloc(sizeof(Packet));
    Clear(sendPacket);
    sendPacket->packetType = ASYNC;
    AppendString(sendPacket, "longestAscendingDigitNumber", strlen("longestAscendingDigitNumber"));
    AppendIntArray(sendPacket, arr, size);
    sendPacket->extractionOffset = htonl(sendPacket->extractionOffset);
    sendPacket->currentSize = htonl(sendPacket->currentSize);

    return callAsyncFunction(connection, sendPacket);
}
int longestAscendingDigitNumberRequest(TCPConnection *connection, int id)
{
    Packet *sendPacket = (Packet *)malloc(sizeof(Packet));
    Clear(sendPacket);
    sendPacket->packetType = REQUEST;
    AppendInt(sendPacket, id);
    callFunction(connection, sendPacket);

    uint32_t value = 0;
    ExtractInt(connection->recvPacket, &value);

    free(sendPacket);
    return value;
}

int calculateWordFrequency(TCPConnection *connection, char *buffer, int bufferSize, char *word, int wordSize)
{
    Packet *sendPacket = (Packet *)malloc(sizeof(Packet));
    Clear(sendPacket);
    sendPacket->packetType = SYNC;
    AppendString(sendPacket, "calculateWordFrequency", strlen("calculateWordFrequency"));
    AppendString(sendPacket, buffer, bufferSize);
    AppendString(sendPacket, word, wordSize);

    callFunction(connection, sendPacket);

    uint32_t value = 0;
    ExtractInt(connection->recvPacket, &value);

    free(sendPacket);
    return value;
}
int calculateWordFrequencyAsync(TCPConnection *connection, char *buffer, int bufferSize, char *word, int wordSize)
{
    Packet *sendPacket = (Packet *)malloc(sizeof(Packet));
    Clear(sendPacket);
    sendPacket->packetType = ASYNC;
    AppendString(sendPacket, "calculateWordFrequency", strlen("calculateWordFrequency"));
    AppendString(sendPacket, buffer, bufferSize);
    AppendString(sendPacket, word, wordSize);

    return callAsyncFunction(connection, sendPacket);
}
int calculateWordFrequencyRequest(TCPConnection *connection, int id)
{
    Packet *sendPacket = (Packet *)malloc(sizeof(Packet));
    Clear(sendPacket);
    sendPacket->packetType = REQUEST;
    AppendInt(sendPacket, id);
    callFunction(connection, sendPacket);

    uint32_t value = 0;
    ExtractInt(connection->recvPacket, &value);

    free(sendPacket);
    return value;
}

float* rotateArray(TCPConnection *connection, float *arr, int size, int rotations, char *direction, int directionSize)
{
    Packet *sendPacket = (Packet *)malloc(sizeof(Packet));
    Clear(sendPacket);
    sendPacket->packetType = SYNC;
    AppendString(sendPacket, "rotateArray", strlen("rotateArray"));
    AppendFloatArray(sendPacket, arr, size);
    AppendInt(sendPacket, rotations);
    AppendString(sendPacket, direction, directionSize);

    callFunction(connection, sendPacket);

    float *data = NULL;
    uint32_t dataSize;
    ExtractFloatArray(connection->recvPacket, &data, &dataSize);

    free(sendPacket);
    return data;
}
int rotateArrayAsync(TCPConnection *connection, float *arr, int size, int rotations, char *direction, int directionSize)
{
    Packet *sendPacket = (Packet *)malloc(sizeof(Packet));
    Clear(sendPacket);
    sendPacket->packetType = ASYNC;
    AppendString(sendPacket, "rotateArray", strlen("rotateArray"));
    AppendFloatArray(sendPacket, arr, size);
    AppendInt(sendPacket, rotations);
    AppendString(sendPacket, direction, directionSize);

    return callAsyncFunction(connection, sendPacket);
}
float* rotateArrayRequest(TCPConnection *connection, int id)
{
    Packet *sendPacket = (Packet *)malloc(sizeof(Packet));
    Clear(sendPacket);
    sendPacket->packetType = REQUEST;
    AppendInt(sendPacket, id);
    callFunction(connection, sendPacket);

    float *data = NULL;
    uint32_t dataSize;
    ExtractFloatArray(connection->recvPacket, &data, &dataSize);

    free(sendPacket);
    return data;
}
