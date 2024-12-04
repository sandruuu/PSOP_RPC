#include "server_stub.h"
#include <stdatomic.h>

pthread_mutex_t dlopen_mtx = PTHREAD_MUTEX_INITIALIZER;;

int generatePacketID() {
    static uint32_t id = 0;
    id++;
    return id;
}

void callSumFunction(Packet *packet) {
    pthread_mutex_lock(&dlopen_mtx);  // Protejează dlopen
    
    void *handle = dlopen("./libFunctionLib.so", RTLD_LAZY);
    
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        pthread_mutex_unlock(&dlopen_mtx);  // Deblochează mutexul
        exit(EXIT_FAILURE);
    }
    
    dlerror();  // Resetează starea erorii
    
    int (*add)(int, int) = (int (*)(int, int))dlsym(handle, "add");
    char *error = dlerror();
    
    if (error != NULL) {
        fprintf(stderr, "%s\n", error);
        pthread_mutex_unlock(&dlopen_mtx);  // Deblochează mutexul
        exit(EXIT_FAILURE);
    }
    
    pthread_mutex_unlock(&dlopen_mtx);  // Deblochează mutexul

    uint32_t value_1;
    uint32_t value_2;

    ExtractInt(packet, &value_1);
    ExtractInt(packet, &value_2);

    int returnVal = add((int)value_1, (int)value_2);

    Clear(packet);
    AppendInt(packet, returnVal);
}

void callMultiplyFunction(Packet *packet)
{
    pthread_mutex_lock(&dlopen_mtx);  // Protejează dlopen
    
    void *handle = dlopen("./libFunctionLib.so", RTLD_LAZY);
    
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        pthread_mutex_unlock(&dlopen_mtx);  // Deblochează mutexul
        exit(EXIT_FAILURE);
    }
    
    dlerror();  // Resetează starea erorii
    
    int (*multiply)(int, int) = (int (*)(int, int))dlsym(handle, "multiply");
    char *error = dlerror();
    
    if (error != NULL) {
        fprintf(stderr, "%s\n", error);
        pthread_mutex_unlock(&dlopen_mtx);  // Deblochează mutexul
        exit(EXIT_FAILURE);
    }
    
    pthread_mutex_unlock(&dlopen_mtx);  // Deblochează mutexul

    uint32_t value_1;
    uint32_t value_2;

    ExtractInt(packet, &value_1);
    ExtractInt(packet, &value_2);

    int returnVal = multiply((int)value_1, (int)value_2);

    Clear(packet);
    AppendInt(packet, returnVal);
}

void callMaxArrayFunction(Packet *packet)
{
    pthread_mutex_lock(&dlopen_mtx);  // Protejează dlopen
    
    void *handle = dlopen("./libFunctionLib.so", RTLD_LAZY);
    
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        pthread_mutex_unlock(&dlopen_mtx);  // Deblochează mutexul
        exit(EXIT_FAILURE);
    }
    
    dlerror();  // Resetează starea erorii
    
    float (*max_array)(float*, int) = (float (*)(float*, int))dlsym(handle, "max_array");
    char *error = dlerror();
    
    if (error != NULL) {
        fprintf(stderr, "%s\n", error);
        pthread_mutex_unlock(&dlopen_mtx);  // Deblochează mutexul
        exit(EXIT_FAILURE);
    }
    
    pthread_mutex_unlock(&dlopen_mtx);  // Deblochează mutexul

    float* array;
    uint32_t size;
    ExtractArray(packet,(void**)(&array),&size,TYPE_FLOAT);

    float returnVal = max_array(array,size);

    Clear(packet);
    AppendFloat(packet, returnVal);
    free(array);
}

void callFunction(Packet *packet)
{
    char *functionName;
    ExtractString(packet, &functionName);

    if (strcmp(functionName, "add") == 0)
    {
        callSumFunction(packet);
    }else 
        if(strcmp(functionName, "multiply") == 0)
        {
            callMultiplyFunction(packet);
        }else 
            if(strcmp(functionName,"max_array") == 0)
            {
                callMaxArrayFunction(packet);
            }

}

void generateAsyncPacket(sendAsync* sendStruct,Packet* packet){

    sendStruct->packet.packetType = ACK;
    sendStruct->idPacket = generatePacketID();
    sendStruct->packet = *packet;
    sendStruct->state = WAITING;
}


