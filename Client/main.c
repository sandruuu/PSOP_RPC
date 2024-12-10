#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client_stub.h"

#define PORT 8080

int main()
{
    TCPConnection *connection = (TCPConnection *)malloc(sizeof(TCPConnection));
    connection->endpoint = (IPEndpoint *)malloc(sizeof(IPEndpoint));
    connection->endpoint->IPAddress = strdup("127.0.0.1");
    connection->endpoint->Port = PORT;
    connection->recvPacket = (Packet *)malloc(sizeof(Packet));

    // int sum = add(connection, 10, 20);
    // printf("add: %d\n", sum);

    // char buffer[] = "ana ana are mere  mere si pere.";
    // char* data = removeDuplicates(connection, buffer, strlen(buffer));
    // printf("removeDuplicates: %s\n", data);

    // int arr[] = {12, 899, 123, 5678, 5467, 56789};
    // int value = longestAscendingDigitNumber(connection, arr, 6);
    // printf("longestAscendingDigitNumber: %d\n", value);

    // char buffer[] = "ana ana are mere  mere si pere ana.";
    // int value = calculateWordFrequency(connection, buffer, strlen(buffer), "ana", strlen("ana"));
    // printf("calculateWordFrequency: %d\n", value);

    float arr[] = {3.5, 4.4, 6.0, 7.9, 8.61, 5.22};
    float *result = rotateArray(connection, arr, 6, 2, "left", strlen("left"));
    printf("rotateArray: ");
    for (int i = 0; i < 6; i++)
    {
        printf("%.2f ", result[i]);
    }
    printf("\n");

    free(result);
    // free(data);
    free(connection->endpoint->IPAddress);
    free(connection->endpoint);
    free(connection->recvPacket);
    free(connection);
    return 0;
}