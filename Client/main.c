#include <stdio.h>
#include <stdlib.h>
#include "client_stub.h"

#define PORT 8080
#define IPADDRESS "25.19.213.168"

int main(){
    TCPConnection* connection = (TCPConnection*)malloc(sizeof(TCPConnection));
    connection->endpoint = (IPEndpoint*)malloc(sizeof(IPEndpoint));
    connection->endpoint->IPAddress = strdup("127.0.0.1");
    connection->endpoint->Port = PORT;
    connection->recvPacket = (Packet*)malloc(sizeof(Packet));
    
    int sum = add(connection, 10, 20);
    printf("Suma numerelor este: %d\n", sum);
    
    free(connection->endpoint->IPAddress);
    free(connection->endpoint);
    free(connection->recvPacket);
    free(connection);
    return 0;
}