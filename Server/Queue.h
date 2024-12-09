#include "../Utils/packet.h"

typedef struct Node {
    Packet* packet;
    int id;
    struct Node* next;
} Node;

typedef struct Queue{
    struct Node* firstNode;
} Queue;

void pushQueue(Queue* queue, Packet* packet, int id, int* size);
Node* popQueue(Queue* queue, int* size);
Packet* extractPacketById(Queue* queue, int id, int* size);
