
#define _GNU_SOURCE 500
#include <sys/epoll.h>
#include <fcntl.h>
#include <stdio.h> 
#include <errno.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> // read(), write(), close()
#include <signal.h>

#define MAX 1000
#define PORT 8080

int add(int a, int b);
int multiply(int a, int b);
float max_array(float *array, int size);

