
#define _GNU_SOURCE 500
#include <stdio.h> 
#include<errno.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <pthread.h>
#define MAX 1000
#define PORT 8080

int add(int a, int b);