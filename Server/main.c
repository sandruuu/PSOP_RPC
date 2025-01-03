#include "connectToClient.h"

int main(int argc, char **argv)
{
    set_log_file("server_log.txt", LOG_TRACE);

    struct TCPConnection server;
    pthread_t id;
    pthread_create(&id, NULL, manageRequestsQueue, NULL);
    chatWithClients(&server);
    closeServerConnection(server.sockFD);
    pthread_join(id, NULL);
    return 0;
}