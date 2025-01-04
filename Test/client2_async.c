#include <stdio.h>
#include <stdlib.h>
#include "../Client/client_stub.h"

#define PORT 8080

int main()
{
    TCPConnection *connection = (TCPConnection *)malloc(sizeof(TCPConnection));
    connection->endpoint = (IPEndpoint *)malloc(sizeof(IPEndpoint));
    connection->endpoint->IPAddress = strdup("127.0.0.1");
    connection->endpoint->Port = PORT;
    connection->recvPacket = (Packet *)malloc(sizeof(Packet));

    char buffer[] = "ana ana are mere  mere si pere ana.";
    int id = calculateWordFrequencyAsync(connection, buffer, strlen(buffer), "ana", strlen("ana"));
    printf("Id-ul functiei este: %d\n", id);
    int value = calculateWordFrequencyRequest(connection, id);
    printf("calculateWordFrequency: %d\n", value);


    free(connection->endpoint->IPAddress);
    free(connection->endpoint);
    free(connection->recvPacket);
    free(connection);
    return 0;
}