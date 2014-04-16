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

#include "auctionserver.h"

#define SERVER_PORT 3490  // the port users will be connecting to
#define BUFFLEN 1024
#define BACKLOG 10     // how many pending connections queue will hold



int main(int argc, char * argv[]) 
{ 
        int s_s; 
        struct sockaddr_in local;
        s_s = socket(AF_INET, SOCK_STREAM, 0); 
 
        memset(&local, 0, sizeof(local)); 
        local.sin_family = AF_INET; 
        local.sin_addr.s_addr = htonl(INADDR_ANY); 
        local.sin_port = htons(SERVER_PORT); 
 
		if (bind(s_s, (struct sockaddr *) &local, sizeof(local)) == -1) {
			perror("bind");
			exit(1);
		}

        if (listen(s_s, BACKLOG) == -1) {
        	perror("listen");
			exit(1);
		}

        handle_connect(s_s); 
        close(s_s); 
 
        return 0; 
} 
 
static void handle_connect(int s_s) 
{ 
        int s_c; 
        struct sockaddr_in from; 
        socklen_t len = sizeof(from); 
        pthread_t thread_do; 
 
        while (1) 
        { 
                s_c = accept(s_s, (struct sockaddr *) &from, &len); 
                if (s_c > 0) 
                {
					pthread_create(&thread_do, NULL, handle_request, (void *) &s_c);
					fprintf(stdout, "new thread for connection created\n");
					//ptherad_join() is set as default
                } 
        } 
} 

static void * handle_request(void * argv) 
{ 
        int s_c = * ((int *) argv); 
        char buff[BUFFLEN]; 
        char m_buff[BUFFLEN]; 
        while (1) { 
                int n = 0; 
                memset(buff, 0, BUFFLEN); 
                memset(m_buff, 0, BUFFLEN); 
                n = recv(s_c, buff, BUFFLEN, 0); 
                if (n > 0) 
                { 
					printf("接收到%s \r\n", buff);       
                } 
                close(s_c); 
        }
		return NULL;
} 
 

