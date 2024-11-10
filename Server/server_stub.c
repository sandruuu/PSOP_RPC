#include "server_stub.h"

int generateID() {
    static int id = 0;
    id++;
    return id;
}

void callSumFunction(Packet *packet)
{
    void *handle = dlopen("./libFunctionLib.so", RTLD_LAZY);

    if (!handle)
    {
        fprintf(stderr, "%s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    dlerror();
    int (*add)(int, int) = (int (*)(int, int))dlsym(handle, "add");
    char *error = dlerror();

    if (error != NULL)
    {
        fprintf(stderr, "%s\n", error);
        exit(EXIT_FAILURE);
    }
    uint32_t value_1;
    uint32_t value_2;

    ExtractInt(packet, &value_1);
    ExtractInt(packet, &value_2);

    int returnVal = add((int)value_1, (int)value_2);

    Clear(packet);
    AppendInt(packet, returnVal);
}

void callFunction(Packet *packet)
{
    char *data;
    ExtractString(packet, &data);

    if (strcmp(data, "add") == 0)
    {
        callSumFunction(packet);
    }
    packet->packetType = SENDSYNC;
}

int callAsyncFunction(Packet* packet){
    int id = generateID();
    Clear(packet);
    packet->packetType = SENDASYNC;
    AppendInt(packet, id);
    return id;
}