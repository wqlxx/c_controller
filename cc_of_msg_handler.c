/*
 * cc_of_msg_handler functions.
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

#include "cc_of_msg_handle.h"


int
cc_event_recv_hello(sw_info* cc_sw_info,buffer* buf)
{
	log_info_for_cc("recv a hello, and send a feature request");
	int ret;
	ret = cc_timer_start();

	ret = cc_send_features_request(cc_sw_info, buf);
	if(ret < 0)
		return CC_ERROR;
	return CC_SUCCESS;
}


int
cc_event_recv_feature_reply(sw_info* cc_sw_info, uint32_t xid)
{
	log_info_for_cc("recv feature reply and prepare to send set config");
	int ret;

	struct timeval* now_tv;
	ret = gettimeofday(now_tv,NULL);
	if( ret < 0 )
		return CC_ERROR;

	ret = cc_timer_check(cc_sw_info->xid_entry->tv,now_tv);
	if( ret < 0 )
		return CC_ERROR;

	ret = cc_send_set_config(cc_sw_info, buf)
	if( ret < 0 )
		return CC_ERROR;

	return CC_SUCCESS;
}


int
cc_event_recv_echo_reply(sw_info* cc_sw_info, buffer* buf)
{
	/*check the xid and time pair, if the xid is time-out
	 *delete it.and check other xid,to find the one who is timeoud
	 */
	 log_info_for_cc("recv a echo reply, and send a request");
	 int ret;
	 uint32_t xid;
	 struct timeval now_tv;

	 ret = gettimeofday(&now_tv,NULL);
	 if( ret < 0 )
	 {
	 	perror("get now timeval");
	 	return CC_ERROR;
	 
	 	cc_timer_check(cc_sw_info->xid_table,&now_tv);

		ret = cc_event_send_to_app(cc_sw_info, buf);
		if( ret < 0 )
			return CC_ERROR;
		
	 	ret = cc_send_echo_request(cc_sw_info, buf);
	 	if( ret < 0 )
			return CC_ERROR;
	 }
	 return CC_SUCCESS;
	
}


int
cc_event_recv_echo_request(sw_info* cc_sw_info,uint32_t xid, buffer* buf)
{
	/*set timer begin to work, wright down the time which send to msg*/
	
	log_info_for_cc("recv a echo request, and send a reply");
	int ret;
	buffer* echo_reply_msg;

	if( (echo_reply_msg = cc_create_echo_reply(xid,buf)) == NULL )
		return CC_ERROR;

	cc_timer_start();
	ret = cc_send_echo_reply(cc_sw_info, echo_reply_msg);
	return ret;
}


int
cc_event_send_to_app(sw_info* cc_sw_info, buffer* buf)
{
	msg_for_app* tmp_buf;
	int ret;
	
	tmp_buf->buf = buf;
	tmp_buf->dpid = cc_sw_info->cc_switch->dpid;
	//ret = cc_send_to_app(tmp_buf);
	/*TEMP : set ret = 0,to pass the function
	*/
	ret = 0;
	if( ret < 0)
		free_buffer(buf);
		return CC_ERROR;
	
	free_buffer(buf);
	return CC_SUCCESS;
}

/*
 *
 */
int
cc_event_send_msg(buffer* buf)
{
	return CC_SUCCESS;
}

