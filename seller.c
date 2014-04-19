#include "port.h"
#include "bidder.h"

user_data_t self_info;
char next_phase_ip[17];
char next_phase_port[5];
int main(int argc, char *argv[])
{
	file_read_self_info(2, SELLERX, &self_info);
	phase1_processing(2, SELLERX, &self_info);

	return 0;
}
