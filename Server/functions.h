
#define _GNU_SOURCE 500
#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#define MAX 1000
#define PORT 8080

int add(int a, int b);
void removeDuplicates(char *buffer, int *size);
int longestAscendingDigitNumber(int *arr, int size);
int calculateWordFrequency(char *buffer, char *word);
void rotateArray(float *arr, int size, int k, char *direction);