#include "queue.h"

void pushQueue(Queue* queue, Packet* packet, int id, int* size){
    
    Node* newNode = (Node*)malloc(sizeof(Node));
    
    newNode->packet = packet;  
    newNode->id = id;        
    newNode->next = NULL;     

    if (queue->firstNode == NULL) {
        queue->firstNode = newNode;
    } else {
        newNode->next = queue->firstNode;
        queue->firstNode = newNode;  
    }
    (*size)++;
}


Node* popQueue(Queue* queue, int* size) {
    if (queue->firstNode == NULL) {
        return NULL; 
    }
    Node* current = queue->firstNode;
    queue->firstNode = current->next;
    (*size)--;
    return current;
}

Packet* extractPacketById(Queue* queue, int id, int* size){
    if (queue->firstNode == NULL) {
        return NULL; 
    }
    Node* current = queue->firstNode;
    Node* previous = NULL;
    while (current != NULL && current->id != id) {
        previous = current;
        current = current->next;
    }
    if (current == NULL) {
        return NULL; 
    }
    if (previous == NULL) {
        queue->firstNode = current->next;  
    } else {
        previous->next = current->next; 
    }
    Packet* packet = current->packet;
    free(current);
    (*size)--;
    return packet;
}