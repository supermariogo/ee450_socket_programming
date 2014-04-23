extern void file_read_reg(void);
extern void phase1_handle_connect(int);
extern int phase1_login_check(char *buff, user_data_t * un_auth_user);
extern void phase1_handle_connect(int s_s);
extern void * phase1_handle_request(void * argv);
extern void phase2_handle_connect(int s_s);
extern void * phase2_handle_request(void * argv);

extern void phase3_read_broadcast_file(char *file_content);
extern void phase3_to_bidder(char *file_content); //and get reply from bidder
extern void compare_and_bid(char *);
extern int get_buyer(item_t item);
extern void phase3_calculate(void);
extern void phase3_announce(int PORT);
extern int name_to_num(int type, char *name);

