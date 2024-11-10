#ifndef _GNU_BIB_LIBRARIES
#define _GNU_BIB_LIBRARIES 
#include "libheaders.h"
#endif
#include "connectToClient.h"

int main(int argc, char* argv){
    struct TCPServerConn server;
    chatWithClients(&server);
    closeServerConnection(server.sockFD);
    return 0;

}