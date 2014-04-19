// these are shared functions
#include "port.h"
extern user_data_t self_info;
extern char next_phase_ip[17];
extern char next_phase_port[5];
void phase1_processing(int type, int X)
{
	int phase1_sfd_c; //socket fd 
	struct sockaddr_in phase1_addr_info; // connect info
	char buff[BUFFLEN]="";
	int n;
	char *tok = NULL;

	memset(&phase1_addr_info, 0, sizeof(phase1_addr_info)); 
    phase1_addr_info.sin_family = AF_INET;
	inet_pton(AF_INET,SERVERHOST, &phase1_addr_info.sin_addr);
    //phase1_addr_info.sin_addr.s_addr = htonl(INADDR_ANY); //use local address 
    phase1_addr_info.sin_port = htons(SERVER_PHASE1_PORT); 

	fprintf(stdout, "Hello, I am type%d(1bidder 2seller) #%d\n",type, X);

	phase1_sfd_c = socket(AF_INET, SOCK_STREAM, 0); //create socket fd
    if (connect(phase1_sfd_c, (struct sockaddr *) &phase1_addr_info, sizeof(phase1_addr_info)) == -1) {
        close(phase1_sfd_c);
        perror("client error : connect");
		exit(-1);
    }	
	
	if (send(phase1_sfd_c, self_info.command,strlen(self_info.command),0)==-1)
    	perror("client error : send");

	n = recv(phase1_sfd_c, buff, BUFFLEN, 0);
	if (n > 0) 
		fprintf(stdout, "client : message from server %s\n",buff);
	else {
		fprintf(stderr, "client : server disconnected, socket close thread exit\n");
		close(phase1_sfd_c);
		exit(0);
	}

	tok = strtok(buff,"#");
	if(strcmp("Accepted",tok)!=0){
		fprintf(stdout,"I am not Accepted, close sockfd and exit");
		close(phase1_sfd_c);
		exit(0);
	}
	if(type==2){
		//seller needs this info
		tok = strtok(NULL, "#");
		strcpy(next_phase_ip,tok);
		tok = strtok(NULL, "#");
		strcpy(next_phase_port,tok);

		fprintf(stdout, "next_phase_ip=%s\n",next_phase_ip);
		fprintf(stdout, "next_phase_port=%s\n",next_phase_port);	
	}
	close(phase1_sfd_c);
	fprintf(stdout, "I was accpted, begin phase2?\n");
	fprintf(stdout, "----------------------------------------\n");
}
void file_read_self_info(int type, int X)
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

int get_peer_ip_or_port(int sockfd, char *dest, int type) 
{
	struct sockaddr_in peer_addr_info;
	socklen_t len;
	len = sizeof(peer_addr_info);
	getpeername(sockfd, (struct sockaddr *)&peer_addr_info, &len);   //get ip and port info
	if(type==1)
		strcpy(dest,inet_ntoa(peer_addr_info.sin_addr)); //inet_ntoa return char*
	else if(type==2)
		sprintf(dest, "%u", htons(peer_addr_info.sin_port));       //htons因为网络传送是低位先传送
	else{
		fprintf(stderr, "wrong type for peer ip or port\n");
		exit(-1);
		}
	return 1;
}

int get_my_ip(int server_fd, char *dest)
{
    struct sockaddr_in server_info;
    int namelen = sizeof(server_info);
    getsockname(server_fd, (struct sockaddr *)&server_info, (socklen_t *)&namelen);
	sprintf(dest,"%s",inet_ntoa(server_info.sin_addr));
    printf("server ip: %s\n", inet_ntoa(server_info.sin_addr));
	//printf("port: %d\n", ntohs(server.sin_port));
	return 1;
}
