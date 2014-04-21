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

	fprintf(stdout, "Hello, I am type%d(1bidder 2seller) #%d\n",type, X);

	s_c = socket(AF_INET, SOCK_STREAM, 0); //create socket fd
    if (connect(s_c, (struct sockaddr *) &phase1_addr_info, sizeof(phase1_addr_info)) == -1) {
        close(s_c);
        perror("client error : connect");
		exit(-1);
    }
	get_my_ip_or_port(s_c,self_info->ip,1);
	get_my_ip_or_port(s_c,self_info->port,2);

	
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

	tok = strtok(buff,"#");
	if(strcmp("Accepted",tok)!=0)
		fprintf(stdout,"I was Rejected--------------------\n");
	else
		fprintf(stdout,"I was Accpted---------------------\n");

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
	
	fprintf(stdout, "Phase1 complete!\nI will sleep for 1s to let server create next socket----------------------\n");
	sleep(1);
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

		fprintf(stdout, "user.type=%s\n",self_info->type);	
		fprintf(stdout, "user.name=%s\n",self_info->name);
		fprintf(stdout, "user.password=%s\n",self_info->password);
		fprintf(stdout, "user.account=%s\n",self_info->account);
		
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
    	fprintf(stdout, "my ip: %s\n", dest);
	}
	else if(type==2){
		sprintf(dest, "%u", ntohs(my_addr_info.sin_port));
		fprintf(stdout, "my port: %s\n", dest);  // it doesn't matter use ntoh or hton, the result is same!!!
	}
	else{
		fprintf(stderr, "wrong type for peer ip or port\n");
		exit(-1);	
	}
	return 1;
}


void phase3_file_to_list(char * file_content, int num, item_t * array)
{
	char * tok=NULL;
	int i=0;
	array=(item_t*)malloc(sizeof(item_t)*num);
	fprintf(stderr,"array address:%d\n",array);
	memset((char*)&array, 0, sizeof(item_t)*num);
	fprintf(stdout, "the file going to strtok is:%s\n",file_content);

	for(i=0;i<num;i++){
		
		if(i==0)
			tok = strtok(file_content, " \n");
		else
			tok = strtok(NULL, " \n");
		strcpy( array->seller_name, tok);
fprintf(stdout, "tok is:%s\n sizeof(array)=%d\n sellername=%d\n",tok, sizeof(array),array->seller_name);

		strcpy(array[i].seller_name,tok); // get seller name
fprintf(stdout, "here ?tok is:%s\n",tok);

		tok = strtok(NULL, " \n");
fprintf(stdout, "tok is:%s\n",tok);
		strcpy(array[i].item_name,tok); // get item name

		tok = strtok(NULL, " \n");
fprintf(stdout, "tok is:%s\n",tok);
		array[i].price=atoi(tok);  // get item price
	}

}

