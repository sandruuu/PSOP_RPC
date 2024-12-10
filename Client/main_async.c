#include <stdio.h>
#include <stdlib.h>
#include "client_stub.h"

#define PORT 8080

int main()
{
    TCPConnection *connection = (TCPConnection *)malloc(sizeof(TCPConnection));
    connection->endpoint = (IPEndpoint *)malloc(sizeof(IPEndpoint));
    connection->endpoint->IPAddress = strdup("127.0.0.1");
    connection->endpoint->Port = PORT;
    connection->recvPacket = (Packet *)malloc(sizeof(Packet));

    // int id = addAsync(connection, 10, 29);
    // printf("Id-ul functiei este: %d\n", id);

    // int sum = addREQUEST(connection, id);
    // printf("Suma numerelor este: %d\n", sum);
    char buffer[] = "ana ana are mere  mere si pere.";
    int id = removeDuplicatesAsync(connection, buffer, strlen(buffer));
    printf("Id-ul functiei este: %d\n", id);
    

    char *data = removeDuplicatesREQUEST(connection, id);
    printf("Duplicate eliminate: %s\n", data);

    free(connection->endpoint->IPAddress);
    free(connection->endpoint);
    free(connection->recvPacket);
    free(connection);
    return 0;
}