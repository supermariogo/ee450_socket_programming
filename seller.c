#include "port.h"
#include "seller.h"

user_data_t self_info;
int main(int argc, char *argv[])
{
	file_read_self_info(2, SELLERX, &self_info);
	phase1_processing(2, SELLERX, &self_info);
	sleep(10);
	phase2_processing(SELLERX);

	return 0;
}


void phase2_processing(int X)
{
	FILE * fp;
	char command[4096];
	char file_content[4096];
	long file_size;
	int line_number=0;
	char current_line[1024];
		if(X==1)
			fp = fopen ("itemList1.txt","r"); 
		else
			fp = fopen ("itemList2.txt","r");

	while (fgets(current_line, 1024, fp)!=NULL ){
		line_number++;
	}

	file_size = ftell(fp);
	rewind(fp);
	fread(file_content,sizeof(char),file_size,fp);
	fclose(fp);

	sprintf(command,"Items#%d#%s",line_number-1,file_content);
	fprintf(stdout, "I am going to send:%s\n", command);

//create connection
	int s_c; //socket fd 
	struct sockaddr_in addr_info; // connect info

	memset(&addr_info, 0, sizeof(addr_info)); 
    addr_info.sin_family = AF_INET;
	inet_pton(AF_INET,SERVERHOST, &addr_info.sin_addr);
    addr_info.sin_port = htons(SERVER_PHASE2_PORT); //from host byte order to network byte order.  

	s_c = socket(AF_INET, SOCK_STREAM, 0); //create socket fd
    if (connect(s_c, (struct sockaddr *) &addr_info, sizeof(addr_info)) == -1) {
        close(s_c);
        perror("client error : connect");
		exit(-1);
    }


	if (send(s_c, command,strlen(command),0)==-1)
    	perror("client error : send");

	close(s_c);
	return;


}
