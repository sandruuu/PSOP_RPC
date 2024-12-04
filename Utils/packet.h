#ifndef PACKET_H
#define PACKET_H
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>

#define MaxPacketSize 8192

typedef enum PacketType
{
    SENDSYNC,
    SENDASYNC,
    REQUEST,
    ACK,
    RESPONSE,
    DISCONNECT
} PacketType;


typedef enum DataType{
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING
} DataType;

typedef struct Packet
{
    PacketType packetType;
    char buffer[MaxPacketSize];
    uint32_t extractionOffset;
    uint32_t currentSize;

} Packet;

void Append(Packet *packet, const void *data, uint32_t size);
void AppendInt(Packet *packet, uint32_t data);
void AppendFloat(Packet *packet, float data);
void AppendArray(Packet *packet, void* data, uint32_t size, DataType dataType);
void AppendString(Packet *packet, char *data, uint32_t size);
void ExtractInt(Packet *packet, uint32_t *data);
void ExtractFloat(Packet *packet,float *data);
void ExtractString(Packet *packet, char **data);
void ExtractArray(Packet *packet,void** data, uint32_t *size,DataType dataType);
void Clear(Packet *packet);

#endif