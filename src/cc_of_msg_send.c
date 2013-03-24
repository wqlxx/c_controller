/*
 * OpenFlow Switch Manager
 *
 * Copyright (C) 2008-2012 NEC Corporation
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
#include "cc_of_msg_send.h"

int
cc_send_hello( void ) 
{
  int ret;
  buffer *buf;

  buf = cc_create_hello( cc_create_xid() );

  ret = cc_send_to_secure_channel( buf );
  if ( ret == 0 ) {
    log_info_for_cc( "Send 'hello' to a switch");
  }

  return ret;
}


int
cc_send_echo_request( sw_info* cc_sw_info, buffer *buf ) 
{
  int ret;
  buffer *buf;
  uint32_t xid;

  xid = cc_generate_xid();
  buf = cc_create_echo_request( xid, buf );
  free_buffer( buf );

  ret = cc_send_to_secure_channel( sw_info, buf );
  if ( ret == 0 ) {
  	log_info_for_cc("send a echo request!");
  }

  return ret;
}


int
cc_send_echo_reply(sw_info* cc_sw_info,buffer *buf ) 
{
  int ret;
  buffer *buf;

  buf = cc_create_echo_reply( xid, buf );
  free_buffer( buf );

  ret = cc_send_to_secure_channel( sw_info, buf );
  if ( ret == 0 ) {
    //debug( "Send 'echo reply' to a switch %#" PRIx64 ".", sw_info->datapath_id );
  }

  return ret;
}


int
cc_send_features_request( sw_info* cc_sw_info,buffer* buf )
{
  int ret;
  buffer *buf;
  
  /*NOTICE! here we should add the ofp_pull, to get out of the 
   *exactly ofp msg
   */
  //ofpbuf_pull(buf,ofp_msg);
  buf = cc_create_features_request( generate_xid() );

  ret = cc_send_to_secure_channel( sw_info, buf );
  if ( ret == 0 ) {
    //debug( "Send 'features request' to a switch. fd:%d", sw_info->secure_channel_fd );
  }

  free_buffer()
  return ret;
}


int
cc_send_get_config_request(sw_info* cc_sw_info, buffer* buf)
{
	int ret;
	buffer *buf;

	buf = cc_create_get_config_request(cc_sw_info->xid);
	return ret = cc_send_to_secure_channel(cc_sw_info->send_queue,buf);
	
}


int
cc_send_set_config(sw_info *cc_sw_info, buffer* buf) 
{
  int ret;
  buffer *msg;
  uint32_t xid;

  xid = cc_generate_xid();
  
  // default switch configuration
  uint16_t config_flags = OFPC_FRAG_NORMAL;
  int16_t miss_send_len = USHRT_MAX;

  
  msg = cc_create_set_config( xid, config_flags,
                           miss_send_len );

  ret = cc_send_to_secure_channel( sw_info, buf );
  if ( ret == 0 ) {
    //debug( "Send 'set config request' to a switch %#" PRIx64 ".", sw_info->datapath_id );
  }

  free_buffer(buf);
  return ret;
}


int
cc_send_error_msg( sw_info* cc_sw_info, uint16_t type, uint16_t code, buffer *data ) 
{
  int ret;
  buffer *msg;
  uint32_t xid;


  if ( data->length > OFP_ERROR_MSG_MAX_DATA ) {
    // FIXME
    data->length = OFP_ERROR_MSG_MAX_DATA;
  }

  xid = cc_generate_xid();
  msg = cc_create_error( xid, type, code, data );

  ret = cc_send_to_secure_channel( sw_info, msg );
  if ( ret == 0 ) {
    log_info_for_cc( "Send 'error' to a switch ");
  }

  return ret;
}


int
cc_send_flow_mod(sw_info* cc_sw_info, buffer* buf)
{
	buffer* send_buf;
	uint32_t xid;
	struct ofp_match match;
	uint64_t cookie;
	uint16_t command;
    uint16_t idle_timeout;
	uint16_t hard_timeout;
    uint16_t priority;
	uint32_t buffer_id;
    uint16_t out_port;
	uint16_t flags;
    openflow_actions *actions; 	

	xid = cc_generate_xid(cc_sw_info);
	struct ofp_match match = (struct ofp_match*)buf;
	cookie = cc_get_cookie();
	send_buf = cc_create_flow_mod(xid, match, cookie, command, idle_timeout,
					hard_timeout,  priority,  buffer_id, out_port,
					flags, actions);
	if( send_buf == NULL )
	{
		log_err_for_cc("cc_create_packet_out");
		return CC_ERROR;
	}

	ret = cc_send_to_secure_channel(cc_sw_info, send_buf);
	if( ret < 0 )
	{
		log_err_for_cc("cc_send_to_secure_channel");
		return CC_ERROR;
	}
	
	return CC_SUCCESS;
}


int
cc_send_packet_out(sw_info* cc_sw_info, buffer* buf)
{
	buffer* send_buf;
	uint32_t xid;
	uint32_t buffer_id;
	uint16_t in_port;
	openflow_actions* actions;
	buffer* data;
	int ret;
	
	send_buf = cc_create_packet_out(xid, buffer_id, in_port, actions, data)
	if( send_buf == NULL )
	{
		log_err_for_cc("cc_create_packet_out");
		return CC_ERROR;
	}

	ret = cc_send_to_secure_channel(cc_sw_info, send_buf);
	if( ret < 0 )
	{
		log_err_for_cc("cc_send_to_secure_channel");
		return CC_ERROR;
	}

	return CC_SUCCESS;
}

int
cc_send_port_mod(sw_info* cc_sw_info, buffer* buf)
{
	buffer* send_buf;
	int ret;
	uint32_t xid;
	uint16_t port_no;
	uint8_t hw_addr[OFP_ETH_ALEN];
	uint32_t config;
	uint32_t mask;
	uint32_t advertise;
	
	send_buf = cc_create_port_mod(xid, port_no,  hw_addr, config, mask, advertise);
	if( send_buf == NULL )
	{
		log_err_for_cc("cc_create_packet_out");
		return CC_ERROR;
	}

	ret = cc_send_to_secure_channel(cc_sw_info, send_buf);
	if( ret < 0 )
	{
		log_err_for_cc("cc_send_to_secure_channel");
		return CC_ERROR;
	}

	return CC_SUCCESS;	
}


int
cc_send_stats_request(sw_info* cc_sw_info, buffer* buf)
{
	buffer* send_buf;
	int ret;
	uint32_t xid;
	uint16_t type;
	uint16_t length;
	uint16_t flags;

	send_buf = cc_create_stats_request(xid, type, length, flags);
	if( send_buf == NULL )
	{
		log_err_for_cc("cc_create_packet_out");
		return CC_ERROR;
	}

	ret = cc_send_to_secure_channel(cc_sw_info, send_buf);
	if( ret < 0 )
	{
		log_err_for_cc("cc_send_to_secure_channel");
		return CC_ERROR;
	}

	return CC_SUCCESS;		
}


int
cc_send_get_desc_stats(sw_info* cc_sw_info, buffer* buf)
{
	buffer* send_buf;
	int ret;
	uint32_t xid;
	uint16_t flags;

	send_buf = cc_create_desc_stats_request(xid, flags);
	if( send_buf == NULL )
	{
		log_err_for_cc("cc_create_packet_out");
		return CC_ERROR;
	}

	ret = cc_send_to_secure_channel(cc_sw_info, send_buf);
	if( ret < 0 )
	{
		log_err_for_cc("cc_send_to_secure_channel");
		free_buffer(send_buf);
		return CC_ERROR;
	}	
	return CC_SUCCESS;
}


int
cc_send_flow_stats_request(sw_info* cc_sw_info, buffer* buf)
{
	buffer* send_buf;
	int ret;
	uint32_t xid;
	uint16_t flags;
	struct ofp_match match;
	uint8_t table_id;
	uint16_t out_port;
	
	send_buf = cc_create_flow_stats_request(xid, flags, match, 
									table_id, out_port);
	if( send_buf == NULL )
	{
		log_err_for_cc("cc_create_packet_out");
		return CC_ERROR;
	}

	ret = cc_send_to_secure_channel(cc_sw_info, send_buf);
	if( ret < 0 )
	{
		log_err_for_cc("cc_send_to_secure_channel");
		free_buffer(send_buf);
		return CC_ERROR;
	}	
	
	return CC_SUCCESS;	
}


int
cc_send_aggregate_stats_request(sw_info* cc_sw_info, buffer* buf)
{
	buffer* send_buf;
	int ret;
	uint32_t xid;
	uint16_t flags;
	struct ofp_match match;
	uint8_t table_id;
	uint16_t out_port;

	send_buf = cc_create_aggregate_stats_request(const uint32_t transaction_id,const uint16_t flags,const struct ofp_match match,const uint8_t table_id,const uint16_t out_port);
	if( send_buf == NULL )
	{
		log_err_for_cc("cc_create_aggregate_stats_request");
		return CC_ERROR;
	}

	ret = cc_send_to_secure_channel(cc_sw_info, send_buf);
	if( ret < 0 )
	{
		log_err_for_cc("cc_send_to_secure_channel");
		free_buffer(send_buf);
		return CC_ERROR;
	}	
	
	return CC_SUCCESS;	
}


int
cc_send_table_stats_request(sw_info* cc_sw_info, buffer* buf)
{
	buffer* send_buf;
	int ret;
	uint32_t xid;
	uint16_t flags;

	send_buf = cc_create_table_stats_request(xid, flags);
	if( send_buf == NULL )
	{
		log_err_for_cc("cc_create_table_stats_request");
		return CC_ERROR;
	}

	ret = cc_send_to_secure_channel(cc_sw_info, send_buf);
	if( ret < 0 )
	{
		log_err_for_cc("cc_send_to_secure_channel");
		return CC_ERROR;
	}

	return CC_SUCCESS;	
}


int
cc_send_port_stats_request(sw_info* cc_sw_info, buffer* buf)
{
	buffer* send_buf;
	int ret;
	uint32_t xid;
	uint16_t flags;
	uint16_t port_no;

	send_buf = cc_create_port_stats_request(xid, flags, port_no);
	if( send_buf == NULL )
	{
		log_err_for_cc("cc_create_port_stats_request");
		return CC_ERROR;
	}

	ret = cc_send_to_secure_channel(cc_sw_info, send_buf);
	if( ret < 0 )
	{
		log_err_for_cc("cc_send_to_secure_channel");
		return CC_ERROR;
	}

	return CC_SUCCESS;	
}


int
cc_send_queue_stats_request(sw_info* cc_sw_info, buffer* buf)
{
	buffer* send_buf;
	int ret;
	uint32_t xid;
	uint16_t flags;
	uint16_t port_no;
	uint32_t queue_id;

	send_buf = cc_create_queue_stats_request(xid, flags, port_no, queue_id);
	if( send_buf == NULL )
	{
		log_err_for_cc("cc_send_queue_stats_request");
		return CC_ERROR;
	}

	ret = cc_send_to_secure_channel(cc_sw_info, send_buf);
	if( ret < 0 )
	{
		log_err_for_cc("cc_send_to_secure_channel");
		return CC_ERROR;
	}

	return CC_SUCCESS;		
}


int
cc_send_vendor_stats_request(sw_info* cc_sw_info, buffer* buf)
{
	buffer* send_buf;
	int ret;
	uint32_t xid;
	uint16_t flags;
	uint32_t vendor;
	buffer body;

	send_buf = cc_create_vendor_stats_request(xid, flags, vendor, body);
	if( send_buf == NULL )
	{
		log_err_for_cc("cc_create_vendor_stats_request");
		return CC_ERROR;
	}

	ret = cc_send_to_secure_channel(cc_sw_info, send_buf);
	if( ret < 0 )
	{
		log_err_for_cc("cc_send_to_secure_channel");
		return CC_ERROR;
	}

	return CC_SUCCESS;
}


/*
 *@param: buf may be the lldp , arp and so on
 */

int
cc_send_msg_to_switch(sw_info* cc_sw_info, buffer* buf)
{

	size_t packet_len = sizeof(struct ofp_packet_out) + actions_length + buf->length;
	buffer* new_;
	new_ = alloc_buffer_with_length(packet_len);
	
	struct ofp_packet_out* opo;
	opo = (struct ofp_packet_out*)malloc(sizeof(struct ofp_packet_out));
	memset(opo, 0, sizeof(struct ofp_packet_out));
	opo->header.type = OFP_VERSION;
	opo->header.type = OFPT_PACKET_OUT;
	opo->header.length = htons(packet_len);

	/*buffer id should be UINT32_MAX, when i want to send packet which is create by myseld
	*/
	opo->buffer_id = htonl(UINT32_MAX);
	opo->in_port = htons(OFPP_LOCAL);//OFPP_LOCAL is virtual port in switch
	opo->actions_len = htons(actions_len);
	memcpy(opo.actions, acitions, cations_len);
	memcpy((uint8_t *)opo->actions + actions_len, buf->data, buf->length);

	buf->data = (void*)opo;

	buffer* new_buf;
	uint32_t xid;
	xid = cc_generate_xid(cc_sw_info);
	uint32_t buffer_id = htons(UINT32_MAX);// in order to send packet in data
	uint16_t in_port = OFPP_LOCAL;
	openflow_actions* actions;
	actions->n_actions = 1;
	actions->list->data =  recv_from_app;
	
	cc_create_packet_out(xid,const uint32_t buffer_id, in_port, actions, new_buf);
	cc_send_to_secure_channel(cc_sw_info, buf);

	return CC_SUCCESS;
}



static int
update_flowmod_cookie( buffer *buf, char *service_name )
{
  struct ofp_flow_mod *flow_mod = buf->data;
  uint16_t command = ntohs( flow_mod->command );
  uint16_t flags = ntohs( flow_mod->flags );
  uint64_t cookie = ntohll( flow_mod->cookie );

  switch ( command ) {
  case OFPFC_ADD:
  {
    uint64_t *new_cookie = insert_cookie_entry( &cookie, service_name, flags );
    if ( new_cookie == NULL ) {
      return CC_ERROR;
    }
    flow_mod->cookie = htonll( *new_cookie );
    flow_mod->flags = htons( flags | OFPFF_SEND_FLOW_REM );
  }
  break;

  case OFPFC_MODIFY:
  case OFPFC_MODIFY_STRICT:
  {
    cookie_entry_t *entry = lookup_cookie_entry_by_application( &cookie, service_name );
    if ( entry != NULL ) {
      flow_mod->cookie = htonll( entry->cookie );
    }
    else {
      uint64_t *new_cookie = insert_cookie_entry( &cookie, service_name, flags );
      if ( new_cookie == NULL ) {
        return CC_ERROR;
      }
      flow_mod->cookie = htonll( *new_cookie );
    }
    flow_mod->flags = htons( flags | OFPFF_SEND_FLOW_REM );
  }
  break;

  case OFPFC_DELETE:
  case OFPFC_DELETE_STRICT:
  {
    cookie_entry_t *entry = lookup_cookie_entry_by_application( &cookie, service_name );
    if ( entry != NULL ) {
      flow_mod->cookie = htonll( entry->cookie );
    }
    else {
      flow_mod->cookie = htonll( RESERVED_COOKIE );
    }
    flow_mod->flags = htons( flags | OFPFF_SEND_FLOW_REM );
  }
  break;

  default:
    return CC_ERROR;
  }

  return CC_SUCCESS;
}


