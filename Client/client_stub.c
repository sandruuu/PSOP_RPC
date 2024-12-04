#include "client_stub.h"

#define PORT 8080
#define IPADDRESS "25.19.213.168"


pthread_t threads[MAXTHREADS];
int threadCount = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t recv_mutex = PTHREAD_MUTEX_INITIALIZER;
uint32_t ClientId;


void SumCallback(Packet *recvPacket)
{
    uint32_t sum = 0;
    ExtractInt(recvPacket, &sum);
    printf("Suma numerelor este: %d\n", sum);
}

int Connect(IPEndpoint *endpoint)
{
    int sockfd;
    struct sockaddr_in serv_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("[Connect(IPEndpoint *endpoint)] - ");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_port = htons(endpoint->Port);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(endpoint->IPAddress);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("[Connect(IPEndpoint *endpoint)] - ");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

void CloseConnection(TCPConnection *connection)
{
    pthread_mutex_lock(&recv_mutex);
    Packet sendPacket;
    Clear(&sendPacket);
    sendPacket.packetType = DISCONNECT;
    int sendBytes = send(connection->sockfd, &sendPacket,sizeof(sendPacket),0);
        
    if(sendBytes < 0)
    {
        perror("void HandleSignal(int sig, siginfo_t *info, void *ucontext) - send -");
        exit(EXIT_FAILURE);
    }
    
    pthread_mutex_unlock(&recv_mutex);

    shutdown(connection->sockfd,SHUT_RDWR);
    close(connection->sockfd);

    free(connection->endpoint->IPAddress);
    free(connection->endpoint);
    free(connection->recvPacket);
    free(connection);

    pthread_barrier_destroy(&cleanup_barrier);
}

void callFunction(TCPConnection *connection, Packet *sendPacket)
{
    pthread_mutex_lock(&recv_mutex);

    int sendBytes = send(connection->sockfd, sendPacket, sizeof(*sendPacket), 0);
    if (sendBytes < 0)
    {
        perror("[callFunction(TCPConnection *connection, Packet *sendPacket)] - ");
        exit(EXIT_FAILURE);
    }

    Clear(connection->recvPacket);
    int recvBytes = recv(connection->sockfd, connection->recvPacket,
                         sizeof(*(connection->recvPacket)),0);
    if (recvBytes < 0)
    {
        perror("[callFunction(TCPConnection *connection, Packet *sendPacket)] - ");
        exit(EXIT_FAILURE);
    }
    pthread_mutex_unlock(&recv_mutex);

   
}

void callAsyncFunction(TCPConnection *connection, Packet *sendPacket)
{
    //connection->sockfd = Connect(connection->endpoint);
    pthread_mutex_lock(&recv_mutex);
    int sendBytes = send(connection->sockfd, sendPacket, sizeof(*(sendPacket)), 0);

    if (sendBytes < 0)
    {
        perror("1 [callAsyncFunction(TCPConnection *connection, Packet *sendPacket)] - ");
        exit(EXIT_FAILURE);
    }

    sendAsync sendAsync;


    int recvBytes = recv(connection->sockfd, &sendAsync,
                         sizeof(sendAsync),0);
    if (recvBytes < 0)
    {
        perror("2 [callAsyncFunction(TCPConnection *connection, Packet *sendPacket)] - ");
        exit(EXIT_FAILURE);
    }
    pthread_mutex_unlock(&recv_mutex);

    
    enqueue(sendAsync);
    
   
    free(sendPacket);
    
    
}
sendAsync* request_result(TCPConnection *connection,Packet* sendPck)
{
    //connection->sockfd = Connect(connection->endpoint);
    pthread_mutex_lock(&recv_mutex);

    int sendBytes = send(connection->sockfd, sendPck, sizeof(*(sendPck)), 0);
    
    if (sendBytes < 0)
    {
        perror("[sendAsync* request_result(TCPConnection *connection,Packet* sendPck)] - ");
        exit(EXIT_FAILURE);
    }

    sendAsync* recvStruct = (sendAsync*)malloc(sizeof(sendAsync));

    if(recvStruct != NULL)
    {

        int recvBytes = recv(connection->sockfd, recvStruct,
                            sizeof(*(recvStruct)),0);
        if (recvBytes < 0)
        {
            perror("[sendAsync* request_result(TCPConnection *connection,Packet* sendPck)] - ");
            exit(EXIT_FAILURE);
        }
    }
    pthread_mutex_unlock(&recv_mutex);

    return recvStruct;
    
}

void add_async(TCPConnection *connection, int a, int b)
{
    Packet* sendPacket = (Packet *)malloc(sizeof(Packet));
    Clear(sendPacket);
    AppendString(sendPacket, "add", strlen("add"));
    AppendInt(sendPacket, a);
    AppendInt(sendPacket, b);
    
    sendPacket->packetType = SENDASYNC;
    sendPacket->extractionOffset = htonl(sendPacket->extractionOffset);
    sendPacket->currentSize = htonl(sendPacket->currentSize);

    callAsyncFunction(connection, sendPacket);
}

void multiply_async(TCPConnection *connection, int a, int b)
{
    Packet* sendPacket = (Packet *)malloc(sizeof(Packet));
    Clear(sendPacket);
    AppendString(sendPacket, "multiply", strlen("multiply"));
    AppendInt(sendPacket, a);
    AppendInt(sendPacket, b);
    
    sendPacket->packetType = SENDASYNC;
    sendPacket->extractionOffset = htonl(sendPacket->extractionOffset);
    sendPacket->currentSize = htonl(sendPacket->currentSize);

    callAsyncFunction(connection, sendPacket);
}

void max_array_async(TCPConnection *connection, float *array, int size)
{
    Packet* sendPacket = (Packet *)malloc(sizeof(Packet));
    Clear(sendPacket);
    AppendString(sendPacket, "max_array", strlen("max_array"));
    AppendArray(sendPacket,array,size,TYPE_FLOAT);

    sendPacket->packetType = SENDASYNC;
    sendPacket->extractionOffset = htonl(sendPacket->extractionOffset);
    sendPacket->currentSize = htonl(sendPacket->currentSize);

    callAsyncFunction(connection, sendPacket);
}

void initConnection(TCPConnection **connection)
{
    *connection = (TCPConnection*)malloc(sizeof(TCPConnection));
    if(*connection == NULL)
    {
        perror("[void initConnection(TCPConnection **connection) - malloc -]");
        exit(EXIT_FAILURE);
    }

    (*connection)->endpoint = (IPEndpoint*)malloc(sizeof(IPEndpoint));
    (*connection)->endpoint->IPAddress = strdup("127.0.0.1");
    (*connection)->endpoint->Port = PORT;
    (*connection)->recvPacket =  (Packet*)malloc(sizeof(Packet));

    (*connection)->sockfd = Connect((*connection)->endpoint);
}

int add(TCPConnection *connection, int a, int b)
{
    Packet *sendPacket = (Packet *)malloc(sizeof(Packet));
    Clear(sendPacket);
    sendPacket->packetType = SENDSYNC;
    AppendString(sendPacket, "add", strlen("add"));
    AppendInt(sendPacket, a);
    AppendInt(sendPacket, b);
    
    sendPacket->extractionOffset = htonl(sendPacket->extractionOffset);
    sendPacket->currentSize = htonl(sendPacket->currentSize);
    callFunction(connection, sendPacket);

    uint32_t sum = 0;
    ExtractInt(connection->recvPacket, &sum);

    free(sendPacket);
    return sum;
}

int multiply(TCPConnection *connection, int a, int b)
{
    Packet *sendPacket = (Packet *)malloc(sizeof(Packet));
    Clear(sendPacket);
    sendPacket->packetType = SENDSYNC;
    AppendString(sendPacket, "multiply", strlen("multiply"));
    AppendInt(sendPacket, a);
    AppendInt(sendPacket, b);
    
    sendPacket->extractionOffset = htonl(sendPacket->extractionOffset);
    sendPacket->currentSize = htonl(sendPacket->currentSize);
    callFunction(connection, sendPacket);

    uint32_t multiply = 0;
    ExtractInt(connection->recvPacket, &multiply);

    free(sendPacket);
    return multiply;
}

float max_array(TCPConnection *connection, float *array, int size)
{
    Packet *sendPacket = (Packet *)malloc(sizeof(Packet));
    Clear(sendPacket);
    sendPacket->packetType = SENDSYNC;
    AppendString(sendPacket, "max_array", strlen("max_array"));
    AppendArray(sendPacket,array,size,TYPE_FLOAT);
    
    sendPacket->extractionOffset = htonl(sendPacket->extractionOffset);
    sendPacket->currentSize = htonl(sendPacket->currentSize);
    callFunction(connection, sendPacket);

    float max = 0;
    ExtractFloat(connection->recvPacket, &max);

    free(sendPacket);
    return max;
}
