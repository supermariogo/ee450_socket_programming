#include "port.h"
#include "MyList.h"
#include "auctionserver.h"
user_data_t user[10];
int user_number=0; // count from zero, valid user, success = 1
int phase1_thread_max_number=0;
int phase2_thread_max_number=0;
MyList * item_list[2];
pthread_barrier_t barr;

int main(int argc, char * argv[]) 
{ 

	memset(&user, 0, sizeof(user[10]));
	file_read_reg();

	int phase1_s_s;
	phase1_s_s=socket_bind_listen(SERVER_PHASE1_PORT);	
	phase1_handle_connect(phase1_s_s); 
	close(phase1_s_s);

	int phase2_s_s;
	phase2_s_s=socket_bind_listen(SERVER_PHASE2_PORT);
	phase2_handle_connect(phase2_s_s);
	close(phase2_s_s);

	return 0; 
}



int socket_bind_listen(uint16_t PORT){
	int s_s ; 
	struct sockaddr_in addr_info;
	s_s = socket(AF_INET, SOCK_STREAM, 0); 
	
	memset(&addr_info, 0, sizeof(addr_info)); 
	addr_info.sin_family = AF_INET; 
	addr_info.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_info.sin_port = htons(PORT);
	
	if (bind(s_s, (struct sockaddr *) &addr_info, sizeof(addr_info)) == -1) {
		perror("server error : bind");
		exit(1);
	}
	
	if (listen(s_s, BACKLOG) == -1) {
		perror("server error : listen");
		exit(1);
	}
	fprintf(stdout, "server : begin to listen\n");
	return s_s;

} 
void phase1_handle_connect(int phase1_s_s) 
{ 
	if(pthread_barrier_init(&barr, NULL, 4)){
	    printf("Could not create a barrier\n");
	    return ;
	}
	int s_c; 
	struct sockaddr_in from; 
	socklen_t len = sizeof(from); 
	pthread_t thread_do[4]; 
	while (1) 
	{ 
	        s_c = accept(phase1_s_s, (struct sockaddr *) &from, &len); 
	        if (s_c > 0) 
	        {
				pthread_create(&thread_do[phase1_thread_max_number], NULL, phase1_handle_request, (void *) &s_c);
				fprintf(stdout, "server : thread %d created\n",(int)thread_do);
				phase1_thread_max_number++;
	        }
			if(phase1_thread_max_number==4){
				pthread_join(thread_do[0], NULL);	
				pthread_join(thread_do[1], NULL);
				pthread_join(thread_do[2], NULL);
				pthread_join(thread_do[3], NULL);
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
	user_data_t un_auth_user;
 	int n = 0; 
	int i = 0;
	
	memset(buff, 0, BUFFLEN);	
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
		fprintf(stdout, "------------------------------------------\n");
		if (send(s_c, "Rejected#",strlen("Rejected#"),0)==-1){
			perror("server error : send");
			close(s_c);
			exit(-1);
		}
		goto SYNC;
	}else{ //authentication accept 
		user[i].authentication_success=1;
		get_peer_ip_or_port(s_c,user[i].ip,1);
		get_peer_ip_or_port(s_c,user[i].port,2);
		printf("Phase 1: Authentication request. User%d: Username %s Password: %s Bank Account: %s User IP Addr: %s. ",i,user[i].name,user[i].password,user[i].account,user[i].ip);
		fprintf(stdout, "User Port %s ",user[i].port);	
		printf("Authorized: accept\n");
		if (send(s_c, "Accepted#",strlen("Accepted#"),0)==-1){
			perror("server error : send");
			close(s_c);
			exit(-1);
		}
	}
	

SYNC:
	fprintf(stdout, "I am waiting for barrier sync\n");
	pthread_barrier_wait(&barr);
	if (send(s_c, "Ready",strlen("Ready#"),0)==-1) {
		perror("server error : send");
		close(s_c);
		exit(-1);
	} 

	close(s_c);
	fprintf(stdout, "phase1 for this user%d complete\n-----------------------------\n",i);
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


void phase2_handle_connect(int phase2_s_s) 
{ 
        int s_c; 
        struct sockaddr_in from; 
        socklen_t len = sizeof(from); 
        pthread_t thread_do[6]; 
		int phase2_thread_number=0;
		int phase2_thread_max_number=0;
		int i=0;
		void *status[2];

		for(i=0;i<10;i++){
			if(user[i].type[0]=='2'&&user[i].authentication_success==1)
				phase2_thread_max_number++;
		}

        while (1) 
        { 
                s_c = accept(phase2_s_s, (struct sockaddr *)&from, &len); 
                if (s_c > 0) 
                {
					pthread_create(&thread_do[phase2_thread_number], NULL, phase2_handle_request, (void *) &s_c);
					fprintf(stdout, "server : thread %d created\n",(int)thread_do);
					phase2_thread_number++;
                }
				if(phase2_thread_number==phase2_thread_max_number){

				for(i=0;i<phase2_thread_max_number;i++){
							pthread_join(thread_do[i], &status[i]);// **=the address of pointer 
							item_list[i]=(MyList *)status[i];
							//fprintf(stdout, "list addres %d\n",(int)item_list[i]);
						}
				printf("End of Phase 2 for Auction Server\n");
				fprintf(stdout, "--------------------------------------------\n");
				return;
				}

        } 
} 

void * phase2_handle_request(void * argv) 
{ 
	int s_c = * ((int *) argv); 
 	char buff[BUFFLEN]; 
 	int n = 0;
	char * tok;
	int i;
	int item_num=0;
	char seller_name[32];
	item_t* item=NULL;
	MyList* List=(MyList*)malloc(sizeof(MyList));
	MyListInit(List);
	// begin to receive itemlists	
	memset(buff, 0, BUFFLEN);
	n = recv(s_c, buff, BUFFLEN, 0);
	if (n > 0) 
 		fprintf(stdout, "server : message from thread %d, %s \n",(int)pthread_self(),buff);
	else {
		fprintf(stderr, "client disconnected, socket close thread exit\n");
		close(s_c);
		pthread_exit(NULL);
	}

	tok = strtok(buff,"#");
	if(strcmp(tok,"Items")!=0){
		fprintf(stderr, "invalid Items command\n");
		pthread_exit(NULL);	
	}
	tok = strtok(NULL, "# \n");
	item_num=atoi(tok);  // get items number
	tok = strtok(NULL, "# \n");
	strcpy(seller_name,tok); // get seller name
	

	for(i=0;i<item_num;i++){
		item=(item_t*)malloc(sizeof(item_t));
		tok = strtok(NULL, "# \n");
		strcpy(item->item_name,tok); // get item name
		tok = strtok(NULL, "# \n");
		item->price=atoi(tok);  // get item price
		strcpy(item->seller_name,seller_name); //get seller name
		MyListAppend(List, (void*)item);
	}
	
	fprintf(stdout, "list addres %d, get items from seller %s, he/she has %d items\n",(int)List, ((item_t *)(MyListFirst(List)->obj))->seller_name,MyListLength(List));
	fprintf(stdout, "phase2 for this user complete\n-----------------------------\n");
	close(s_c);
	
 	return (void *)List;  
}


