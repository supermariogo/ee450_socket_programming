#include "port.h"
#include "auctionserver.h"
#include <unistd.h>
#include <fcntl.h>
user_data_t user[10];
int user_number=0; // count from zero

int main(int argc, char * argv[]) 
{ 
	file_read_reg();

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
					fprintf(stdout, "server : thread %d created\n",(int)thread_do);
					//ptherad_join() is set as default
                } 
				fprintf(stderr, "am i waiting for accept? \n");
        } 
} 

static void * handle_request(void * argv) 
{ 
	int s_c = * ((int *) argv); 
 	char buff[BUFFLEN]; 
 	int n = 0; 
 	memset(buff, 0, BUFFLEN);
	int i;
	char command[256];
	char myip[17];
	
	n = recv(s_c, buff, BUFFLEN, 0);
	if (n > 0) 
 		fprintf(stdout, "server : message from thread %d, %s \n",(int)pthread_self(),buff);
	else {
		fprintf(stderr, "client disconnected, socket close thread exit\n");
		close(s_c);
		pthread_exit(NULL);
	}
	i=phase1_login_check(buff);
	get_peer_ip_or_port(s_c,user[i].ip,1);
	get_peer_ip_or_port(s_c,user[i].port,2);
	
	if(i==-1){
		//authentication fail
		printf("Phase 1: Authentication request. User%d: Username %s Password: %s Bank Account: %s User IP Addr: %s. Authorized: reject\n",i,user[i].name,user[i].password,user[i].account,user[i].ip);	
		fprintf(stderr, "client disconnected, socket close thread exit\n");
		if (send(s_c, "Rejected#",strlen("Rejected#"),0)==-1)
			perror("server error : send");
		close(s_c);
		pthread_exit(NULL);
		
	}else{
		user[i].authentication_success=1; 
		printf("Phase 1: Authentication request. User%d: Username %s Password: %s Bank Account: %s User IP Addr: %s. Authorized: accept\n",i,user[i].name,user[i].password,user[i].account,user[i].ip);
	}
	

	//authentication success
	if(user[i].type[0]=='2'){
		get_my_ip(s_c,myip);	
		sprintf(command,"Accepted#%s#%d#",myip,SERVER_PHASE2_PORT);
		if (send(s_c, command,strlen(command),0)==-1)
			perror("server error : send");
		else{
			printf("Phase 1: Auction Server IP Address: %s PreAuction Port Number: %d sent to the Seller",myip,SERVER_PHASE2_PORT);
			
		}
	}else if(user[i].type[0]=='1'){
		sprintf(command,"Accepted#");
		if (send(s_c, command,strlen(command),0)==-1)
			perror("server error : send");
	}

	fprintf(stdout, "phase1 complete\n-----------------------------\n");
	close(s_c);
	pthread_exit(NULL);
	
 	return NULL;
}

int phase1_login_check(char *buff){
	user_data_t login_command;
	char * tok;
	int i;
	fprintf(stdout, "string before strtok(): %s\n", buff);
	tok = strtok(buff," #");
	if(strcmp(tok,"Login")!=0){
		fprintf(stderr, "invalid login command\n");
		pthread_exit(NULL);	
	}
	tok = strtok(NULL, " ");
	strcpy(login_command.type,tok);
	tok = strtok(NULL, " ");
	strcpy(login_command.name,tok);
	tok = strtok(NULL, " ");
	strcpy(login_command.password,tok);
	tok = strtok(NULL, " \n");
	strcpy(login_command.account,tok);

	//check
	for (i = 0; i < user_number; i++) {
		if(strcmp(login_command.name,user[i].name))
			continue;
		if(strcmp(login_command.password,user[i].password))
			continue;
		if(strcmp(login_command.account,user[i].account))
			continue;
		strcpy(user[i].type,login_command.type);
		fprintf(stdout, "login success type=%s\n",user[i].type);
		// assign user ip and port
		

		return i;
	}
	return -1;

}

void file_read_reg(void)
{
	FILE * fp;
	fp = fopen ("Registration.txt","r"); 
	char current_line[1024];
	char *tok = NULL;
	int i=0;
	while (fgets(current_line, 1024, fp)!=NULL ){

		fprintf(stdout, "string before strtok(): %s\n", current_line);
		tok = strtok(current_line," ");
		strcpy(user[i].name,tok);
		tok = strtok(NULL, " ");
		strcpy(user[i].password,tok);
		tok = strtok(NULL, " \n");
		strcpy(user[i].account,tok);

		fprintf(stdout, "user.name=%s\n",user[i].name);
		fprintf(stdout, "user.password=%s\n",user[i].password);
		fprintf(stdout, "user.account=%s\n",user[i].account);
		
		i++;
	}
	user_number=i;// count from zero
	fprintf(stdout, "user_number=%d\n",user_number);
	fclose(fp);

}





 
