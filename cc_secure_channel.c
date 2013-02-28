#include "cc_secure_channel.h"

static int
cc_select_handler(sw_info* cc_sw_info,buffer* buf,uint16_t type)
{
	int ret;
	switch ( header->type ) {
  	// Immutable messages.
  		case OFPT_HELLO:
   			ret = ofpmsg_recv_hello( sw_info, buf );
   			break;
	
  		case OFPT_ERROR:
   			ret = ofpmsg_recv_error( sw_info, buf );
   			break;

  		case OFPT_ECHO_REQUEST:
    		ret = ofpmsg_recv_echorequest( sw_info, buf );
    		break;

  		case OFPT_ECHO_REPLY:
   			ret = ofpmsg_recv_echoreply( sw_info, buf );
    		break;

  		case OFPT_VENDOR:
    		ret = ofpmsg_recv_vendor( sw_info, buf );
    		break;

  		// Switch configuration messages.
  		case OFPT_FEATURES_REPLY:
    		ret = ofpmsg_recv_featuresreply( sw_info, buf );
    		break;

  		case OFPT_GET_CONFIG_REPLY:
    		ret = ofpmsg_recv_getconfigreply( sw_info, buf );
    		break;

  		// Asynchronous messages.
  		case OFPT_PACKET_IN:
    		ret = ofpmsg_recv_packetin( sw_info, buf );
    		break;

  		case OFPT_FLOW_REMOVED:
    		ret = ofpmsg_recv_flowremoved( sw_info, buf );
    		break;

  		case OFPT_PORT_STATUS:
    		ret = ofpmsg_recv_portstatus( sw_info, buf );
    		break;

  		// Statistics messages.
  		case OFPT_STATS_REPLY:
    		ret = ofpmsg_recv_statsreply( sw_info, buf );
    		break;

  		// Barrier messages.
  		case OFPT_BARRIER_REPLY:
    		ret = ofpmsg_recv_barrierreply( sw_info, buf );
    		break;

  		// Queue Configuration messages.
  		case OFPT_QUEUE_GET_CONFIG_REPLY:
    		ret = ofpmsg_recv_queue_getconfigreply( sw_info, buf );
    		break;

  		default:
    		assert( 0 );
    		break;
  }

  return ret;
}


static int
cc_ofpmsg_handle(sw_info* cc_sw_info,buffer* buf)
{
	int ret;
	struct ofp_header *header;
	uint16_t error_type,error_code;

	log_info_for_cc("get a msg from the secure channel");

	header = buf->data;
	ret = validate_openflow_message(buf);
	if ( ret != 0 ) {
    	perror( "Invalid openflow message. type:%d, errno:%d", header->type, ret );

    	error_type = OFPET_BAD_REQUEST;
    	error_code = OFPBRC_BAD_TYPE;
    	get_error_type_and_code( header->type, ret, &error_type, &error_code );
    	debug( "Validation error. type %u, errno %d, error type %u, error code %u",
           header->type, ret, error_type, error_code );

    	cc_send_err_msg( sw_info, error_type, error_code, buf );
    	free_buffer( buf );

    	return CC_ERROR;
	}

	ret = cc_select_handler(cc_sw_info,buf,header->type)
	if(ret < 0)
	{
		perror("handler ofmsg error");
		log_err_for_cc("handlerofmsg error!");
		return CC_ERROR;
	}
	
	return CC_SUCCESS;
}

/*
unsigned long
cc_read(int fd,char* buf,size_t size)
{
	ssize_t read_length = read(fd,buf,size);
	if( read_length < 0)
	{
		if( errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)
		{
			log_warning_for_cc("");
			return CC_ERROR;
		}
		perror("receive error");
		return CC_ERROR;
	}

	if( read_length == 0 )
	{
		
	}
	
}
*/

static int
cc_recv_from_secure_channel(sw_info *cc_sw_info,void* op_data)
{
	size_t remaining_length = CC_RECV_BUFFER_SIZE - cc_sw_info->recv_queue->length ;
	buffer *tmp_buff = (buffer*)malloc(struct buffer);
	if(tmp_buff == NULL)
	{
		log_debug_for_cc("create buffer failed!");
		return CC_ERROR;
	}
	
	sszie_t tmp_recv_buff_length = read(cc_sw_info->cc_switch->cc_socket->fd,tmp_buff->data,CC_BUFFER_SIZE);
	if(tmp_recv_buff_length < 0)
	{
		log_debug_for_cc("recv failed!");
		perror("recv failed!");
		if( errno == EINTER || errno == EAGAIN || errno == EWOULDBLOCK )
		{
			return CC_ERROR;
		}
		perror("recv failed!");
		//reconnect();
		return CC_ERROR;
	}

	/*input the msg in queue*/
	//enqueue_message(cc_sw_info->recv_queue,tmp_buff);
	
	size_t read_total = 0;
	while( tmp_recv_buff_length >= sizeof(struct ofp_header)) {
		struct ofp_header *header = cc_sw_info->recv_queue->tail->data;
		if ( header->version != OFP_VERSION ) {
      		error( "Receive error: invalid version (version %d)", header->version );
      		ofpmsg_send_error_msg( sw_info,OFPET_BAD_REQUEST, OFPBRC_BAD_VERSION, sw_info->fragment_buf );
      		return CC_ERROR;
   		}
		
		uint16_t message_length = ntohs( header->length );
		if( message_length > CC_RECV_BUFFER_SIZE )
		{
			perror("recv msg size is larger than buff");
			break;
		}
		/*input the msg in queue*/
		enqueue_message(cc_sw_info->recv_queue,tmp_buff);
	}	
	
	return CC_SUCCESS;
}

static int
cc_handler_message_from_secure_channel(sw_info* cc_sw_info)
{
	int ret;
	int errors = 0;
	buffer *msg;

	

	while((msg = dequeue_message( sw_info->recv_queue))
	{
		ret = cc_ofpmsg_handle(sw_info,msg);
		if(ret<0)
		{
			perror("failed to handle message to applocation.");
			errors++;
		}
	}	

	return errors == 0 ? CC_SUCCESS : CC_ERROR;
}


static int
cc_secure_channel_read(sw_info *cc_sw_info)
{
	int ret;
	if(cc_recv_from_secure_channel(cc_sw_info) < 0)
	{
		log_err_for_cc("read from secure channel failed!");
		perror("read error!");
		return CC_ERROR;
	}

	if(cc_sw_info->recv_queue->length > 0)
	{
		ret = pool_add_worker(cc_handler_message_from_secure_channel,cc_sw_info);
		if( ret == CC_ERROR )
		{
			log_err_for_cc("Use thread in thread pool failed");
			return CC_ERROR;
		}
	}
		/*
		ret = cc_handler_message_from_secure_channel(cc_sw_info);
		if(ret < 0)
		{
			log_err_for_cc("handle message error!");
			perror("handle msg error!");
			return CC_ERROR;
		}
		*/
	}
	
	return CC_SUCCESS;
}


static int
cc_append_to_writev_args( buffer *message, void *user_data ) {
  cc_writev_args *args = user_data;

  args->iov[ args->iovcnt ].iov_base = message->data;
  args->iov[ args->iovcnt ].iov_len = message->length;
  args->iovcnt++;

  if ( args->iovcnt >= IOV_MAX ) {
    return CC_ERROR;
  }

  return CC_SUCCESS;
}



static int
cc_send_to_secure_channel(sw_info* cc_sw_info)
{
	int ret;
	buffer* msg;

	while(( msg = peek_message(cc_sw_info->recv_queue) )!= NULL)
	{
		ssize_t write_length = write( cc_sw_info->cc_switch->cc_socket->fd,msg,sizeof(msg));
		if( write_length < 0 )
		{
			if ( errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK ) 
			{
				
				return CC_ERROR;
			}
			perror("fail to write a message to secure channel");
			return CC_ERROR;
		}else if( (size_t)write_length > 0 && (size_t)write_length < msg->length ) {
			log_err_for_cc("write msg to secure channel error!");
			//write( cc_sw_info->cc_switch->cc_socket->fd,msg,sizeof(msg));
			continue;
		}
		buffer* buf = dequeue_message(message_queue * queue);
		free_buffer(buf);
	}
}


/*send to secure channel through socket*/
static int
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

	return CC_SUCCESS;
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

	/*
	pthread_rwlock_wrlock(&cc_sw_info->send_queue->queue_lock,NULL);
	
	if( sw_info->send_queue->length == 0 ){
		return CC_ERROR;
	}
	*/
	get_out_of_the_recv_queue(cc_sw_info);
	
	
		
	/*
	/****to be continue******/
	pthread_rwlock_unlock(&cc_sw_info->send_queue->queue_lock);
	*/
}
	
/*temp code*/
buffer*
get_out_of_the_recv_queue(cc_sw_info)
{	
	pthread_rwlock_lock(&cc_sw_info->queue->queue_lock,NULL);

	if(cc_sw_info->queue->length == 0)
	{
		log_err_for_cc();
		return CC_ERROR:
	}
	
	buffer* tans_buff;
	tans_buff = cc_sw_info->queue->head->data;
	
	pthread_rwlock_lock(&cc_sw_info->queue->queue_lock,NULL);
	
	return tans_buff;
}
/************************************************/

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



