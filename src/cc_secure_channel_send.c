/*
 * cc_secure_channel_send functions.
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


#include "cc_secure_channel_send.h"

FUNC_CB_S  
cc_select_send_func(uint16_t type)
{
	int ret;
	FUNC_CB_S func_cb;
	switch(type){
		  // Immutable messages.
			  case OFPAPP_HELLO:
				  func_cb = cc_send_hello;
				  break;
		  
			  case OFPAPP_ERROR:
				  func_cb = cc_send_error_msg;
				  break;
		
			  case OFPAPP_ECHO_REQUEST:
				  func_cb = cc_send_echo_request;
				  break;
		
			  case OFPAPP_ECHO_REPLY:
				  func_cb = cc_send_echo_reply;
				  break;

			  case OFPAPP_FEATURE_REQUEST:
				  func_cb = cc_send_features_request;
				  break;
		
			  // Asynchronous messages.
			  case OFPAPP_FLOW_MOD:
				  func_cb = cc_send_flow_mod;
				  break;
		
			  case OFPAPP_PORT_MOD:
				  func_cb = cc_send_port_mod;
				  break;
		
			  // Statistics messages.
			  case OFPAPP_STATS_REQUEST:
				  func_cb = cc_send_stats_request;
				  break;
		
			  // Queue Configuration messages.
			  case OFPAPP_QUEUE_STATS_REQUEST:
				  func_cb = cc_send_queue_stats_request;
				  break;

			  case OFPAPP_TABLE_STATS_REQUEST:
				  func_cb = cc_send_table_stats_request;
				  break;

			  case OFPAPP_VENDOR_STATS_REQUEST:
				  func_cb = cc_send_vendor_stats_request;
				  break;

			  case OFPAPP_AGGREGATE_STATS_REQUEST:
				  func_cb = cc_send_aggregate_stats_request;
				  break;

			  case OFPAPP_FLOW_STATS_REQUEST:
				  func_cb = cc_send_flow_stats_request;
				  break;

			  case OFPAPP_SET_CONFIG:
				  func_cb = cc_send_set_config;
				  break;

			  case OFPAPP_GET_CONFIG_REQUEST:
				  func_cb = cc_send_get_config_request;
				  break;

			  case OFPAPP_GET_DESC_STATS_REQUEST:
				  func_cb = cc_send_get_desc_stats;
				  break;

			  case OFPAPP_PACKET_OUT:
				  func_cb = cc_send_packet_out;
				  break;

			  default:
				  func_cb = NULL;
				  break;
		}

	return func_cb;
}


static int
cc_send_to_secure_channel(sw_info* cc_sw_info,buffer* buf)
{
	int ret;
	buffer* msg;

	if( sw_info->send_queue == NULL )
		sw_info->send_queue = create_message_queue();

	ret = enqueue_message(cc_sw_info->send_queue, buf);

	return ret;
}


static int
cc_send_to_secure_channel_app(sw_info* cc_sw_info,buffer* buf)
{
	int ret;
	buffer* msg;

	if( cc_sw_info->app_send_queue == NULL )
		cc_sw_info->app_send_queue = create_message_queue();

	ret = enqueue_message(cc_sw_info->send_queue, buf);

	return ret;
}


static int
cc_flush_to_secure_channel(sw_info* cc_sw_info)
{
	buffer* msg;
	int count = 0;
	while(( msg = dequeue_message(cc_sw_info->recv_queue, msg)) != NULL && count < 50)
	{
		ssize_t write_length = write( cc_sw_info->cc_switch->cc_socket->fd, msg->data, CC_BUFFER_SIZE);
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
		free_buffer(msg);
		count++;
	}
	return CC_SUCCESS;
}


#if 0
/*send to secure channel through socket*/
int
cc_secure_channel_write(sw_info *cc_sw_info, buffer* buf)
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

