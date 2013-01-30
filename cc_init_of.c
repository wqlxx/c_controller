#include "cc_basic.h"

/*FUNC:cc_init_of.c need to 
*(1)init the 'cc_sw_queue'
*(2)init the socket and return a fd in the struct sw_queue
*(3)log system (in plan /blue print)
*(4)to be continue
*
*return the cc_sw_queue and err or success msg code
*/

int
cc_init_of(struct cc_socket* cc_socket)
{
	int ret;
	ret = cc_conn_init(&cc_socket);
	if(ret < 0 )
	{
		printf("ERR|conn error!");
		return CC_ERR;
	}
	while(1)
	{
		cc_conn_accept(cc_sw_queue);
	}
	
}

	