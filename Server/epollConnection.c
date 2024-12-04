#define MAX_EVENTS 10
#include "epollConnection.h"

struct epoll_event ev, events[MAX_EVENTS];
int  nfds, epollfd;

void do_use_fd(int connfd)
{

    Packet *packet = (Packet*)malloc(sizeof(Packet));
    if(packet == NULL)
    {
        perror("malloc:\n");
        exit(EXIT_FAILURE);
    }
    Clear(packet);

    //se citesc datele transmise de client prin socket 

    //pthread_mutex_lock(&recv_mutex);
    int readBytes = read(connfd, packet, sizeof(*packet));
    //pthread_mutex_unlock(&recv_mutex);

    packet->extractionOffset = ntohl(packet->extractionOffset);
    packet->currentSize = ntohl(packet->currentSize);

    if (readBytes < 0)
    {
        perror("[threadRecv(void *argv)] - ");
        exit(EXIT_FAILURE);
    }

    processPacket(packet, connfd);
       
}
static int setnonblocking(int sockfd)
{
	if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) | O_NONBLOCK) ==-1) {
		return -1;
	}
	return 0;
}

void epollComunication(TCP_serverSocket *server){
     
    createSocket(server);
    initSocket(server);
    Bind(server);
    Listen(server);

    epollfd = epoll_create1(0);
    if (epollfd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    ev.events = EPOLLIN;
    ev.data.fd = server->sockfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, server->sockfd, &ev) == -1) {
        perror("epoll_ctl: server->sockfd");
        exit(EXIT_FAILURE);
    }

    for (;;) {

        nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for (int n = 0; n < nfds; ++n) {
            if (events[n].data.fd == server->sockfd) {

                TCP_clientSocket *client = (TCP_clientSocket *)malloc(sizeof(TCP_clientSocket));
                Accept(server,client);
                
                setnonblocking(client->connfd);
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = client->connfd;

                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, client->connfd,
                            &ev) == -1) {
                    perror("epoll_ctl: client->connfd");
                    exit(EXIT_FAILURE);
                }
            } else {
                do_use_fd(events[n].data.fd);
            }
        }
    }

}

          