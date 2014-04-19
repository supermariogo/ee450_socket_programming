
extern int phase1_login_check(char *buff);
extern void file_read_reg(void);

static void handle_connect(int s_s);
static void * handle_request(void * argv);
extern int get_peer_ip_or_port(int sockfd, char *dest, int type);
