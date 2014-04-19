#include "port.h"
#include "bidder.h"

user_data_t self_info;
int main(int argc, char *argv[])
{
	file_read_self_info(2, SELLERX, &self_info);
	phase1_processing(2, SELLERX, &self_info);

	return 0;
}
