#include <pthread.h>
#include "../Utils/packet.h"
#include <stdbool.h>
#include <sys/eventfd.h>
#include "signal.h"

#ifndef GLOBA_VARIABLES_ONCE
#define GLOBA_VARIABLES_ONCE

#define DELETE_THREAD_SIZE 1
#define FUNCTION_NAME_LEN 20
#define RESULT_BUFFER_SIZE 100
#define THREAD_SIZE 3 
#define MAX_QUEUE_SIZE 15


typedef enum queuedElementState{
    WAITING,
    READY,
    DELETED,
    NO_STATE,
    PROCESSING
}queuedElementState;

typedef enum queueCond{
    NO_WAITING_PACKETS,
    WAITING_PACKETS
}queueCond;

typedef struct sendAsync
{
    Packet packet;
    queuedElementState state;
    uint32_t idPacket;

}sendAsync;

typedef struct queueElement{
    Packet* packet;
    queuedElementState state;
    uint32_t idPacket;
    pthread_mutex_t mtxPck;

}queueElement;

typedef struct pendingQueue{

    queueElement *queue[MAX_QUEUE_SIZE];
    int size;
    int front;
 
}pendingQueue;

extern pthread_t thread[THREAD_SIZE];
extern pthread_mutex_t mtx;
extern pthread_cond_t signalQueueState;         //daca exista elemente in waiting sau nu 

extern queueCond queCond;
extern bool continue_;
extern pendingQueue queStruct;

extern pthread_barrier_t cleanup_barrier;
extern pthread_mutex_t count_mutex;
extern int thread_count_reached;
extern bool delete;
extern int stop;
extern pthread_cond_t cond_stop;


void initPendingQueue();
queueElement* dequeue();
void create_thread(void* connection);
void enqueue(sendAsync recvAsync);
void* clearQueueOfProcessedPackets(void* data);
void waitThreads();
void initialize_barrier();
void* HandleQueuedPacket(void *recvconnection);
void printResultFromAsyncFunction(Packet request, Packet response);
#endif

