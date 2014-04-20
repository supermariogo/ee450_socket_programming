extern void file_read_reg(void);
extern int socket_bind_listen(uint16_t PORT);	
extern void phase1_handle_connect(int);
extern int phase1_login_check(char *buff, user_data_t * un_auth_user);
extern void phase1_handle_connect(int s_s);
extern void * phase1_handle_request(void * argv);
extern void phase2_handle_connect(int s_s);
extern void * phase2_handle_request(void * argv);
