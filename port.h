#ifndef SERVER_PHASE1_PORT

#define LOCALHOST "127.0.0.1"
#define NUNKI "68.181.201.3" 
#define SERVER_PHASE1_PORT 2039
#define SERVER_PHASE2_PORT 2139
#define BIDDER1_PHASE3_PORT 14039
#define BIDDER2_PHASE3_PORT 14139
#define BUFFLEN 1024
#define BACKLOG 10     // how many pending connections queue will hold
#define max(A,B) (((A)>(B)) ? (A) : (B))
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


typedef struct tag_user_data {
	char type[2];
    char name[32];
	char password[32];
	char account[32];
	char command[256];
	char ip[17];
	char port[6];
	int  authentication_success;
	char next_phase_ip[17];
	char next_phase_port[5];

} user_data_t;

typedef struct tag_item {
    char seller_name[32];
	char item_name[32];
	int price;
	char bidder_name[2][32];
	int  bidder_price[2];
	char buyer_name[32];
	int buyer_price;
} item_t;

extern void phase1_processing(int type, int X, user_data_t * self_info);
extern void file_read_self_info(int type, int X, user_data_t * self_info);

extern int get_peer_ip_or_port(int sockfd, char *dest, int type);
extern int get_my_ip_or_port(int server_fd, char *dest, int type);

extern item_t * phase3_file_to_list(char * file_content, int num);

