#include "cc_basic.h"

/*FUNC:cc_init_of.c need to 
*(1)init the 'cc_sw_queue'
*(2)init the socket and return a fd in the struct sw_queue
*(3)log system (in plan /blue print)
*(4)to be continue
*
*return the cc_sw_queue and err or success msg code
*/

static switch_table cc_switch_table;

static void
_cc_init_event_handler(sw_info* cc_sw_info,int fd,event_handler_callback read_callback,void* read_data,
						event_handler_callback write_callback,void* write_data)
					
{
	cc_sw_info->eh->read_buf = read_data;
	cc_sw_info->eh->write_buf = write_data;
	cc_sw_info->eh->read_handler = read_callback;
	cc_sw_info->eh->write_handler = write_callback;
}
void (*cc_init_evnet_handler)(sw_info* cc_sw_info,int fd,event_handler_callback read_callback,void* read_data,
						event_handler_callback write_callback,void* write_data) = _cc_init_event_handler;
/*
static void
_cc_start_event_handler(sw_info* cc_sw_info)
{
	int ret;
	while(1)
	{
		ret = cc_secure_channel_read(cc_sw_info);
		if( ret == CC_ERROR )
		{
			log_err_for_cc("read from secure channle error !");
			continue;
		}
	}
		
		
}
void (*cc_start_event_handler)(sw_info* cc_sw_info) = _cc_start_event_handler;
*/

static int
cc_insert_sw_info(sw_info* cc_sw_info)
{
	if(cc_switch_table->head == NULL)
	{
		cc_switch_table->head = (each_sw*)malloc(sizeof(each_sw));
		if( cc_switch_table->head == NULL )
		{
			log_err_for_cc("create switch table's head failed in %s",__func__);
			return CC_ERROR;
		}
	}
	each_sw* tmp = cc_switch_table->head->next;
	/*to find the end of list*/
	while(tmp != NULL)
	{	
		tmp = tmp->next;
	}
	tmp->next = cc_sw_info->cc_switch;
	return CC_SUCCESS;
}

sw_info*
cc_recv_conn_from_switch(struct cc_socket* listen_sock)
{
	sw_info* cc_sw_info = NULL;
	int ret,_ret;
	ret = cc_conn_accept(cc_sw_info);
	/*here whether the accept is success or unsuccess ,return to the main process
	 *and register the information of switch include dpid 
	 */
	if( ret != CC_ERROR && cc_sw_info != NULL )
	{
		_ret = cc_insert_sw_info(cc_sw_info);
		if( _ret == CC_ERROR )
		{
			log_err_for_cc("insert sw info failed");
			return CC_ERROR;
		}
	}else{
		
	}
}


static int
cc_init_of(struct cc_socket* listen_socket)
{
	int ret;

	while(1)
	{
		ret = cc_recv_conn_from_switch(cc_socket);
		if( ret == CC_CONN_SUCCESS )
			break;
		else
			continue;
	}
	cc_init_event_handler(cc_sw_info->);
	//cc_start_event_handler();
	
}


static void
cc_app_init(const char** app_name,int num)
{
	int ret;
	int i;
	
	for(i = 0; i < num; i++)
	{
		ret = cc_app_register(*app_name);
		if(ret < 0)
		{
			log_err_for_cc("register app error!");
		}
	}
}


void
cc_app_run(struct cc_app* cc_app)
{
	
}

