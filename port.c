// these are shared functions
#include "port.h"

void phase1_processing(int type, int X, user_data_t * self_info)
{
	int s_c; //socket fd 
	struct sockaddr_in phase1_addr_info; // connect info
	char buff[BUFFLEN]="";
	int n;
	char *tok = NULL;

	memset(&phase1_addr_info, 0, sizeof(phase1_addr_info)); 
    phase1_addr_info.sin_family = AF_INET;
	inet_pton(AF_INET,SERVERHOST, &phase1_addr_info.sin_addr);
    phase1_addr_info.sin_port = htons(SERVER_PHASE1_PORT); //from host byte order to network byte order.  

	//fprintf(stdout, "Hello, I am type%d(1bidder 2seller) #%d\n",type, X);

	s_c = socket(AF_INET, SOCK_STREAM, 0); //create socket fd
    if (connect(s_c, (struct sockaddr *) &phase1_addr_info, sizeof(phase1_addr_info)) == -1) {
        close(s_c);
        perror("client error : connect");
		exit(-1);
    }
	get_my_ip_or_port(s_c,self_info->ip,1);
	get_my_ip_or_port(s_c,self_info->port,2);
	if(type==1)
		printf("Phase 1: <Bidder%d> has TCP port %s and IP address:%s\n",X, self_info->port,self_info->ip);
	else
		printf("Phase 1: <Seller%d> has TCP port %s and IP address:%s\n",X, self_info->port,self_info->ip);

	if (send(s_c, self_info->command,strlen(self_info->command),0)==-1)
    	perror("client error : send");

	n = recv(s_c, buff, BUFFLEN, 0);
	if (n > 0) 
		fprintf(stdout, "client : message from server %s\n",buff);
	else {
		fprintf(stderr, "client : server disconnected, socket close thread exit\n");
		close(s_c);
		exit(0);
	}
	printf("Phase 1: Login request. User: %s, password: %s, Bank account: %s\n", self_info->name,self_info->password,self_info->account);
	tok = strtok(buff,"#");
	if(strcmp("Accepted",tok)!=0){
		printf("Phase 1: Login request reply: Rejected\n");
		self_info->authentication_success=0;
	}
	else{
		printf("Phase 1: Login request reply: Accepted\n");
		if(type==2)//only for seller
			printf("Phase 1: Auction Server has IP Address: %s and PreAuction TCP Port Number: %d \n", NUNKI,SERVER_PHASE2_PORT);
		
		self_info->authentication_success=1;
	}

	//upon receive "Ready" to sync 4 threads  ask "Ready?" 
	if (send(s_c, "Ready?",strlen("Ready?"),0)==-1)
		perror("client error : send");

	memset(buff, 0, BUFFLEN);
	fprintf(stdout, "I am waiting for Ready signal to continue\n");
	n = recv(s_c, buff, BUFFLEN, 0);
	if (n > 0) 
		fprintf(stdout, "Get ready signal:%s\n",buff);
	else {
		fprintf(stderr, "server close connection! No ready signal!\n");
		close(s_c);
		exit(-1);
	}

	if(strcmp(buff,"Ready#")!=0){
		fprintf(stderr, "invalid Ready command\n");
		exit(-1);	
	}

	/*
	 *if(type==1)
	 *    printf("End of Phase 1 for <Bidder%d>.\n", X);
	 *else 
	 */
	if(type==2)
	{
		printf("End of Phase 1 for <Seller%d>. \n",X);
		sleep(1);
	}
	close(s_c);

}
void file_read_self_info(int type, int X, user_data_t * self_info)
{
	FILE * fp;
	char current_line[1024];
	char *tok = NULL;
	if(type==1){
		if(X==1)
			fp = fopen ("bidderPass1.txt","r"); 
		else
			fp = fopen ("bidderPass2.txt","r"); 
	}
	else{
		if(X==1)
			fp = fopen ("sellerPass1.txt","r"); 
		else
			fp = fopen ("sellerPass2.txt","r");	
	}
	if (fgets(current_line, 1024, fp)!=NULL ){

		fprintf(stdout, "string before strtok(): %s\n", current_line);
		tok = strtok(current_line," ");
		strcpy(self_info->type,tok);	
		tok = strtok(NULL," ");	
		strcpy(self_info->name,tok);
		tok = strtok(NULL, " ");
		strcpy(self_info->password,tok);
		tok = strtok(NULL, " \n");
		strcpy(self_info->account,tok);
		/*
		 *
		 *    fprintf(stdout, "user.type=%s\n",self_info->type);	
		 *    fprintf(stdout, "user.name=%s\n",self_info->name);
		 *    fprintf(stdout, "user.password=%s\n",self_info->password);
		 *    fprintf(stdout, "user.account=%s\n",self_info->account);
		 *
		 */
	}
	fclose(fp);

	sprintf (self_info->command, "Login#%s %s %s %s\n", self_info->type, self_info->name, self_info->password, self_info->account);
	fprintf(stdout, "command: %s\n",self_info->command);

}

int get_peer_ip_or_port(int sockfd, char *dest, int type) 
{
	struct sockaddr_in peer_addr_info;
	socklen_t len;
	len = sizeof(peer_addr_info);
	getpeername(sockfd, (struct sockaddr *)&peer_addr_info, &len);   //get ip and port info

	if(type==1)
		inet_ntop(AF_INET, &peer_addr_info.sin_addr, dest, 17);
	else if(type==2)
		sprintf(dest, "%u", ntohs(peer_addr_info.sin_port));       // from network byte order to host byte order.
	else{
		fprintf(stderr, "wrong type for peer ip or port\n");
		exit(-1);
		}
	return 1;
}

int get_my_ip_or_port(int s_fd, char *dest, int type)
{
    struct sockaddr_in my_addr_info;
    socklen_t len = sizeof(my_addr_info);
    getsockname(s_fd, (struct sockaddr *)&my_addr_info, &len);
	if(type==1){
		inet_ntop(AF_INET, &my_addr_info.sin_addr, dest, 17);
    	//fprintf(stdout, "my ip: %s\n", dest);
	}
	else if(type==2){
		sprintf(dest, "%u", ntohs(my_addr_info.sin_port));
		//fprintf(stdout, "my port: %s\n", dest);  // it doesn't matter use ntoh or hton, the result is same!!!
	}
	else{
		fprintf(stderr, "wrong type for peer ip or port\n");
		exit(-1);	
	}
	return 1;
}


item_t * phase3_file_to_list(char * file_content, int num)
{
	char * tok=NULL;
	int i=0;
	item_t * array=(item_t*)malloc(num*sizeof(item_t));
	memset((void *)array, 0, num*sizeof(item_t));

	//fprintf(stdout, "the file going to strtok is:\n%s\n",file_content);

	for(i=0;i<num;i++){
		
		if(i==0)
			tok = strtok(file_content, " \n#");
		else
			tok = strtok(NULL, " \n#");
		strcpy(array[i].seller_name,tok); // get seller name

		tok = strtok(NULL, " \n#");
		strcpy(array[i].item_name,tok); // get item name

		tok = strtok(NULL, " \n#");
		array[i].price=atoi(tok);  // get item price
	}
	return array;
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
	fprintf(stdout, "AS server, I begin to listen\n");
	if(PORT==SERVER_PHASE1_PORT)
		printf("Phase 1: Auction server has TCP port number %d and IP address %s\n", SERVER_PHASE1_PORT, NUNKI);
	else if(PORT==SERVER_PHASE2_PORT)
		printf("Phase 2: Auction Server IP Address: %s PreAuction TCP Port Number: %d\n", NUNKI, SERVER_PHASE2_PORT);
	
	return s_s;

}


void listen_result(int type, int X, char *user_name)
{
	int s_s;
	int s_c; 
	struct sockaddr_in from;
	char buff[8192];
	int PORT;
	int n;
	socklen_t len = sizeof(from); 
	if(type==1){
		if(X==1)
			PORT= BIDDER1_FINAL_PORT;
		else
			PORT= BIDDER2_FINAL_PORT; 
	}
	else{
		if(X==1)
			PORT= SELLER1_FINAL_PORT; 
		else
			PORT= SELLER2_FINAL_PORT;	
	}
	fprintf(stdout, "listen port %d\n",PORT);
	s_s=socket_bind_listen(PORT);
	s_c = accept(s_s, (struct sockaddr *)&from, &len); 	
	
	sprintf(buff,"%s#",user_name);
	if (send(s_c, buff,strlen(buff),0)==-1)
		perror("failed to send my name");
	else
		fprintf(stdout, "send my name: %s, and begin to recv my info\n",buff);
	
	memset(buff,0,8192);
	n = recv(s_c, buff, 8192, 0);
	if (n > 0) 
		printf("%s\n",buff);
	else if(n==0){
		printf("NO INFORMATION FOR ME, I AM FAILED BIDDER\n");
	}
	else {
		fprintf(stderr, "can't receive infomation of me \n");
		close(s_c);
		exit(0);
	}
	close(s_c);
	close(s_s);

	if(type==1)
		printf("End of Phase 3 for <Bidder%d>.\n", X);
	else
		printf("End of Phase 3 for <Seller%d>.\n", X);
}
