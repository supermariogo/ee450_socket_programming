#ifndef SERVER_PHASE1_PORT


#define SERVER_PHASE1_PORT 2039
#define BUFFLEN 1024
#define BACKLOG 10     // how many pending connections queue will hold

#endif


#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <errno.h> 
#include <string.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <sys/wait.h>
#include <pthread.h>
#include <signal.h>





