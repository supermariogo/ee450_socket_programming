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

#include "auctionserver.h"

#ifndef PORT
#define SERVER_PORT_PHASE_1 "1100939"
#endif

#define BACKLOG 10


int main(int argc, const char *argv[])
{	
	
	return 0;
}

