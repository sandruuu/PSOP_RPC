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

    float arr[] = {3.5, 4.4, 6.0, 7.9, 8.61, 5.22};
    int id = rotateArrayAsync(connection, arr, 6, 2, "left", strlen("left"));
    float *result = rotateArrayRequest(connection, id);
    printf("rotateArray: ");
    for (int i = 0; i < 6; i++)
    {
        printf("%.2f ", result[i]);
    }
    printf("\n");

    free(connection->endpoint->IPAddress);
    free(connection->endpoint);
    free(connection->recvPacket);
    free(connection);
    return 0;
}