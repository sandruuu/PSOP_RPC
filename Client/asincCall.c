#include "client_stub.h"
#include "asincCall.h"


pthread_t thread[THREAD_SIZE];
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;   
pthread_cond_t signalQueueState = PTHREAD_COND_INITIALIZER; 

pendingQueue queStruct;
queueCond queCond;


bool delete = false;
pthread_barrier_t cleanup_barrier;
pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
int thread_count_reached = 0;

bool continue_ = true;
int stop = 0;
pthread_cond_t cond_stop = PTHREAD_COND_INITIALIZER;


void create_thread(void* connection)
{
    initPendingQueue();

    // pthread_attr_t threads_attr;
    // pthread_attr_init(&threads_attr);
    // pthread_attr_setdetachstate(&threads_attr,PTHREAD_CREATE_DETACHED);

 
    for ( int i = 0;i < THREAD_SIZE; i++)
    {
        int s = pthread_create(&thread[i],NULL,HandleQueuedPacket,connection);
        if(s != 0)
        {
            perror("pthread_create\n");
            exit(EXIT_FAILURE);
        }
        
    }
    // pthread_attr_destroy(&threads_attr);
}

void waitThreads()
{
    int i = 0;
    for ( i = 0; i < THREAD_SIZE; i++) {
        pthread_join(thread[i], NULL);
    }
}

void enqueue(sendAsync recvAsync)
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

        queStruct.queue[position]->state = recvAsync.state;
        Packet* pck = malloc(sizeof(Packet));
        if(pck != NULL)
        {
            *pck = recvAsync.packet;
            queStruct.queue[position]->packet = pck;
        }else {
            perror("[enqueue(sendAsync recvAsync)] -> malloc -");
        }
        queStruct.queue[position]->idPacket = recvAsync.idPacket;

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

void *clearQueueOfProcessedPackets(void *data)
{
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    pthread_sigmask(SIG_BLOCK, &set, NULL);

    pthread_mutex_lock(&mtx);
    queCond = WAITING_PACKETS;
    pthread_cond_broadcast(&signalQueueState);
    pthread_mutex_unlock(&mtx);

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
                thread_count_reached++;
                pthread_cond_signal(&cond_stop);
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
                copy = queStruct.queue[i];
                
                if(pthread_mutex_trylock(&copy->mtxPck) == 0)
                {

                    if(copy->state == DELETED)
                    {
                        printf("Cererea cu id-ul %u a fost stearsa\n",copy->idPacket);
                        queStruct.queue[i] = NULL;
                        pthread_mutex_unlock(&copy->mtxPck);
                        
                        int j = 0;

                        for(j = i; j < queStruct.size-1;j++)
                        {
                            queStruct.queue[j] = queStruct.queue[j+1];
                        }
                            queStruct.queue[j] = NULL;

                        queStruct.size--;
                        pthread_mutex_destroy(&copy->mtxPck);

                        free(copy->packet);
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

        if(continue_ == false)
        {
            stop++;
            pthread_cond_signal(&cond_stop);
            pthread_mutex_unlock(&count_mutex);
            pthread_barrier_wait(&cleanup_barrier);
            break;
        }

        pthread_mutex_unlock(&count_mutex);
        pthread_barrier_wait(&cleanup_barrier);
    }
   
    return NULL;
}


void initialize_barrier() {

    if (pthread_barrier_init(&cleanup_barrier, NULL, THREAD_SIZE+DELETE_THREAD_SIZE) != 0) {
        perror("pthread_barrier_init");
        exit(EXIT_FAILURE);
    }
}


void *HandleQueuedPacket(void *recvconnection)
{
    TCPConnection* connection = (TCPConnection*)recvconnection;
    sendAsync* recvAsync = NULL;

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    pthread_sigmask(SIG_BLOCK, &set, NULL);
   
    while(1)
    {
        
        pthread_mutex_lock(&count_mutex);

        if(delete ==  true)
        {   
            thread_count_reached++;
            pthread_mutex_unlock(&count_mutex);
            pthread_barrier_wait(&cleanup_barrier);

            pthread_mutex_lock(&count_mutex);

            if(continue_ == false)
            {
                stop++;
                pthread_cond_signal(&cond_stop);
                pthread_mutex_unlock(&count_mutex);
                return NULL;
            }else 
            {
                pthread_mutex_unlock(&count_mutex);
            }

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
            queueElement* pck_W = dequeue();
            pthread_mutex_unlock(&mtx);

            if(pck_W != NULL)
            {
                pthread_mutex_lock(&pck_W->mtxPck);
                printf("Cererea cu id-ul %u a fost trimisa catre procesare\n",pck_W->idPacket);
                Packet* sendPck = malloc(sizeof(Packet));
                if(sendPck != NULL)
                {
                    Clear(sendPck);
                    sendPck->packetType = REQUEST;
                    AppendInt(sendPck,pck_W->idPacket);
                    
                    recvAsync = request_result(connection,sendPck);
                    
                    if(recvAsync->idPacket == pck_W->idPacket)
                    {
                        if(recvAsync->state == READY)
                    {
                        //trebuie sa adauga logica de returnare a rezultatului
                        pck_W->state = DELETED;
                        printf("Cererea cu id-ul %u a fost procesata\n",pck_W->idPacket);
                        printResultFromAsyncFunction(*pck_W->packet,recvAsync->packet);
                        
                    }else  
                        if(recvAsync->state == WAITING){
                        printf("Cererea cu id-ul %u este inca in waiting\n",pck_W->idPacket);
                        pck_W->state = WAITING;
            
                        }else 
                            if(recvAsync->state == DELETED){
                                printf("Cererea cu id-ul %u a fost stearsa. Se va cere reprocesarea\n",pck_W->idPacket);
                                pck_W->packet->packetType = SENDASYNC;
                                Clear(sendPck);
                                *sendPck = *pck_W->packet;
                                pck_W->state = DELETED;
                            }
                    }else 
                        {
                            printf("Id-ul packhetului primit %u nu corespunde cu id-ul %u pachetului cerut de client\n",
                            recvAsync->idPacket,pck_W->idPacket);
                        }
                }
                
                //printf("Deblochez mutexul pck_W->mtxPck din threadul- %ld\n\n",pthread_self());
                pthread_mutex_unlock(&pck_W->mtxPck);

                if(recvAsync != NULL && recvAsync->state == DELETED)
                    callAsyncFunction(connection,sendPck);
                else 
                    free(sendPck);
                recvAsync = NULL;
            }

        }
        
   }
    
}

void printResultFromAsyncFunction(Packet request, Packet response)
{

    char* functionName = (char*) malloc(sizeof(char)*FUNCTION_NAME_LEN);
    if(functionName == NULL)
    {
        perror("malloc -> printResultFromAsyncFunction: ");
        exit(EXIT_FAILURE);
    }
    memset(functionName,0,FUNCTION_NAME_LEN);
    ExtractString(&request,&functionName);

    if(strcmp(functionName,"add") == 0)
    {
        uint32_t val1 = 0;
        uint32_t val2 = 0;
        uint32_t result = 0;

        ExtractInt(&request,&val1);
        ExtractInt(&request,&val2);
        ExtractInt(&response,&result);

        char resultBuffer[RESULT_BUFFER_SIZE];
        memset(resultBuffer,0,RESULT_BUFFER_SIZE);
        int len = snprintf(resultBuffer,RESULT_BUFFER_SIZE,"%d + %d = %d\n",val1,val2,result);
        write(STDOUT_FILENO,resultBuffer,len);

    }else 
        if(strcmp(functionName,"multiply") == 0)
        {
            uint32_t val1 = 0;
            uint32_t val2 = 0;
            uint32_t result = 0;

            ExtractInt(&request,&val1);
            ExtractInt(&request,&val2);
            ExtractInt(&response,&result);

            char resultBuffer[RESULT_BUFFER_SIZE];
            memset(resultBuffer,0,RESULT_BUFFER_SIZE);
            int len = snprintf(resultBuffer,RESULT_BUFFER_SIZE,"%d * %d = %d\n",val1,val2,result);
            write(STDOUT_FILENO,resultBuffer,len);
        }else
            if(strcmp(functionName,"max_array") == 0)
            {
               float* array;
               uint32_t size;
               float result;

                ExtractArray(&request,(void*)(&array),&size,TYPE_FLOAT);
                ExtractFloat(&response,&result);

                char resultBuffer[RESULT_BUFFER_SIZE];
                memset(resultBuffer,0,RESULT_BUFFER_SIZE);
                int len = 0;
                int i = 0;
                while(i < size)
                   { 
                        len += snprintf(resultBuffer +len,RESULT_BUFFER_SIZE," %.2f ",array[i]);
                        i++;
                   }
                resultBuffer[len] = '-';
                len++;
                len += snprintf(resultBuffer +len,RESULT_BUFFER_SIZE," %.2f ",result);

                write(STDOUT_FILENO,resultBuffer,len);
            }
    free(functionName);

}

void initPendingQueue()
{
    queStruct.size = 0;
    //queStruct.front = 0;
    for( int i=0;i< MAX_QUEUE_SIZE;i++)
        queStruct.queue[i] = NULL;

    queCond = NO_WAITING_PACKETS;
    
}
queueElement* dequeue() {
    
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
