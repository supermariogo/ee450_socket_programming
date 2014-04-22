#include "port.h"
#include "bidder.h"

user_data_t self_info;

item_t *item_array; //for boradcast.txt
int item_num=0;
//char broadcast_file[4096];  buff-num#

item_t *item_bid_array; //for bidding.txt
int item_bid_num=0;
char bid_file[4096];


int main(int argc, char *argv[])
{
	file_read_self_info(1, BIDDERX, &self_info);
	phase1_processing(1, BIDDERX, &self_info);

	//self_info.authentication_success=BIDDERX;
	//strcpy(self_info.name,"NAMENAME");

	phase3_read_bidding_file(BIDDERX,bid_file);
	fprintf(stdout, "bid file has read to to memory:%s\n",bid_file );
	
	item_bid_array=phase3_file_to_list(bid_file, item_bid_num);
	fprintf(stdout, "bid file has copied to bid_array, begin UDP\n");

	/*
	 *get messsage form client
	 *stroke the message and store it to broadcast_file
	 *broadcat to item_array list
	 *compare item_array and
	 *send name#10#30#40#
	 */
	phase3_bid();

	if(self_info.authentication_success!=1)
		printf("I am not a authorised user. END\n");

	return 0;
}


void phase3_read_bidding_file(int X, char * bid_file)
{
// read file to file_content
	FILE * fp;
	long file_size;
	int line_number=0;
	char current_line[1024];
	if(X==1)
		fp = fopen ("bidding1.txt","r"); 
	else
		fp = fopen ("bidding2.txt","r");

	while (fgets(current_line, 1024, fp)!=NULL ){
		line_number++;
	}
	item_bid_num=line_number;
	file_size = ftell(fp);
	rewind(fp);
	fread(bid_file,sizeof(char),file_size,fp);
	fclose(fp);
	
}


void phase3_bid(void)
{
	// get messsage form client
	// stroke the message and store it to broadcast_file
	// broadcat to item_array list
	// compare item_array and
	// send name#10#30#40

	struct sockaddr_in myaddr;      /* our address */
	struct sockaddr_in remaddr;     /* remote address */
	socklen_t addrlen = sizeof(remaddr);            /* length of addresses */
	int fd;                         /* our socket */
	char buff[4096];       /* receive bufffer */
	
	/* create a UDP socket */
	
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
	        perror("cannot create socket\n");
	        exit(-1);
	}
	
	/* bind the socket to SERVERHOST */
	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	inet_pton(AF_INET, SERVERHOST, &myaddr.sin_addr);
	if(BIDDERX==1)
		myaddr.sin_port = htons(BIDDER1_PHASE3_PORT);
	else if(BIDDERX==2)
		myaddr.sin_port = htons(BIDDER2_PHASE3_PORT);
	else{
		fprintf(stderr, "bidder what?!\n");
		exit(1);
	}
	
	if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
	fprintf(stderr, "I am bidder%d, I cannot bind for UDP\n", BIDDERX);
	perror("bind failed");
	exit(1);
	}
	
	/* 1. now loop, receiving data and printing what we received */
	fprintf(stdout, "waitting for recvfrom().................\n");
	int recvlen = recvfrom(fd, buff, BUFFLEN, 0, (struct sockaddr *)&remaddr, &addrlen);
	if(recvlen<=0){
		fprintf(stderr,"recvfrom error\n");
		exit(-1);
	}else
		fprintf(stdout, "received:%s\n", buff);	
	
	// 2. stroke the message and store it to broadcast list(item_array)
	fprintf(stdout, "beging to stroke message and store it to item_array\n");
	char * tok;
	tok=strtok(buff,"#\n ");
	item_num=atoi(tok);
	item_array= phase3_file_to_list(tok+strlen(tok)+1,item_num);

	char message[1024];
	// 3. compare array and construct message name#10#30#40# 
	compare_and_bid(message);
	// 4. send message
	if(self_info.authentication_success!=1)
		sendto(fd, "failed bidder#", strlen("failed bidder#"), 0, (struct sockaddr *)&remaddr, addrlen);
	else{
		if(sendto(fd, message, strlen(message), 0, (struct sockaddr *)&remaddr, addrlen)<0){
			perror("sendto error : connect");
			exit(-1);
		}else
			fprintf(stdout,"send complete\nwating for annoucement-------------------------\n");
	}
}

void compare_and_bid(char * message)
{	
	int i=0;
	int j=0;
	sprintf(message,"%s#",self_info.name);
	for(j=0;j<item_num;j++){
		for(i=0;i<item_bid_num;i++)
		{
			if(strcmp(item_array[j].item_name,item_bid_array[i].item_name)==0
					&&
			   strcmp(item_array[j].seller_name,item_bid_array[i].seller_name)==0)
			{
				item_array[j].bidder_price[0]=item_bid_array[i].price;
				fprintf(stdout,"match!, item_name=%s, our price=%d\n",item_array[j].item_name,item_array[j].bidder_price[0]);
				break;
			}
		}
		sprintf(message,"%s%d#",message,item_array[j].bidder_price[0]);
			//bidder_price[0] is only used for store and output, not ture meaning
	}
	fprintf(stdout, "the message going to send is:%s\n",message); } 

