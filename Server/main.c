#include "connectToClient.h"

int main(int argc, char** argv){
    struct TCPServerConn server;
    pthread_t id;
    pthread_create(&id, NULL, manageRequestsQueue, NULL);
    chatWithClients(&server);
    closeServerConnection(server.sockFD);
    pthread_join(id,NULL);
    return 0;
}