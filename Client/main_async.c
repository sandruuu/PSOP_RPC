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
    // int sum = addRequest(connection, id);
    // printf("Suma numerelor este: %d\n", sum);

    // char buffer[] = "ana ana are mere  mere si pere.";
    // int id = removeDuplicatesAsync(connection, buffer, strlen(buffer));
    // printf("Id-ul functiei este: %d\n", id);
    // char *data = removeDuplicatesRequest(connection, id);
    // printf("Duplicate eliminate: %s\n", data);

    // int arr[] = {12, 899, 123, 5678, 5467, 56789};
    // int id = longestAscendingDigitNumberAsync(connection, arr, 6);
    // printf("Id-ul functiei este: %d\n", id);
    // int value = longestAscendingDigitNumberRequest(connection, id);
    // printf("longestAscendingDigitNumber: %d\n", value);

    // char buffer[] = "ana ana are mere  mere si pere ana.";
    // int id = calculateWordFrequencyAsync(connection, buffer, strlen(buffer), "ana", strlen("ana"));
    // printf("Id-ul functiei este: %d\n", id);
    // int value = calculateWordFrequencyRequest(connection, id);
    // printf("calculateWordFrequency: %d\n", value);

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