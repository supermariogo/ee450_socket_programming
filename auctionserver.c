#include "port.h"
#include "MyList.h"
#include "auctionserver.h"
user_data_t user[10];
int user_number=0; // count from zero, valid user, success = 1
int phase1_thread_max_number=0;
int phase2_thread_max_number=0; //seller number
MyList * item_list[2];

item_t *item_array;
int item_num=0;

pthread_barrier_t barr;

int main(int argc, char * argv[]) 
{ 

	memset(&user, 0, sizeof(user));
	file_read_reg();

	int phase1_s_s;
	phase1_s_s=socket_bind_listen(SERVER_PHASE1_PORT);	
	phase1_handle_connect(phase1_s_s); 
	close(phase1_s_s);

	int phase2_s_s;
	phase2_s_s=socket_bind_listen(SERVER_PHASE2_PORT);
	phase2_handle_connect(phase2_s_s);
	close(phase2_s_s);
	
	//phase 3
	sleep(2);
	char file_content[4096];
	char file_content2[4096];
	//read boradcast.txt to file_content
	phase3_read_broadcast_file(file_content); 
	strcpy(file_content2, file_content);//back up to file_content2
	//malloc for item arrary; destory file_content; store it to item_array
	item_array=phase3_file_to_list(file_content,item_num);
	//send broadcastList and get reply and store bidder's info	
	phase3_to_bidder(file_content2); 
	//higher price bidder to item.buyer_name item.buyer_price	
	phase3_calculate();

	sleep(2);// to give time for them create socket and listen
	phase3_announce(SELLER1_FINAL_PORT);
	phase3_announce(SELLER2_FINAL_PORT);
	phase3_announce(BIDDER1_FINAL_PORT);
	phase3_announce(BIDDER2_FINAL_PORT);

	printf("End of Phase 3 for Auction Server.\n");

	return 0; 
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
				//fprintf(stdout, "server : thread %d created\n",(int)thread_do);
				phase1_thread_max_number++;
	        }
			if(phase1_thread_max_number==4){
				pthread_join(thread_do[0], NULL);	
				pthread_join(thread_do[1], NULL);
				pthread_join(thread_do[2], NULL);
				pthread_join(thread_do[3], NULL);
				printf("End of Phase 1 for Auction Server\n");
				//fprintf(stdout, "--------------------------------------------\n");
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
	if (n > 0){ 
 		//fprintf(stdout, "server : message from thread %d, %s \n",(int)pthread_self(),buff);
	}
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
		//fprintf(stdout, "User Port %s ",un_auth_user.port);
		printf("Authorized: reject\n");
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
		//fprintf(stdout, "User Port %s ",user[i].port);	
		printf("Authorized: accept\n");
		if (send(s_c, "Accepted#",strlen("Accepted#"),0)==-1){
			perror("server error : send");
			close(s_c);
			exit(-1);
		}
		char myip[17];
		if(user[i].type[0]=='2'){
			get_my_ip_or_port(s_c,myip,1);
			//fprintf(stdout,"My type is %c",user[i].type[0]);
			printf("Phase 1: Auction Server IP Address: %s PreAuction Port Number: %d sent to seller\n",myip,SERVER_PHASE2_PORT);
		}
	}
	

SYNC:
	//fprintf(stdout, "I am waiting for barrier sync\n");
	pthread_barrier_wait(&barr);
	memset(buff, 0, BUFFLEN);	
	n = recv(s_c, buff, BUFFLEN, 0);// this should Ready?
	if (n <= 0){ 
		fprintf(stderr, "client disconnected, socket close thread exit\n");
		close(s_c);
		pthread_exit(NULL);
	}else{
		//fprintf(stdout, "I recived %s\n",buff);	
	}// this should "Ready?" 

	if (send(s_c, "Ready#",strlen("Ready#"),0)==-1){
		perror("server error : send");
		close(s_c);
		exit(-1);
	}
	//fprintf(stdout, "I have send Ready signal\n");
	close(s_c);
	//fprintf(stdout, "phase1 for this user%d complete\n-----------------------------\n",i);
 	return NULL;
}

int phase1_login_check(char *buff, user_data_t * un_auth_user){
	user_data_t temp_user;
	char * tok;
	int i;
	//fprintf(stdout, "string before strtok(): %s\n", buff);
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
		//fprintf(stdout, "login success type=%s\n",user[i].type);
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
	//fprintf(stdout,"----------Loading Registration file-------------------\n");
	while (fgets(current_line, 1024, fp)!=NULL ){
		if(strlen(current_line)<2) //<2 invalid
			continue;
		tok = strtok(current_line," ");
		strcpy(user[i].name,tok);
		tok = strtok(NULL, " ");
		strcpy(user[i].password,tok);
		tok = strtok(NULL, " \n");
		strcpy(user[i].account,tok);

		/*
		 *fprintf(stdout, "name=%s ",user[i].name);
		 *fprintf(stdout, "password=%s ",user[i].password);
		 *fprintf(stdout, "account=%s \n",user[i].account);
		 */
		
		i++;
	}
	user_number=i;// count from zero
	//fprintf(stdout,"----------%d users in Registration file---------------\n\n",user_number);
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
			//fprintf(stdout, "server : thread %d created\n",(int)thread_do);
			phase2_thread_number++;
		}
		if(phase2_thread_number==phase2_thread_max_number){
			for(i=0;i<phase2_thread_max_number;i++){
				pthread_join(thread_do[i], &status[i]);// **=the address of pointer 
				item_list[i]=(MyList *)status[i];
			}
			printf("End of Phase 2 for Auction Server\n");
			//fprintf(stdout, "--------------------------------------------\n");
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
	//int i;
	//int item_num=0;
	char seller_name[32];
	//item_t* item=NULL;
	/*
	 *MyList* List=(MyList*)malloc(sizeof(MyList));
	 *MyListInit(List);
	 */
	// begin to receive itemlists	
	memset(buff, 0, BUFFLEN);
	n = recv(s_c, buff, BUFFLEN, 0);
	if (n > 0){ 
 		//fprintf(stdout, "server : message from thread %d, %s \n",(int)pthread_self(),buff);
	}
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
	printf("Phase2: Seller%d send item lists\n",name_to_num(2, seller_name));
	printf("Phase2: (broadcast info)\n%s\n",tok+strlen(tok)+1);

	/*
	 *for(i=0;i<item_num;i++){
	 *    item=(item_t*)malloc(sizeof(item_t));
	 *    tok = strtok(NULL, "# \n");
	 *    strcpy(item->item_name,tok); // get item name
	 *    tok = strtok(NULL, "# \n");
	 *    item->price=atoi(tok);  // get item price
	 *    strcpy(item->seller_name,seller_name); //get seller name
	 *    MyListAppend(List, (void*)item);
	 *}
	 */
	
	//fprintf(stdout, "list addres %d, get items from seller %s, he/she has %d items\n",(int)List, ((item_t *)(MyListFirst(List)->obj))->seller_name,MyListLength(List));
	//fprintf(stdout, "phase2 for seller %s complete\n-----------------------------\n", seller_name);
	close(s_c);
 	//return (void *)List;
	return NULL;
}


void phase3_read_broadcast_file(char * file_content)
{
// read file to file_content
	FILE * fp;
	long file_size;
	int line_number=0;
	char current_line[1024];
	fp = fopen ("broadcastList.txt","r"); 
	while (fgets(current_line, 1024, fp)!=NULL ){
		if(strlen(current_line)>2) //<2 invalid
			line_number++;
	}
	item_num=line_number;
	file_size = ftell(fp);
	rewind(fp);
	fread(file_content,sizeof(char),file_size,fp);
	fclose(fp);
	
}

void phase3_to_bidder(char *file_content)
{
	int s_c;
	struct sockaddr_in servaddr;    /* server address */
	char buff[BUFFLEN];
	socklen_t addrlen;
	int recvlen;
	char *tok=NULL;
	char bidder_name[32];
	int i,j;
	addrlen=sizeof(servaddr);
	char message[4096];

	for(i=0;i<2;i++){
		printf("Phase 3: Auction Server IP Address: %s Auction UDP Port Number:%d\n",NUNKI, BIDDER1_PHASE3_PORT+i*100);
		memset((char*)&servaddr, 0, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = htons(BIDDER1_PHASE3_PORT+i*100);
		inet_pton(AF_INET, SERVERHOST, &servaddr.sin_addr);
	
		if ((s_c = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			perror("cannot create socket for bidder \n");
			return;
		}
		printf("Phase 3: (Item list displayed here)\n%s",file_content);	
		/* send a message to the bidderi */
		sprintf(message,"%d#%s",item_num,file_content);
		if (sendto(s_c,message, strlen(message), 0, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
			perror("sendto failed");
			printf("The bidder who has port %d has failed in authentication\n", BIDDER1_PHASE3_PORT+i*100);
		}
		//fprintf(stdout, "SERVER: broadcast %s\nwait recvfrom()(bidderX's price)\n",message);
		/* get reply from bidderi */
		memset((char*)buff, 0, BUFFLEN);
		recvlen = recvfrom(s_c, buff, BUFFLEN, 0, (struct sockaddr *)&servaddr, &addrlen);
		if (recvlen > 0) {
		    //printf("Phase 3: Auction Server received a bidding from Bidder%d\n%s\n", buff);
			if(strcmp(buff,"failed bidder#")==0)
				continue;
		}else{
			fprintf(stderr, "recvfrom error\n");
		}
		//  buff=name#20#30#40
		tok = strtok(buff, "#\n ");
		strcpy(bidder_name, tok);
		printf("Phase 3: Auction Server received a bidding from Bidder%d\n%s\n", name_to_num(1,bidder_name),buff);
		for(j=0;j<item_num;j++){
			tok = strtok(NULL, "#\n ");
			item_array[j].bidder_price[i]=atoi(tok);  // get bidder price
			strcpy(item_array[j].bidder_name[i],bidder_name); // get bidder_name
		}
	}
}

void phase3_calculate(void)
{	
	int i=0;
	int j=0;
	for(i=0;i<item_num;i++){
		j=get_buyer(item_array[i]);
		if (j==-1){
			//fprintf(stdout, "item#%d failed to sell\n",i);
		}
		else{
			item_array[i].buyer_price=item_array[i].bidder_price[j];
			strcpy(item_array[i].buyer_name,item_array[i].bidder_name[j]);
			//fprintf(stdout, "item#%d selled to bidder_name=%s, at price %d\n", i,item_array[i].buyer_name,item_array[i].buyer_price);
		}
		printf("Phase 3: Item %s was sold at price %d (0 means failed)\n", item_array[i].item_name,item_array[i].buyer_price);
		
	
	}
}

int get_buyer(item_t item){
	if (item.bidder_price[0]==item.bidder_price[1])
		return -1;
	else if(item.price>max(item.bidder_price[0],item.bidder_price[1]))
		return -1;
	else {
		if (item.bidder_price[0]>item.bidder_price[1])	
			return 0;
		else 
			return 1;
	}
} 

void phase3_announce(int PORT)
{
	int s_c; //socket fd 
	struct sockaddr_in addr_info; // connect info
	char buff[BUFFLEN]="";
	int n,i;
	char *tok = NULL;
	char user_name[32];
	char message[8192];
	memset(message, 0, sizeof(message));
	memset(&addr_info, 0, sizeof(addr_info)); 
    addr_info.sin_family = AF_INET;
	inet_pton(AF_INET,SERVERHOST, &addr_info.sin_addr);
    addr_info.sin_port = htons(PORT); //from host byte order to network byte order.  

	s_c = socket(AF_INET, SOCK_STREAM, 0); //create socket fd
    if (connect(s_c, (struct sockaddr *) &addr_info, sizeof(addr_info)) == -1) {
        perror("client error : connect");
		close(s_c);
		exit(-1);
    }

	n = recv(s_c, buff, BUFFLEN, 0);
	if (n > 0) {
		//fprintf(stdout, "get user_name %s\n",buff);
	}
	else {
		//fprintf(stderr, "can't get user_name, \n");
		close(s_c);
		exit(0);
	}
	tok = strtok(buff,"#");
	strcpy(user_name,tok);
	
	for(i=0;i<item_num;i++)
	{
		if(PORT<4000)  //to seller
		{
			if(strcmp(user_name, item_array[i].seller_name)==0)
				sprintf(message,"%sItem %s was sold at price %d\n",message, item_array[i].item_name,item_array[i].buyer_price);
		}
		else
		{
			if(strcmp(user_name, item_array[i].buyer_name)==0)
				sprintf(message,"%sItem %s was sold at price %d\n",message, item_array[i].item_name,item_array[i].buyer_price);
		}
	}
	//fprintf(stdout, "send to remote port %d\n%s\n------------------------------------\n",PORT,message);
	if (send(s_c, message,strlen(message),0)==-1)
		perror("client error : send");

	close(s_c);
	return;

}

int name_to_num(int type, char *name)
{	
	int j=0;
	int i=0;
	for(i=0; i<user_number; i++){
		if(user[i].type[0]-48==type)
			j++;
		if(strcmp(name, user[i].name)==0)
			return j;
		if(j>2)
			return 0;
	}
	return 0;
}
	
