/*
 * Utility functions.
 *
 * Author: qiang wang <wqlxx@yahoo.com.cn>
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "cc_secure_channel_recv.h"

FUNC_CB
cc_select_handler(uint16_t type)
{
	FUNC_CB func_cb;
	switch ( type ) {
  	// Immutable messages.
  		case OFPT_HELLO:
			func_cb = cc_recv_hello_msg;
   			//ret = ofpmsg_recv_hello( sw_info, buf );
   			break;
	
  		case OFPT_ERROR:
			func_cb = cc_recv_err_msg;
   			//ret = ofpmsg_recv_error( sw_info, buf );
   			break;

  		case OFPT_ECHO_REQUEST:
			func_cb = cc_recv_echo_request;
    		//ret = ofpmsg_recv_echorequest( sw_info, buf );
    		break;

  		case OFPT_ECHO_REPLY:
			func_cb = cc_recv_echo_reply;
   			//ret = ofpmsg_recv_echoreply( sw_info, buf );
    		break;

  		case OFPT_VENDOR:
			func_cb = cc_recv_vendor;
    		//ret = ofpmsg_recv_vendor( sw_info, buf );
    		break;

  		// Switch configuration messages.
  		case OFPT_FEATURES_REPLY:
			func_cb = cc_recv_features_reply;
    		//ret = ofpmsg_recv_featuresreply( sw_info, buf );
    		break;

  		case OFPT_GET_CONFIG_REPLY:
			func_cb = cc_recv_get_config_reply;
    		//ret = ofpmsg_recv_getconfigreply( sw_info, buf );
    		break;

  		// Asynchronous messages.
  		case OFPT_PACKET_IN:
			func_cb = cc_recv_packet_in;
    		//ret = ofpmsg_recv_packetin( sw_info, buf );
    		break;

  		case OFPT_FLOW_REMOVED:
			func_cb = cc_recv_flow_removed;
    		//ret = ofpmsg_recv_flowremoved( sw_info, buf );
    		break;

  		case OFPT_PORT_STATUS:
			func_cb = cc_recv_port_status;
    		//ret = ofpmsg_recv_portstatus( sw_info, buf );
    		break;

  		// Statistics messages.
  		case OFPT_STATS_REPLY:
			func_cb = cc_recv_stats_reply;
    		//ret = ofpmsg_recv_statsreply( sw_info, buf );
    		break;

  		// Barrier messages.
  		case OFPT_BARRIER_REPLY:
			func_cb = cc_recv_barrier_reply;
    		//ret = ofpmsg_recv_barrierreply( sw_info, buf );
    		break;

  		// Queue Configuration messages.
  		case OFPT_QUEUE_GET_CONFIG_REPLY:
			func_cb = NULL;
    		//ret = ofpmsg_recv_queue_getconfigreply( sw_info, buf );
    		break;

  		default:
    		assert( 0 );
    		break;
  }

  return func_cb;
}


static int
cc_ofpmsg_handle(sw_info* cc_sw_info,buffer* buf)
{
	int ret;
	struct ofp_header *header;
	uint16_t error_type,error_code;
	FUNC_CB func_cb;

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

    	cc_send_error_msg( sw_info, error_type, error_code, buf );
    	free_buffer( buf );

    	return CC_ERROR;
	}
	worker_buf* buf_arg;
	buf_arg->cc_sw_info = cc_sw_info;
	buf_arg->buf = buf;
	func_cb = cc_select_handler(header->type);

	if(func_cb == NULL)
	{
		perror("handler ofmsg error");
		log_err_for_cc("handlerofmsg error!");
		return CC_ERROR;
	}
	
	ret = pool_add_worker(cc_sw_info->cc_recv_thread_pool, func_cb, buf_arg);
	
	return CC_SUCCESS;
}


static int
cc_recv_from_secure_channel(sw_info *cc_sw_info)
{

	buffer *tmp_buff; 
	szie_t tmp_recv_buff_length;
	size_t msg_len;
	struct ofp_header *header;
	//FUNC_CB func_cb;
	
	tmp_buff = alloc_buffer_with_length(CC_RECV_BUFFER_SIZE);
	if(tmp_buff == NULL)
	{
		log_debug_for_cc("create buffer failed!");
		return CC_ERROR;
	}
	/*
	fd_set readfds;
	struct 
	int maxfd = cc_sw_info->cc_switch->cc_socket->fd + 1;
	FD_SET(maxfd,&readfds);
	ret = select(maxfd,&readfds,NULL,NULL,cc_time);
	if( ret == 0 )
	*/

	tmp_recv_buff_length = read(cc_sw_info->cc_switch->cc_socket->fd, tmp_buff->data, CC_BUFFER_SIZE);
	if(tmp_recv_buff_length < 0)
	{
		log_debug_for_cc("recv failed!");
		perror("recv failed!");
		if( errno == EINTER || errno == EAGAIN || errno == EWOULDBLOCK )
		{
			return CC_ERROR;
		}
		//reconnect();
		return CC_ERROR;
	}

	/*input the msg in queue*/
	//enqueue_message(cc_sw_info->recv_queue,tmp_buff);

	if( tmp_recv_buff_length >= sizeof(struct ofp_header) ) 
	{
		
		/*it will reload the tmp_buff->length with thc really recv length ,
		 *no more than the CC_BUFFER_SIZE
		 */
		tmp_buff->length = tmp_recv_buff_length;

		header = tmp_buff->data;
		if ( header->version != OFP_VERSION ) {
      		error( "Receive error: invalid version (version %d)", header->version );
      		cc_send_error_msg( sw_info,OFPET_BAD_REQUEST, OFPBRC_BAD_VERSION, sw_info->fragment_buf );
      		return CC_ERROR;
   		}
		
		uint16_t message_length = ntohs( header->length );
		if( message_length > CC_RECV_BUFFER_SIZE )
		{
			perror("recv msg size is larger than buff");
			return CC_ERROR;
		}
		/*input the msg in queue*/
		//func_cb = cc_select_handler(type);
		enqueue_message(cc_sw_info->recv_queue, tmp_buff);
	}	

	return CC_SUCCESS;
}


static int
cc_secure_channle_read(sw_info* cc_sw_info)
{
	int ret;
	buffer* buf;
	ret = cc_recv_from_secure_channel(cc_sw_info);
	if( ret == -1 )
		return CC_ERROR;
	
	while( cc_sw_info->recv_queue->length > 0 ){
		buf = dequeue_message(cc_sw_info->recv_queue);
		cc_ofpmsg_handle(cc_sw_info, buf);
	}

	return CC_SUCCESS;
}


/*
 *	send message to secure channel
 */
static int
cc_append_to_writev_args( buffer *message, void *user_data )
{
  cc_writev_args *args = user_data;

  args->iov[ args->iovcnt ].iov_base = message->data;
  args->iov[ args->iovcnt ].iov_len = message->length;
  args->iovcnt++;

  if ( args->iovcnt >= IOV_MAX ) {
    return CC_ERROR;
  }

  return CC_SUCCESS;
}


/* it is for timer */
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

#endif

