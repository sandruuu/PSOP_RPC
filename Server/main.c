#ifndef _GNU_BIB_LIBRARIES
#define _GNU_BIB_LIBRARIES 
#include "epollConnection.h"
#endif

int main(int argc, char* argv){

    struct TCP_serverSocket server;
    create_thread();
    create_cleanup();

    struct sigaction sa;
    sa.sa_handler = SIG_IGN; 
    sigemptyset(&sa.sa_mask);
    memset(&sa, 0, sizeof(sa));
    sigaction(SIGWINCH, &sa, NULL);

    
    epollComunication(&server);
    pthread_barrier_destroy(&cleanup_barrier);
    closeServerConnection(server.sockfd);
    return 0;
}