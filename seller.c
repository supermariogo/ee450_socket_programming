#include "port.h"
#include "bidder.h"


int main(int argc, char *argv[])
{
	int bidder_phase1_sfd_c; //socket fd 
	struct sockaddr_in bidder_phase1_addr; // connect info
	
	memset(&bidder_phase1_addr, 0, sizeof(bidder_phase1_addr)); 
    bidder_phase1_addr.sin_family = AF_INET; 
    bidder_phase1_addr.sin_addr.s_addr = htonl(INADDR_ANY); //use local address 
    bidder_phase1_addr.sin_port = htons(SERVER_PHASE1_PORT); 
 
	bidder_phase1_sfd_c = socket(AF_INET, SOCK_STREAM, 0); //create socket fd
    if (connect(bidder_phase1_sfd_c, (struct sockaddr *) &bidder_phase1_addr, sizeof(bidder_phase1_addr)) == -1) {
        close(bidder_phase1_sfd_c);
        perror("bidder error : connect");
    }	
	if (send(bidder_phase1_sfd_c, "hello world",12,0)==-1){
		close(bidder_phase1_sfd_c);
        perror("bidder error : send");	
	} 

	if (send(bidder_phase1_sfd_c, "hello world",12,0)==-1){
		close(bidder_phase1_sfd_c);
        perror("bidder error : send");	
	}

	close(bidder_phase1_sfd_c);	
    return 0;
}
