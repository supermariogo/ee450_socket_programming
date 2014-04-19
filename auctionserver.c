#include "port.h"
#include "auctionserver.h"
#include <unistd.h>
#include <fcntl.h>
user_data_t user[10];
int user_number=0; // count from zero, valid user, success = 1


int main(int argc, char * argv[]) 
{ 
	file_read_reg();

	int phase1_s_s;
	phase1_s_s=phase1_socket_bind_listen();	
	phase1_handle_connect(phase1_s_s); 
	close(phase1_s_s); 
	
	return 0; 
}



int phase1_socket_bind_listen(){
	int phase1_s_s ; 
	struct sockaddr_in phase1_addr_info;
	phase1_s_s = socket(AF_INET, SOCK_STREAM, 0); 
	
	memset(&phase1_addr_info, 0, sizeof(phase1_addr_info)); 
	phase1_addr_info.sin_family = AF_INET; 
	phase1_addr_info.sin_addr.s_addr = htonl(INADDR_ANY); 
	phase1_addr_info.sin_port = htons(SERVER_PHASE1_PORT);
	
	if (bind(phase1_s_s, (struct sockaddr *) &phase1_addr_info, sizeof(phase1_addr_info)) == -1) {
		perror("server error : bind");
		exit(1);
	}
	
	if (listen(phase1_s_s, BACKLOG) == -1) {
		perror("server error : listen");
		exit(1);
	}
	fprintf(stdout, "server : begin to listen\n");
	return phase1_s_s;

} 
void phase1_handle_connect(int phase1_s_s) 
{ 
        int s_c; 
        struct sockaddr_in from; 
        socklen_t len = sizeof(from); 
        pthread_t thread_do[4]; 
		int phase1_thread_number=0;

        while (1) 
        { 
                s_c = accept(phase1_s_s, (struct sockaddr *) &from, &len); 
                if (s_c > 0) 
                {
					pthread_create(&thread_do[phase1_thread_number], NULL, phase1_handle_request, (void *) &s_c);
					fprintf(stdout, "server : thread %d created\n",(int)thread_do);
					phase1_thread_number++;
					//ptherad_join() is set as default
                }
				if(phase1_thread_number==4){
					(void) pthread_join(thread_do[0], NULL);	
					(void) pthread_join(thread_do[1], NULL);
					(void) pthread_join(thread_do[2], NULL);
					(void) pthread_join(thread_do[3], NULL);
					printf("End of Phase 1 for Auction Server\n");
					fprintf(stdout, "--------------------------------------------\n");
					return;
				}

        } 
} 

void * phase1_handle_request(void * argv) 
{ 
	int s_c = * ((int *) argv); 
 	char buff[BUFFLEN]; 
 	int n = 0; 
 	memset(buff, 0, BUFFLEN);
	int i;
	char command[256];
	char myip[17];
	user_data_t un_auth_user;
	
	n = recv(s_c, buff, BUFFLEN, 0);
	if (n > 0) 
 		fprintf(stdout, "server : message from thread %d, %s \n",(int)pthread_self(),buff);
	else {
		fprintf(stderr, "client disconnected, socket close thread exit\n");
		close(s_c);
		pthread_exit(NULL);
	}
	
	i=phase1_login_check(buff,&un_auth_user);

	if(i==-1){ //authentication reject
		get_peer_ip_or_port(s_c,un_auth_user.ip,1);
		get_peer_ip_or_port(s_c,un_auth_user.port,2);
		printf("Phase 1: Authentication request. User%d: Username %s Password: %s Bank Account: %s User IP Addr: %s. ",i,un_auth_user.name,un_auth_user.password,un_auth_user.account,un_auth_user.ip);
		fprintf(stdout, "User Port %s ",un_auth_user.port);
		printf("Authorized: reject\n");	
		if (send(s_c, "Rejected#",strlen("Rejected#"),0)==-1)
			perror("server error : send");
		close(s_c);
		pthread_exit(NULL);
	}else{ //authentication accept 
		user[i].authentication_success=1;
		get_peer_ip_or_port(s_c,user[i].ip,1);
		get_peer_ip_or_port(s_c,user[i].port,2);
		printf("Phase 1: Authentication request. User%d: Username %s Password: %s Bank Account: %s User IP Addr: %s. ",i,user[i].name,user[i].password,user[i].account,user[i].ip);
		fprintf(stdout, "User Port %s ",user[i].port);	
		printf("Authorized: accept\n");
	}
	
	//authentication accept
	if(user[i].type[0]=='1'){
		sprintf(command,"Accepted#");
		if (send(s_c, command,strlen(command),0)==-1){
			perror("server error : send");
			close(s_c);
			pthread_exit(NULL);
		}
	}
	else if(user[i].type[0]=='2'){
		get_my_ip(s_c, myip);	
		sprintf(command,"Accepted#%s#%d#",myip,SERVER_PHASE2_PORT);
		if (send(s_c, command,strlen(command),0)==-1){
			perror("server error : send");
			close(s_c);
			pthread_exit(NULL);
		}
		else{
			printf("Phase 1: Auction Server IP Address: %s PreAuction Port Number: %d sent to the Seller",myip,SERVER_PHASE2_PORT);
		}
	}
	else {
		fprintf(stderr, "ivalid error,quit\n");
		exit(0);
	}

	fprintf(stdout, "phase1 for this user complete\n-----------------------------\n");
	close(s_c);
	pthread_exit(NULL);
	
 	return NULL;
}

int phase1_login_check(char *buff, user_data_t * un_auth_user){
	user_data_t temp_user;
	char * tok;
	int i;
	fprintf(stdout, "string before strtok(): %s\n", buff);
	tok = strtok(buff," #");
	if(strcmp(tok,"Login")!=0){
		fprintf(stderr, "invalid login command\n");
		pthread_exit(NULL);	
	}
	tok = strtok(NULL, " ");
	strcpy(temp_user.type,tok);
	tok = strtok(NULL, " ");
	strcpy(temp_user.name,tok);
	tok = strtok(NULL, " ");
	strcpy(temp_user.password,tok);
	tok = strtok(NULL, " \n");
	strcpy(temp_user.account,tok);

	//check
	for (i = 0; i < user_number; i++) {
		if(strcmp(temp_user.name,user[i].name))
			continue;
		if(strcmp(temp_user.password,user[i].password))
			continue;
		if(strcmp(temp_user.account,user[i].account))
			continue;
		strcpy(user[i].type,temp_user.type);
		fprintf(stdout, "login success type=%s\n",user[i].type);
		return i;
	}

	//for no match
	*un_auth_user=temp_user;		
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





 
