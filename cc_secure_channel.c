#include "cc_basic.h"
#include <sys/time.h>

static void
cc_secure_channel_read(sw_info *cc_sw_info)
{
	int ret;
	if(recv_from_secure_channel(cc_sw_info) < 0)
	{
		log_err_for_cc();
	}

	if(cc_sw_info->recv_queue->length > 0)
	{
		ret = cc_handler_message_from_secure_channel(cc_sw_info);
		if(ret < 0)
		{
		
		}
	}
}

/*send to secure channel through socket*/
static void
cc_secure_channel_write(sw_info *cc_sw_info)
{
	int ret;
	
	if(cc_sw_info->send_queue->length > 0)
	{
		if(cc_send_to_secure_channel(cc_sw_info) < 0)
		{
			log_err_for_cc();
			return CC_ERROR;
		}
	}else{
		return CC_ERROR;
	}

	return CC_SUCCESS
}


/*get the msg to be sent form the queue*/
int
cc_get_out_queue(sw_info *cc_sw_info)
{
	if( cc_sw_info == NULL && cc_sw_info->send_queue == NULL && \
		cc_sw_info->secure_channel_fd < 0){

		log_err_for_cc();
		return CC_ERROR;
	}

	buffer* cc_buf;
	ssize_t cc_write_len;

	pthread_rwlock_wrlock(&cc_sw_info->send_queue->queue_lock,NULL);
	
	if( sw_info->send_queue->length == 0 ){
		return CC_ERROR;
	}

	
		
	
	/****to be continue******/
	pthread_rwlock_unlock(&cc_sw_info->send_queue->queue_lock);
}
	


static void
cc_time_out_event(sw_info *cc_sw_info)
{
	switch(cc_sw_info->state){
		case CC_WAIT_HELLO:
			cc_handle_timeout(CC_TIMEOUT_FOR_HELLO,cc_sw_info);
			
		case CC_WAIT_ECHO_REPLY:
			cc_handle_timeout(CC_TIMEOUT_FOR_ECHO_REPLY,cc_sw_info);
	}
				
}	

static void
cc_set_timer(int sec,sw_info* cc_sw_info)
{
	struct itimespec interval;

	interval.it_value.tv_sec = sec;
	interval.it_value.tv_nsec = 0;
  	interval.it_interval.tv_sec = 0;
  	interval.it_interval.tv_nsec = 0;
	cc_time_out_event(cc_sw_info);
}
		
static void
cc_handle_timeout()
{
}



