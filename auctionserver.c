#include "port.h"
#include "auctionserver.h"
#include <pthread.h>
int main(int argc, char * argv[]) 
{ 
        int server_phase1_sfd_s ; 
        struct sockaddr_in server_phase1_addr_info;
        server_phase1_sfd_s = socket(AF_INET, SOCK_STREAM, 0); 
 
        memset(&server_phase1_addr_info, 0, sizeof(server_phase1_addr_info)); 
        server_phase1_addr_info.sin_family = AF_INET; 
        server_phase1_addr_info.sin_addr.s_addr = htonl(INADDR_ANY); 
        server_phase1_addr_info.sin_port = htons(SERVER_PHASE1_PORT); 
 
		if (bind(server_phase1_sfd_s, (struct sockaddr *) &server_phase1_addr_info, sizeof(server_phase1_addr_info)) == -1) {
			perror("server error : bind");
			exit(1);
		}

        if (listen(server_phase1_sfd_s, BACKLOG) == -1) {
        	perror("server error : listen");
			exit(1);
		}
		fprintf(stdout, "server : begin to listen\n");
        handle_connect(server_phase1_sfd_s); 
        close(server_phase1_sfd_s); 
 
        return 0; 
} 
 
static void handle_connect(int server_phase1_sfd_s) 
{ 
        int s_c; 
        struct sockaddr_in from; 
        socklen_t len = sizeof(from); 
        pthread_t thread_do; 
		
        while (1) 
        { 
                s_c = accept(server_phase1_sfd_s, (struct sockaddr *) &from, &len); 
                if (s_c > 0) 
                {
					pthread_create(&thread_do, NULL, handle_request, (void *) &s_c);
					fprintf(stdout, "server : created thread %d\n",(int)thread_do);
					//ptherad_join() is set as default
                } 
        } 
} 

static void * handle_request(void * argv) 
{ 
	int s_c = * ((int *) argv); 
 	char buff[BUFFLEN]; 
 	int n = 0; 
 	memset(buff, 0, BUFFLEN); 
 	
 	while (1) {
		n = recv(s_c, buff, BUFFLEN, 0);
		if (n > 0) 
 		{ 
 			fprintf(stdout, "server : message from thread %d, %s \n",(int)pthread_self(),buff);       
 		} 
 	}
 	return NULL;
} 
 

