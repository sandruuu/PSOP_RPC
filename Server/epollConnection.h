#define MAX_EVENTS 10

// #include <sys/epoll.h>
// #include <fcntl.h>
#include "connectToClient.h"

extern struct epoll_event ev, events[MAX_EVENTS];
extern int nfds, epollfd;

void epollComunication(TCP_serverSocket *server);