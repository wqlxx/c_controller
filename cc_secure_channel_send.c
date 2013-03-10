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
cc_flush_to_secure_channel(sw_info* cc_sw_info)
{
	buffer* msg;
	
	while(( peek_message(cc_sw_info->recv_queue, msg)) < 0 )
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
		/*after send ,free the buf*/
		buffer* buf = dequeue_message(message_queue * queue);
		free_buffer(buf);
	}
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

