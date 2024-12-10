#include "packet.h"

void Clear(Packet *packet)
{
    memset(packet->buffer, 0, MaxPacketSize);
    packet->extractionOffset = 0;
    packet->currentSize = 0;
    packet->packetType = UNKNOWN;
}

void Append(Packet *packet, const void *data, uint32_t size)
{
    if ((packet->currentSize + size + 1) > MaxPacketSize)
    {
        printf("[Append(Packet*, const void*, uint32_t)] - Packet size exceeded max packet size.\n");
        exit(EXIT_FAILURE);
    }
    memcpy(packet->buffer + packet->currentSize, (char *)data, size);
    packet->currentSize += size;
}

void AppendInt(Packet *packet, uint32_t data)
{
    uint32_t convertedData = htonl(data);
    Append(packet, &convertedData, sizeof(uint32_t));
}

void AppendString(Packet *packet, char *data, uint32_t size)
{
    AppendInt(packet, size);
    Append(packet, data, size);
}

void ExtractInt(Packet *packet, uint32_t *data)
{
    if ((packet->extractionOffset + sizeof(uint32_t)) > packet->currentSize)
    {
        printf("[ExtractInt(Packet*, uint32_t*)] - Extraction offset exceeded buffer size.\n");
        exit(EXIT_FAILURE);
    }

    memcpy(data, packet->buffer + packet->extractionOffset, sizeof(uint32_t));
    *data = ntohl(*data);

    packet->extractionOffset += sizeof(uint32_t);
}

void ExtractString(Packet *packet, char **data)
{
    uint32_t stringSize = 0;
    ExtractInt(packet, &stringSize);

    if ((packet->extractionOffset + stringSize) > packet->currentSize)
    {
        printf("[ExtractString(Packet*, char**)] - Extraction offset exceeded buffer size.\n");
        exit(EXIT_FAILURE);
    }

    *data = (char *)malloc(stringSize + 1);

    if (*data == NULL)
    {
        perror("[ExtractString(Packet*, char**)] - ");
        exit(EXIT_FAILURE);
    }

    memcpy(*data, packet->buffer + packet->extractionOffset, stringSize);
    (*data)[stringSize] = '\0';

    packet->extractionOffset += stringSize;
}

void ExtractIntArray(Packet *packet, int **data, uint32_t *size)
{
    ExtractInt(packet, size);
    (*((uint32_t **)data)) = (uint32_t *)malloc(sizeof(uint32_t) * (*size));
    if (*data == NULL)
    {
        perror("ExtractIntArray(Packet*, void**, uint32_t*) - ");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < *size; i++)
    {
        uint32_t aux;
        ExtractInt(packet, &aux);
        (*((uint32_t **)data))[i] = aux;
    }
}

void AppendIntArray(Packet *packet, int *data, uint32_t size)
{
    AppendInt(packet, size);
    for (int i = 0; i < size; i++)
        AppendInt(packet, (uint32_t)(((uint32_t *)(data))[i]));
}

void AppendFloat(Packet *packet, float data)
{
    uint32_t convertedData;
    memcpy(&convertedData, &data, sizeof(float));
    convertedData = htonl(convertedData);
    Append(packet, &convertedData, sizeof(uint32_t));
}

void ExtractFloat(Packet *packet, float *data)
{
    if ((packet->extractionOffset + sizeof(uint32_t)) > packet->currentSize)
    {
        printf("[ExtractFloat(Packet*, float*)] - Extraction offset exceeded buffer size.\n");
        exit(EXIT_FAILURE);
    }
    uint32_t temp;
    memcpy(&temp, packet->buffer + packet->extractionOffset, sizeof(uint32_t));
    temp = ntohl(temp);
    memcpy(data, &temp, sizeof(float));
    packet->extractionOffset += sizeof(uint32_t);
}

void AppendFloatArray(Packet *packet, float *data, uint32_t size)
{
    AppendInt(packet, size);
    for (int i = 0; i < size; i++)
    {
        AppendFloat(packet, data[i]);
    }
}

void ExtractFloatArray(Packet *packet, float **data, uint32_t *size)
{
    ExtractInt(packet, size);
    (*data) = (float *)malloc(sizeof(float) * (*size));
    if (*data == NULL)
    {
        perror("ExtractFloatArray(Packet*, void**, uint32_t*) - ");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < *size; i++)
    {
        float aux;
        ExtractFloat(packet, &aux);
        (*data)[i] = aux;
    }
}