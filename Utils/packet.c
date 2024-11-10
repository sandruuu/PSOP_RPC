#include "packet.h"

void Clear(Packet* packet){
    memset(packet->buffer, 0, MaxPacketSize);
    packet->extractionOffset = 0;
    packet->currentSize = 0;
    packet->packetType = UNKNOWN;
    
}

void Append(Packet* packet, const void* data, uint32_t size){
    if ((packet->currentSize + size +1) > MaxPacketSize) {
        printf("[Append(Packet* packet, const void* data, uint32_t size)] - Packet size exceeded max packet size.\n");
        exit(EXIT_FAILURE);
    }
    memcpy(packet->buffer + packet->currentSize, (char*)data, size);
    packet->currentSize += size;
}

void AppendInt(Packet* packet, uint32_t data){
    uint32_t convertedData = htonl(data);
    Append(packet, &convertedData, sizeof(uint32_t));
}

void AppendString(Packet* packet, char* data, uint32_t size){
    AppendInt(packet, size);
    Append(packet, data, size);
}

void ExtractInt(Packet* packet, uint32_t* data){
    if ((packet->extractionOffset + sizeof(uint32_t)) > packet->currentSize) {
        printf("[ExtractInt(Packet* packet, uint32_t* data)] - Extraction offset exceeded buffer size.\n");
        exit(EXIT_FAILURE);
    }

    memcpy(data, packet->buffer + packet->extractionOffset, sizeof(uint32_t));
    *data = ntohl(*data);

    packet->extractionOffset += sizeof(uint32_t);
}

void ExtractString(Packet* packet, char** data){
    uint32_t stringSize = 0;
    ExtractInt(packet,&stringSize);
    
    if ((packet->extractionOffset + stringSize) > packet->currentSize) {
        printf("[ExtractString(Packet* packet, char** data)] - Extraction offset exceeded buffer size.\n");
        exit(EXIT_FAILURE);
    }

    *data = (char*)malloc(stringSize + 1);

    if (*data == NULL) {
        perror("[ExtractString(Packet* packet, char** data)] - ");
        exit(EXIT_FAILURE);
    }

    memcpy(*data, packet->buffer + packet->extractionOffset, stringSize);
    (*data)[stringSize] = '\0';

    packet->extractionOffset += stringSize;
}