#include "port.h"
#include "bidder.h"

user_data_t self_info;

int main(int argc, char *argv[])
{
	file_read_self_info();

	int bidder_phase1_sfd_c; //socket fd 
	struct sockaddr_in bidder_phase1_addr_info; // connect info
	
	memset(&bidder_phase1_addr_info, 0, sizeof(bidder_phase1_addr_info)); 
    bidder_phase1_addr_info.sin_family = AF_INET; 
    bidder_phase1_addr_info.sin_addr.s_addr = htonl(INADDR_ANY); //use local address 
    bidder_phase1_addr_info.sin_port = htons(SERVER_PHASE1_PORT); 

	fprintf(stdout, "Hello, I am bidder%d\n",BIDDERX);

	bidder_phase1_sfd_c = socket(AF_INET, SOCK_STREAM, 0); //create socket fd
    if (connect(bidder_phase1_sfd_c, (struct sockaddr *) &bidder_phase1_addr_info, sizeof(bidder_phase1_addr_info)) == -1) {
        close(bidder_phase1_sfd_c);
        perror("bidder error : connect");
		exit(-1);
    }	

	if (send(bidder_phase1_sfd_c, self_info.command,strlen(self_info.command),0)==-1)
    	perror("bidder error : send");
		
	close(bidder_phase1_sfd_c);	
    return 0;
}


void file_read_self_info(void)
{
	FILE * fp;
	if(BIDDERX==1)
		fp = fopen ("bidderPass1.txt","r"); 
	else
		fp = fopen ("bidderPass2.txt","r");  
	char current_line[1024];
	char *tok = NULL;
	if (fgets(current_line, 1024, fp)!=NULL ){

		fprintf(stdout, "string before strtok(): %s\n", current_line);
		tok = strtok(current_line," ");
		strcpy(self_info.type,tok);	
		tok = strtok(NULL," ");	
		strcpy(self_info.name,tok);
		tok = strtok(NULL, " ");
		strcpy(self_info.password,tok);
		tok = strtok(NULL, " \n");
		strcpy(self_info.account,tok);

		fprintf(stdout, "user.type=%s\n",self_info.type);	
		fprintf(stdout, "user.name=%s\n",self_info.name);
		fprintf(stdout, "user.password=%s\n",self_info.password);
		fprintf(stdout, "user.account=%s\n",self_info.account);
		
	}
	fclose(fp);

	sprintf (self_info.command, "Login#%s %s %s %s", self_info.type, self_info.name, self_info.password, self_info.account);
	//"Login#type username password bankaccount"
	fprintf(stdout, "command: %s\n",self_info.command);


}
