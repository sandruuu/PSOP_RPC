#include "connectToClient.h"

#ifndef GLOBAL_VARIABLES
#define GLOBAL_VARIABLES

pthread_t thread[THREAD_SIZE];
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t signalQueueState = PTHREAD_COND_INITIALIZER;         //daca exista elemente in waiting sau nu 
queueCond queCond;
pendingQueue queStruct;

pthread_barrier_t cleanup_barrier;
pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
int thread_count_reached = 0;
bool delete = false;

#endif



void processPacket(Packet *packet, int connfd)
{
    sendAsync sendStruct;
    switch (packet->packetType)
    {
    case SENDSYNC:

        callFunction(packet);
        packet->packetType = SENDSYNC;
        int sendBytes = write(connfd, packet, sizeof(*packet));
        if (sendBytes < 0)
        {
            perror("[processPacket(Packet *packet)] - ");
            exit(EXIT_FAILURE);
        }
        
        break;

    case SENDASYNC:

            generateAsyncPacket(&sendStruct,packet);

            //ACK prin care clientul este deblocat si nu mai asteapta raspuns de la server 
           
            sendBytes = write(connfd, &sendStruct, sizeof(sendStruct));
           

            if (sendBytes < 0)
            {
                perror("[processPacket(Packet *packet)] - ");
                exit(EXIT_FAILURE);
            }

           
            enqueue(packet,sendStruct.idPacket);
       
        break;
    case REQUEST:

    //clientul care a apelat asincron o functia, transmite o cerere prin
    // care vrea sa obtine valoarea de return a acesteia 

        uint32_t extractedID;
        ExtractInt(packet, &extractedID);

        pthread_mutex_lock(&mtx);
        queueElement* requestedPacket = getRequestedPacket(extractedID);
        pthread_mutex_unlock(&mtx);

        if(requestedPacket != NULL)
        {
            pthread_mutex_lock(&requestedPacket->mtxPck);

            sendStruct.idPacket = requestedPacket->idPacket;
            sendStruct.packet = *requestedPacket->packet;
            sendStruct.state = requestedPacket->state;

           
            sendBytes = write(connfd, &sendStruct, sizeof(sendStruct));
          

            if (sendBytes < 0)
            {
                perror("[processPacket(Packet *packet)] - ");
                exit(EXIT_FAILURE);
            }
            pthread_mutex_unlock(&requestedPacket->mtxPck);
                

        }else {

            sendStruct.state = DELETED;
            sendStruct.idPacket = extractedID;
            
            
            sendBytes = write(connfd, &sendStruct, sizeof(sendStruct));
            

            if (sendBytes < 0)
            {
                perror("[processPacket(Packet *packet)] - ");
                exit(EXIT_FAILURE);
            }
        }
        break;
    case DISCONNECT:
        closeClientConnection(connfd);
        break;
    default:
        break;
    }
}

void initialize_barrier() {

    if (pthread_barrier_init(&cleanup_barrier, NULL, THREAD_SIZE+DELETE_THREAD_SIZE) != 0) {
        perror("pthread_barrier_init");
        exit(EXIT_FAILURE);
    }
}

void* HandleQueuedPacket(void* value) {
    while (1) {

        pthread_mutex_lock(&count_mutex);
        
        if(delete ==  true)
        {   
            thread_count_reached++;
            pthread_mutex_unlock(&count_mutex);
            pthread_barrier_wait(&cleanup_barrier);

        }else {
            pthread_mutex_unlock(&count_mutex);
        }

        while(queCond == NO_WAITING_PACKETS)
        { 
            pthread_mutex_lock(&mtx);
            pthread_cond_wait(&signalQueueState,&mtx);
            pthread_mutex_unlock(&mtx);
        }

        if(queCond == WAITING_PACKETS)
        {
            pthread_mutex_lock(&mtx);
            queueElement* queuedPck_W = dequeue();
            pthread_mutex_unlock(&mtx);

            if(queuedPck_W != NULL)
            {   
                pthread_mutex_lock(&queuedPck_W->mtxPck);
                printf("Cererea cu id-ul %u a fost trimisa catre procesare\n",queuedPck_W->idPacket);
               
                callFunction(queuedPck_W->packet);

                queuedPck_W->state = READY;
                queuedPck_W->packet->packetType = RESPONSE;
                pthread_mutex_unlock(&queuedPck_W->mtxPck);
           }
             
        }
    }
    return NULL;
}

void create_thread(){
    
    initPendingQueue();
    initialize_barrier();
    
    for ( int i = 0;i < THREAD_SIZE; i++)
    {
        int s = pthread_create(&thread[i],NULL,HandleQueuedPacket,NULL);
        if(s != 0)
        {
            perror("pthread_create\n");
            exit(EXIT_FAILURE);
        }
    }

}
//se inreagistreaza pachetul transmis pentru procesare asincrona 

void enqueue(Packet* packet, uint32_t idPacket)
{
   pthread_mutex_lock(&mtx);

    if(queStruct.size >= MAX_QUEUE_SIZE)
    {
        perror("Too many elements in queue \n");
        exit(EXIT_FAILURE);
    }
    int position = queStruct.size;

    queStruct.queue[position] = (queueElement*)malloc(sizeof(queueElement));

    if(queStruct.queue[position] != NULL)
    {
        pthread_mutex_init(&queStruct.queue[position]->mtxPck,NULL);
        pthread_mutex_lock(&queStruct.queue[position]->mtxPck);

        queStruct.queue[position]->state = WAITING;
        packet->packetType = RESPONSE;
        queStruct.queue[position]->packet = packet;
        queStruct.queue[position]->idPacket = idPacket;
        queStruct.queue[position]->timestamp = time(NULL);

        printf("Cererea cu id-ul %u a fost adaugata in coada\n",queStruct.queue[position]->idPacket);
        pthread_mutex_unlock(&queStruct.queue[position]->mtxPck);

        queStruct.size++;
        queCond = WAITING_PACKETS;
        pthread_cond_signal(&signalQueueState);
    }else {
        perror("[void enqueue(Packet pck)] malloc - ");

    }
    
    pthread_mutex_unlock(&mtx);

}

//primul pachet aflat in waiting se trasmite la procesare 
queueElement* dequeue()
{
   
    int i = 0;
    queueElement* element = NULL;

    for(i = 0;i < queStruct.size;i++)
    {
        if(queStruct.queue[i] != NULL)
        {
            if(pthread_mutex_trylock(&queStruct.queue[i]->mtxPck) == 0)
            {
                if(queStruct.queue[i]->state == WAITING)
                {
                    queStruct.queue[i]->state = PROCESSING;
                    element = queStruct.queue[i];
                    pthread_mutex_unlock(&queStruct.queue[i]->mtxPck);
                    break;
                }
                pthread_mutex_unlock(&queStruct.queue[i]->mtxPck);
            }
        }

    }
    return element;
}

void initPendingQueue(){

    queStruct.size = 0;
    queStruct.front = 0;

    for(int i = 0;i<MAX_QUEUE_SIZE;i++)
        queStruct.queue[i]= NULL;

    queCond = NO_WAITING_PACKETS;
       
}

void create_cleanup()
{
    pthread_t cleanup_thread;
    pthread_create(&cleanup_thread, NULL, clearQueueOfOutdatedPackets, NULL);
}

void *clearQueueOfOutdatedPackets(void *data)
{
    while(1)
    {
        pthread_mutex_lock(&count_mutex);
        delete  = true;
        pthread_mutex_unlock(&count_mutex);

        while(1)
        {
            pthread_mutex_lock(&count_mutex);
            if(thread_count_reached == THREAD_SIZE)
            {
                pthread_mutex_unlock(&count_mutex);
                break;
            }
            pthread_mutex_unlock(&count_mutex);
        }
    

        pthread_mutex_lock(&mtx);
        queueElement* copy;

      
        for( int i = 0;i < queStruct.size;i++)
        {
            copy = queStruct.queue[i];

            if(copy != NULL)
            { 
                if(pthread_mutex_lock(&copy->mtxPck) == 0)

                {
                    time_t current_time = time(NULL);

                    if(difftime(current_time,copy->timestamp) > EXPIRATION_TIME)
                    {
                        printf("Cererea cu id-ul %u a fost stearsa\n",copy->idPacket);
                        queStruct.queue[i] = NULL;
                        pthread_mutex_unlock(&copy->mtxPck);
                        
                        int j=0;
                        for(j = i;j<queStruct.size-1;j++)
                        {
                            queStruct.queue[j] = queStruct.queue[j+1];
                        }
                        
                        queStruct.queue[j] = NULL;

                        queStruct.size--;
                        pthread_mutex_destroy(&copy->mtxPck);
                        free(copy);

                    }else 
                        pthread_mutex_unlock(&copy->mtxPck);
                }
            }
        }


        pthread_mutex_unlock(&mtx);

        pthread_mutex_lock(&count_mutex);
        delete  = false;
        thread_count_reached = 0;
        pthread_mutex_unlock(&count_mutex);

        pthread_barrier_wait(&cleanup_barrier);
        
    }

}

queueElement *getRequestedPacket(uint32_t idPacket)
{

    for( int i= 0;i < queStruct.size;i++)
    {
        pthread_mutex_lock(&queStruct.queue[i]->mtxPck);
        if (queStruct.queue[i]->idPacket == idPacket)
            {
                pthread_mutex_unlock(&queStruct.queue[i]->mtxPck);
                return queStruct.queue[i];
            }
        pthread_mutex_unlock(&queStruct.queue[i]->mtxPck);
    }
      
    return NULL;
}

