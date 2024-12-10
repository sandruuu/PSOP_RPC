#include "functions.h"

int add(int a, int b)
{
    int ret = a + b;
    return ret;
}
void removeDuplicates(char *buffer, int *size)
{
    char result[*size];
    result[0] = '\0';
    char *lastWord = NULL;
    char *token = strtok(buffer, " ");
    int index = 0;

    while (token)
    {
        if (!lastWord || strcmp(lastWord, token) != 0)
        {
            if (index > 0)
            {
                result[index++] = ' ';
            }
            strcpy(result + index, token);
            index += strlen(token);
            if (token != NULL)
            {
                lastWord = token;
            }
        }

        token = strtok(NULL, " ");
    }

    strcpy(buffer, result);
    *size = index;
}
int longestAscendingDigitNumber(int *arr, int size)
{
    int longestSubseq = 0;
    int longestSubseqValue = 0;
    for (int i = 0; i < size; i++)
    {
        int increasingSubseqValue = 0;
        int aux = arr[i];
        int lastDigit = aux % 10;
        aux = aux / 10;
        while (aux != 0)
        {
            int digit = aux % 10;
            aux = aux / 10;
            if (digit > lastDigit)
            {
                break;
            }
            increasingSubseqValue++;
        }
        if (aux == 0)
        {
            if (longestSubseqValue < increasingSubseqValue)
            {
                longestSubseq = arr[i];
            }
        }
    }
    return longestSubseq;
}
int calculateWordFrequency(char *buffer, char *word)
{
    int count = 0;
    char *token;

    token = strtok(buffer, " .!,?");
    while (token != NULL)
    {
        if (strcmp(token, word) == 0)
        {
            count++;
        }
        token = strtok(NULL, " .!,?");
    }

    return count;
}
void rotateArray(float *arr, int size, int k, char *direction)
{
    k = k % size;

    if (k == 0)
    {
        return;
    }

    if ((strcmp(direction, "right")) == 0)
    {
        float *temp = (float *)malloc(k * sizeof(float));
        memcpy(temp, arr + (size - k), k * sizeof(float));

        memmove(arr + k, arr, (size - k) * sizeof(float));

        memcpy(arr, temp, k * sizeof(float));

        free(temp);
    }
    else if ((strcmp(direction, "left")) == 0)
    {
        float *temp = (float *)malloc(k * sizeof(float));
        memcpy(temp, arr, k * sizeof(float));

        memmove(arr, arr + k, (size - k) * sizeof(float));

        memcpy(arr + (size - k), temp, k * sizeof(float));

        free(temp);
    }
}