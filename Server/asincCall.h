#ifndef ASINC_CALL_H
#define ASINC_CALL_H

#include "../Utils/packet.h"
#include <pthread.h>

#define THREAD_SIZE 4 
#define MAX_QUEUE_SIZE 30
#define DELETE_THREAD_SIZE 1
#define EXPIRATION_TIME 1


typedef enum queuedElementState{
    WAITING,
    READY,
    DELETED,
    NO_STATE,
    PROCESSING
}queuedElementState;

typedef enum queueCond{
    NO_WAITING_PACKETS,
    WAITING_PACKETS,
    EXIT

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
    time_t timestamp;

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
extern pendingQueue queStruct;

extern pthread_barrier_t cleanup_barrier;
extern pthread_mutex_t count_mutex;
extern int thread_count_reached;
extern bool delete;


void * HandleQueuedPacket(void* value );
void initPendingQueue();
void create_thread();
void enqueue(Packet* packet, uint32_t idPacket);
queueElement* dequeue();
void* clearQueueOfOutdatedPackets(void* data);
void create_cleanup();
queueElement* getRequestedPacket(uint32_t idPacket);
void processPacket(Packet *packet, int connfd);
void initialize_barrier();
#endif
