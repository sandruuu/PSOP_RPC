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

int add(int a, int b){

    int ret =a+b;
    return ret;
}