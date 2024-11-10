#ifndef PACKET_H
#define PACKET_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>

#define MaxPacketSize 8192

typedef enum
{
    SENDSYNC,
    SENDASYNC,
    REQUEST,
    ACK,
    NREADY,
    NACK,
    UNKNOWN
} PacketType;

typedef struct Packet
{
    PacketType packetType;
    char buffer[MaxPacketSize];
    uint32_t extractionOffset;
    uint32_t currentSize;
} Packet;

void Append(Packet *packet, const void *data, uint32_t size);
void AppendInt(Packet *packet, uint32_t data);
void AppendString(Packet *packet, char *data, uint32_t size);
void ExtractInt(Packet *packet, uint32_t *data);
void ExtractString(Packet *packet, char **data);
void Clear(Packet *packet);

#endif