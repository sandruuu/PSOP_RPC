#include "packet.h"

void Clear(Packet* packet){
    memset(packet->buffer, 0, MaxPacketSize);
    packet->extractionOffset = 0;
    packet->currentSize = 0;
    //packet->packetType = UNKNOWN;
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

void AppendFloat(Packet *packet, float data)
{
    uint32_t convertedData;
    memcpy(&convertedData, &data, sizeof(float));
    convertedData = htonl(convertedData);
    Append(packet, &convertedData, sizeof(uint32_t));
}

void AppendArray(Packet *packet, void *data, uint32_t size, DataType dataType)
{
     switch(dataType){
        
        case TYPE_INT:
            AppendInt(packet,size);
            for(int i = 0; i < size ; i++)
                AppendInt(packet,(uint32_t)(((uint32_t*)(data))[i]));
            break;
        
        case TYPE_FLOAT:
            AppendInt(packet,size);
            for(int i = 0; i < size ; i++)
                AppendFloat(packet,(float)(((float*)(data))[i]));
            break;
         default:
            printf("Unknown dataType\n");
            break;

    }
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

void ExtractFloat(Packet *packet, float *data)
{
    if ((packet->extractionOffset + sizeof(uint32_t)) > packet->currentSize) {
        printf("[ExtractFloat(Packet* packet, float* data)] - Extraction offset exceeded buffer size.\n");
        exit(EXIT_FAILURE);
    }

    uint32_t temp;
    memcpy(&temp, packet->buffer + packet->extractionOffset, sizeof(uint32_t));
    temp = ntohl(temp);
    memcpy(data, &temp, sizeof(float));

    packet->extractionOffset += sizeof(uint32_t);
}

void ExtractArray(Packet *packet, void **data, uint32_t *size, DataType dataType)
{
     switch(dataType){
        
        case TYPE_INT:
        ExtractInt(packet,size);
            (*((uint32_t**)data)) = (uint32_t*)malloc(sizeof(uint32_t)* (*size));
            
            if(*data == NULL)
            {
                perror("malloc - ");
                exit(EXIT_FAILURE);
            }
            
            for(int i = 0; i < *size ; i++)
                {
                    uint32_t buffer;
                    ExtractInt(packet,&buffer);
                    (*((uint32_t**)data))[i] = buffer;
                }
        
            break;
        
        case TYPE_FLOAT:

            ExtractInt(packet,size);
            
            (*((float**)data)) = (float*)malloc(sizeof(float)* (*size));
            if(*data == NULL)
            {
                perror("malloc - ");
                exit(EXIT_FAILURE);
            }
            for(int i = 0; i < *size ; i++)
                {
                    float buffer;
                    ExtractFloat(packet,&buffer);
                    (*((float**)data))[i] = buffer;
                }
            break;
         default:
            printf("Unknown dataType\n");
            break;
    }
}
void ExtractString(Packet* packet, char** data){
    uint32_t stringSize = 0;
    ExtractInt(packet, &stringSize);
    
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
