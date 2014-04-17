#include "port.h"
#include "auctionserver.h"

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
 			fprintf(stdout, "server : message from thread %d, %s \n",(int)pthread_self(),buff);       
		fprintf(stderr, "am i waiting?\n");
		//phase1_login_check(buff);
 	}
	
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
		return i;
	}
	return 999;

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
 

