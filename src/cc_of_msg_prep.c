/*
 *  mul_of_msg.c: MUL openflow message handling 
 *  Copyright (C) 2012, Dipjyoti Saikia <dipjyoti.saikia@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "cc_of_msg_prep.h"

static buffer *
cc_create_header( const uint32_t xid, const uint8_t type, const uint16_t length ) {

  buffer *buf;
  alloc_new_data(buf);
  if(buf == NULL)
  {
  	perror("alloc failed");
	log_err_for_cc("alloc error!");
	return NULL;
  }

  struct ofp_header *header = append_back_buffer( buffer, length );
  if(header== NULL)
  {
  	perror("create header failed");
	log_err_for_cc("create header error!");
	return NULL;
  }
  memset( header, 0, length );

  header->version = OFP_VERSION;
  header->type = type;
  header->length = htons( length );
  header->xid = htonl( xid );

  return buffer;
}

static buffer *
cc_create_error(uint32_t xid, uint16_t type, uint16_t code, buffer* data)
{
	struct ofp_error_msg *error_msg;
	uint16_t len;
	uint16_t data_len = 0;
	buffer *buffer;

	if((data != NULL) && (data->length > 0) )
	{
		data_len = (uint16_t)data->length;
	}

	len = (uint16_t)(sizeof( struct ofp_error_msg ) + data_len);
	buffer = cc_create_header(xid,OFPT_ERROR,len);

	error_msg = (struct ofp_error_msg *)buffer->data;
	error_msg->type = htons(type);
	error_msg->code = htons(code);

	if(data_len > 0)
	{
		memcpy(error_msg->data,data->data,data->length);
	}

	return buffer;
}


static buffer*
cc_create_hello(uint32_t xid)
{
	return cc_create_header(xid,OFPT_HELLO, sizeof( struct ofp_header ));
}

/* param: xid is the genereate with rand
*  param: buf is used to calculate the len except the ofp header
*/
static buffer*
cc_create_echo_request(uint32_t xid, buffer* buf)
{
	int data_length = 0;
	if ( ( buf!= NULL ) && ( buf->length > 0 ) ) {
		data_length = ( uint16_t ) buf->length;
  	}

	buffer* echo_requset = cc_create_header(xid, OFPT_ECHO_REQUEST,  ( uint16_t ) ( sizeof( struct ofp_header ) + data_length ) );
	if( echo_requset == NULL )
	{
		return NULL;
	}

	if( data_length > 0 )
	{
		memcpy( (char*)echo_request->data + sizeof( struct ofp_header ), buf->data, data_length);
	}

	return echo_requset;
}


static buffer*
cc_create_echo_reply(uint32_t xid,buffer* buf)
{
	int data_length = 0;
	if ( ( buf!= NULL ) && ( buf->length > 0 ) ) {
		data_length = ( uint16_t ) buf->length;
  	}
	
	buffer *echo_reply = cc_create_header( xid, OFPT_ECHO_REPLY, ( uint16_t ) ( sizeof( struct ofp_header ) + data_length ) );
		
	if ( data_length > 0 ) {
		memcpy( ( char * ) echo_reply->data + sizeof( struct ofp_header ), body->data, data_length );
  	}

	return echo_reply;
}


buffer *
cc_create_vendor( const uint32_t transaction_id, const uint32_t vendor, const buffer *data ) {
  void *d;
  uint16_t length;
  uint16_t data_length = 0;
  buffer *buffer;
  struct ofp_vendor_header *vendor_header;

  if ( ( data != NULL ) && ( data->length > 0 ) ) {
    data_length = ( uint16_t ) data->length;
  }

  //debug( "Creating a vendor ( xid = %#x, vendor = %#x, data length = %u ).",
    //     transaction_id, vendor, data_length );

  length =  ( uint16_t ) ( sizeof( struct ofp_vendor_header ) + data_length );
  buffer = cc_create_header( transaction_id, OFPT_VENDOR, length );
  //assert( buffer != NULL );

  vendor_header = ( struct ofp_vendor_header * ) buffer->data;
  vendor_header->vendor = htonl( vendor );

  if ( data_length > 0 ) {
    d = ( void * ) ( ( char * ) buffer->data + sizeof( struct ofp_vendor_header ) );
    memcpy( d, data->data, data_length );
  }

  return buffer;
}


buffer *
cc_create_features_request( const uint32_t transaction_id ) {
  //debug( "Creating a features request ( xid = %#x ).", transaction_id );

  return cc_create_header( transaction_id, OFPT_FEATURES_REQUEST, sizeof( struct ofp_header ) );
}


buffer *
cc_create_features_reply( const uint32_t transaction_id, const uint64_t datapath_id,
                       const uint32_t n_buffers, const uint8_t n_tables,
                       const uint32_t capabilities, const uint32_t actions,
                       const list_element *ports ) {
  char port_str[ 1024 ];
  int count = 0;
  uint16_t n_ports = 0;
  buffer *buffer;
  struct ofp_switch_features *switch_features;
  struct ofp_phy_port *phy_port;
  struct ofp_phy_port pn;
  list_element *p = NULL, *port;

  //debug( "Creating a features reply "
    //     "( xid = %#x, datapath_id = %#" PRIx64 ", n_buffers = %u, n_tables = %u, capabilities = %#x, actions = %#x ).",
      //   transaction_id, datapath_id, n_buffers, n_tables, capabilities, actions );

  if ( ports != NULL ) {
    p = ( list_element * ) malloc( sizeof( list_element ) );
    memcpy( p, ports, sizeof( list_element ) );

    port = p;
    while ( port != NULL ) {
      port = port->next;
      n_ports++;
    }
  }

  //debug( "# of ports = %u.", n_ports );

  buffer = cc_create_header( transaction_id, OFPT_FEATURES_REPLY,
                          ( uint16_t ) ( offsetof( struct ofp_switch_features, ports )
                                         + sizeof( struct ofp_phy_port ) * n_ports ) );
  //assert( buffer != NULL );

  switch_features = ( struct ofp_switch_features * ) buffer->data;
  switch_features->datapath_id = htonll( datapath_id );
  switch_features->n_buffers = htonl( n_buffers );
  switch_features->n_tables = n_tables;
  memset( switch_features->pad, 0, sizeof( switch_features->pad ) );
  switch_features->capabilities = htonl( capabilities );
  switch_features->actions = htonl( actions );

  if ( n_ports ) {
    phy_port = ( struct ofp_phy_port * ) ( ( char * ) buffer->data
                                           + offsetof( struct ofp_switch_features, ports ) );
    port = p;
    while ( port != NULL ) {
      phy_port_to_string( port->data, port_str, sizeof( port_str ) );
      //debug( "[%u] %s", count++, port_str );
      hton_phy_port( &pn, ( struct ofp_phy_port * ) port->data );
      memcpy( phy_port, &pn, sizeof( struct ofp_phy_port ) );
      port = port->next;
      phy_port++;
    }

    free( p );
  }

  return buffer;
}


buffer *
cc_create_get_config_request( const uint32_t transaction_id ) {
  //debug( "Creating a get config request ( xid = %#x ).", transaction_id );

  return cc_create_header( transaction_id, OFPT_GET_CONFIG_REQUEST, sizeof( struct ofp_header ) );
}


buffer *
cc_create_get_config_reply( const uint32_t transaction_id, const uint16_t flags,
                         const uint16_t miss_send_len ) {
  buffer *buffer;
  struct ofp_switch_config *switch_config;

  //debug( "Creating a get config reply ( xid = %#x, flags = %#x, miss_send_len = %u ).",
    //     transaction_id, flags, miss_send_len );

  buffer = cc_create_header( transaction_id, OFPT_GET_CONFIG_REPLY, sizeof( struct ofp_switch_config ) );
  //assert( buffer != NULL );

  switch_config = ( struct ofp_switch_config * ) buffer->data;
  switch_config->flags = htons( flags );
  switch_config->miss_send_len = htons( miss_send_len );

  return buffer;
}


buffer *
cc_create_set_config( const uint32_t transaction_id, const uint16_t flags, uint16_t miss_send_len ) {
  //debug( "Creating a set config ( xid = %#x, flags = %#x, miss_send_len = %u ).",
        // transaction_id, flags, miss_send_len );

  buffer *set_config = cc_create_header( transaction_id, OFPT_SET_CONFIG, sizeof( struct ofp_switch_config ) );
  //assert( set_config != NULL );

  struct ofp_switch_config *switch_config = ( struct ofp_switch_config * ) set_config->data;
  switch_config->flags = htons( flags );
  switch_config->miss_send_len = htons( miss_send_len );
  return set_config;
}


buffer *
cc_create_packet_in( const uint32_t transaction_id, const uint32_t buffer_id, const uint16_t total_len,
                  uint16_t in_port, const uint8_t reason, const buffer *data ) {
  uint16_t data_length = 0;
  buffer *buffer;
  struct ofp_packet_in *packet_in;

  if ( ( data != NULL ) && ( data->length > 0 ) ) {
    data_length = ( uint16_t ) data->length;
  }

  //debug( "Creating a packet-in "
        // "( xid = %#x, buffer_id = %#x, total_len = %u, "
        // "in_port = %u, reason = %#x, data length = %u ).",
        // transaction_id, buffer_id, total_len,
        // in_port, reason, data_length );

  buffer = cc_create_header( transaction_id, OFPT_PACKET_IN,
                          ( uint16_t ) ( offsetof( struct ofp_packet_in, data ) + data_length ) );
  //assert( buffer != NULL );

  packet_in = ( struct ofp_packet_in * ) buffer->data;
  packet_in->buffer_id = htonl( buffer_id );
  packet_in->total_len = htons( total_len );
  packet_in->in_port = htons( in_port );
  packet_in->reason = reason;
  packet_in->pad = 0;

  if ( data_length > 0 ) {
    memcpy( packet_in->data, data->data, data_length );
  }

  return buffer;
}


buffer *
cc_create_flow_removed( const uint32_t transaction_id, const struct ofp_match match,
                     const uint64_t cookie, const uint16_t priority,
                     const uint8_t reason, const uint32_t duration_sec,
                     const uint32_t duration_nsec, const uint16_t idle_timeout,
                     const uint64_t packet_count, const uint64_t byte_count ) {
  char match_str[ 1024 ];
  buffer *buffer;
  struct ofp_match m = match;
  struct ofp_flow_removed *flow_removed;

  
  /*
  // Because match_to_string() is costly, we check logging_level first.
  if ( get_logging_level() >= LOG_DEBUG ) {
    match_to_string( &m, match_str, sizeof( match_str ) );
    //debug( "Creating a flow removed "
           "( xid = %#x, match = [%s], cookie = %#" PRIx64 ", priority = %u, "
           "reason = %#x, duration_sec = %u, duration_nsec = %u, "
           "idle_timeout = %u, packet_count = %" PRIu64 ", byte_count = %" PRIu64 " ).",
           transaction_id, match_str, cookie, priority,
           reason, duration_sec, duration_nsec,
           idle_timeout, packet_count, byte_count );
  }
  */

  buffer = cc_create_header( transaction_id, OFPT_FLOW_REMOVED, sizeof( struct ofp_flow_removed ) );
  //assert( buffer != NULL );

  flow_removed = ( struct ofp_flow_removed * ) buffer->data;
  hton_match( &flow_removed->match, &m );
  flow_removed->cookie = htonll( cookie );
  flow_removed->priority = htons( priority );
  flow_removed->reason = reason;
  memset( &flow_removed->pad, 0, sizeof( flow_removed->pad ) );
  flow_removed->duration_sec = htonl( duration_sec );
  flow_removed->duration_nsec = htonl( duration_nsec );
  flow_removed->idle_timeout = htons( idle_timeout );
  memset( &flow_removed->pad2, 0, sizeof( flow_removed->pad2 ) );
  flow_removed->packet_count = htonll( packet_count );
  flow_removed->byte_count = htonll( byte_count );

  return buffer;
}


buffer *
cc_create_port_status( const uint32_t transaction_id, const uint8_t reason,
                    const struct ofp_phy_port desc ) {
  char desc_str[ 1024 ];
  buffer *buffer;
  struct ofp_phy_port d = desc;
  struct ofp_port_status *port_status;

  phy_port_to_string( &d, desc_str, sizeof( desc_str ) );
  //debug( "Creating a port status ( xid = %#x, reason = %#x, desc = [%s] ).",
        // transaction_id, reason, desc_str );

  buffer = cc_create_header( transaction_id, OFPT_PORT_STATUS, sizeof( struct ofp_port_status ) );
  //assert( buffer != NULL );

  port_status = ( struct ofp_port_status * ) buffer->data;
  port_status->reason = reason;
  memset( &port_status->pad, 0, sizeof( port_status->pad ) );
  hton_phy_port( &port_status->desc, &d );

  return buffer;
}


buffer *
cc_create_packet_out( const uint32_t transaction_id, const uint32_t buffer_id, const uint16_t in_port,
                   const openflow_actions *actions, const buffer *data ) {
  void *a, *d;
  uint16_t length;
  uint16_t data_length = 0;
  uint16_t action_length = 0;
  uint16_t actions_length = 0;
  buffer *buffer;
  struct ofp_packet_out *packet_out;
  struct ofp_action_header *action_header;
  list_element *action;

  if ( ( data != NULL ) && ( data->length > 0 ) ) {
    data_length = ( uint16_t ) data->length;
  }

  //debug( "Creating a packet-out ( xid = %#x, buffer_id = %#x, in_port = %u, data length = %u ).",
        // transaction_id, buffer_id, in_port, data_length );

  if ( buffer_id == UINT_MAX ) {
    if ( data == NULL ) {
      //die( "An Ethernet frame must be provided if buffer_id is equal to %#x", UINT_MAX );
      log_err_for_cc("An Ethernet frame must be provided if buffer_id is equal to UINT_MAX");
	  return CC_ERROR;
	}
    if ( data->length + ETH_FCS_LENGTH < ETH_MINIMUM_LENGTH ) {
      //die( "The length of the provided Ethernet frame is shorter than the minimum length of an Ethernet frame (= %d bytes).", ETH_MINIMUM_LENGTH );
      log_err_for_cc("The length of the provided Ethernet frame is shorter than the minimum length of an Ethernet frame");
	  return CC_ERROR;
	}
  }

  if ( actions != NULL ) {
    //debug( "# of actions = %d.", actions->n_actions );
    actions_length = get_actions_length( actions );
  }

  length = ( uint16_t ) ( offsetof( struct ofp_packet_out, actions ) + actions_length + data_length );
  buffer = cc_create_header( transaction_id, OFPT_PACKET_OUT, length );
  //assert( buffer != NULL );

  packet_out = ( struct ofp_packet_out * ) buffer->data;
  packet_out->buffer_id = htonl( buffer_id );
  packet_out->in_port = htons( in_port );
  packet_out->actions_len = htons( actions_length );

  if ( actions_length > 0 ) {
    a = ( void * ) ( ( char * ) buffer->data + offsetof( struct ofp_packet_out, actions ) );

    action = actions->list;
    while ( action != NULL ) {
      action_header = ( struct ofp_action_header * ) action->data;
      action_length = action_header->len;
      hton_action( ( struct ofp_action_header * ) a, action_header );
      a = ( void * ) ( ( char * ) a + action_length );
      action = action->next;
    }
  }

  if ( data_length > 0 ) {
    d = ( void * ) ( ( char * ) buffer->data
                     + offsetof( struct ofp_packet_out, actions ) + actions_length );
    memcpy( d, data->data, data_length );
  }

  return buffer;
}


buffer *
cc_create_flow_mod( const uint32_t transaction_id, const struct ofp_match match,
                 const uint64_t cookie, const uint16_t command,
                 const uint16_t idle_timeout, const uint16_t hard_timeout,
                 const uint16_t priority, const uint32_t buffer_id,
                 const uint16_t out_port, const uint16_t flags,
                 const openflow_actions *actions ) {
  void *a;
  char match_str[ 1024 ];
  uint16_t length;
  uint16_t action_length = 0;
  uint16_t actions_length = 0;
  buffer *buffer;
  struct ofp_match m = match;
  struct ofp_flow_mod *flow_mod;
  struct ofp_action_header *action_header;
  list_element *action;

  /*
  // Because match_to_string() is costly, we check logging_level first.
  if ( get_logging_level() >= LOG_DEBUG ) {
    match_to_string( &m, match_str, sizeof( match_str ) );
    //debug( "Creating a flow modification "
           "( xid = %#x, match = [%s], cookie = %#" PRIx64 ", command = %#x, "
           "idle_timeout = %u, hard_timeout = %u, priority = %u, "
           "buffer_id = %#x, out_port = %u, flags = %#x ).",
           transaction_id, match_str, cookie, command,
           idle_timeout, hard_timeout, priority,
           buffer_id, out_port, flags );
  }
  */

  if ( actions != NULL ) {
    //debug( "# of actions = %d.", actions->n_actions );
    actions_length = get_actions_length( actions );
  }

  length = ( uint16_t ) ( offsetof( struct ofp_flow_mod, actions ) + actions_length );
  buffer = cc_create_header( transaction_id, OFPT_FLOW_MOD, length );
  //assert( buffer != NULL );

  flow_mod = ( struct ofp_flow_mod * ) buffer->data;
  hton_match( &flow_mod->match, &m );
  flow_mod->cookie = htonll( cookie );
  flow_mod->command = htons( command );
  flow_mod->idle_timeout = htons( idle_timeout );
  flow_mod->hard_timeout = htons( hard_timeout );
  flow_mod->priority = htons( priority );
  flow_mod->buffer_id = htonl( buffer_id );
  flow_mod->out_port = htons( out_port );
  flow_mod->flags = htons( flags );

  if ( actions_length > 0 ) {
    a = ( void * ) ( ( char * ) buffer->data + offsetof( struct ofp_flow_mod, actions ) );

    action = actions->list;
    while ( action != NULL ) {
      action_header = ( struct ofp_action_header * ) action->data;
      action_length = action_header->len;
      hton_action( ( struct ofp_action_header * ) a, action_header );
      a = ( void * ) ( ( char * ) a + action_length );
      action = action->next;
    }
  }

  return buffer;
}


buffer *
cc_create_port_mod( const uint32_t transaction_id, const uint16_t port_no,
                 const uint8_t hw_addr[ OFP_ETH_ALEN ], const uint32_t config,
                 const uint32_t mask, const uint32_t advertise ) {
  buffer *buffer;
  struct ofp_port_mod *port_mod;


         transaction_id, port_no,
         hw_addr[ 0 ], hw_addr[ 1 ], hw_addr[ 2 ], hw_addr[ 3 ], hw_addr[ 4 ], hw_addr[ 5 ],
         config, mask, advertise );

  buffer = cc_create_header( transaction_id, OFPT_PORT_MOD, sizeof( struct ofp_port_mod ) );
  //assert( buffer != NULL );

  port_mod = ( struct ofp_port_mod * ) buffer->data;
  port_mod->port_no = htons( port_no );
  memcpy( port_mod->hw_addr, hw_addr, OFP_ETH_ALEN );
  port_mod->config = htonl( config );
  port_mod->mask = htonl( mask );
  port_mod->advertise = htonl( advertise );
  memset( port_mod->pad, 0, sizeof( port_mod->pad ) );

  return buffer;
}


static buffer *
cc_create_stats_request( const uint32_t transaction_id, const uint16_t type,
                      const uint16_t length, const uint16_t flags ) {
  buffer *buffer;
  struct ofp_stats_request *stats_request;

  //debug( "Creating a stats request ( xid = %#x, type = %#x, length = %u, flags = %#x ).",
         //transaction_id, type, length, flags );

  buffer = cc_create_header( transaction_id, OFPT_STATS_REQUEST, length );
  //assert( buffer != NULL );

  stats_request = ( struct ofp_stats_request * ) buffer->data;
  stats_request->type = htons( type );
  stats_request->flags = htons( flags );

  return buffer;
}


buffer *
cc_create_desc_stats_request( const uint32_t transaction_id, const uint16_t flags ) {
  //debug( "Creating a description stats request ( xid = %#x, flags = %#x ).",
         //transaction_id, flags );

  return cc_create_stats_request( transaction_id, OFPST_DESC,
                               sizeof( struct ofp_stats_request ), flags );
}


buffer *
cc_create_flow_stats_request( const uint32_t transaction_id, const uint16_t flags,
                           const struct ofp_match match, const uint8_t table_id,
                           const uint16_t out_port ) {
  char match_str[ 1024 ];
  uint16_t length;
  buffer *buffer;
  struct ofp_match m = match;
  struct ofp_flow_stats_request *flow_stats_request;

  /*
  // Because match_to_string() is costly, we check logging_level first.
  if ( get_logging_level() >= LOG_DEBUG ) {
    match_to_string( &m, match_str, sizeof( match_str ) );
    //debug( "Creating a flow stats request ( xid = %#x, flags = %#x, match = [%s], table_id = %u, out_port = %u ).",
           transaction_id, flags, match_str, table_id, out_port );
  }
  */

  length = ( uint16_t ) ( offsetof( struct ofp_stats_request, body )
                         + sizeof( struct ofp_flow_stats_request ) );
  buffer = cc_create_stats_request( transaction_id, OFPST_FLOW, length, flags );
  //assert( buffer != NULL );

  flow_stats_request = ( struct ofp_flow_stats_request * ) ( ( char * ) buffer->data
                       + offsetof( struct ofp_stats_request, body ) );
  hton_match( &flow_stats_request->match, &m );
  flow_stats_request->table_id = table_id;
  flow_stats_request->pad = 0;
  flow_stats_request->out_port = htons( out_port );

  return buffer;
}


buffer *
cc_create_aggregate_stats_request( const uint32_t transaction_id,
                                const uint16_t flags, const struct ofp_match match,
                                const uint8_t table_id, const uint16_t out_port ) {
  char match_str[ 1024 ];
  uint16_t length;
  buffer *buffer;
  struct ofp_match m = match;
  struct ofp_aggregate_stats_request *aggregate_stats_request;

  /*
  // Because match_to_string() is costly, we check logging_level first.
  if ( get_logging_level() >= LOG_DEBUG ) {
    match_to_string( &m, match_str, sizeof( match_str ) );
    //debug( "Creating an aggregate stats request ( xid = %#x, flags = %#x, match = [%s], table_id = %u, out_port = %u ).",
           transaction_id, flags, match_str, table_id, out_port );
  }
  */

  length = ( uint16_t ) ( offsetof( struct ofp_stats_request, body )
           + sizeof( struct ofp_aggregate_stats_request ) );
  buffer = cc_create_stats_request( transaction_id, OFPST_AGGREGATE, length, flags );
  //assert( buffer != NULL );

  aggregate_stats_request = ( struct ofp_aggregate_stats_request * ) ( ( char * ) buffer->data
                            + offsetof( struct ofp_stats_request, body ) );
  hton_match( &aggregate_stats_request->match, &m );
  aggregate_stats_request->table_id = table_id;
  aggregate_stats_request->pad = 0;
  aggregate_stats_request->out_port = htons( out_port );

  return buffer;
}


buffer *
cc_create_table_stats_request( const uint32_t transaction_id, const uint16_t flags ) {
  //debug( "Creating a table stats request ( xid = %#x, flags = %#x ).", transaction_id, flags );

  return cc_create_stats_request( transaction_id, OFPST_TABLE,
                               sizeof( struct ofp_stats_request ), flags );
}


buffer *
cc_create_port_stats_request( const uint32_t transaction_id, const uint16_t flags,
                           const uint16_t port_no ) {
  uint16_t length;
  buffer *buffer;
  struct ofp_port_stats_request *port_stats_request;

  //debug( "Creating a port stats request ( xid = %#x, flags = %#x, port_no = %u ).",
        // transaction_id, flags, port_no );

  length = ( uint16_t ) ( offsetof( struct ofp_stats_request, body )
           + sizeof( struct ofp_port_stats_request ) );
  buffer = cc_create_stats_request( transaction_id, OFPST_PORT, length, flags );
  //assert( buffer != NULL );

  port_stats_request = ( struct ofp_port_stats_request * ) ( ( char * ) buffer->data
                       + offsetof( struct ofp_stats_request, body ) );
  port_stats_request->port_no = htons( port_no );
  memset( &port_stats_request->pad, 0, sizeof( port_stats_request->pad ) );

  return buffer;
}


buffer *
cc_create_queue_stats_request( const uint32_t transaction_id, const uint16_t flags,
                            const uint16_t port_no, const uint32_t queue_id ) {
  uint16_t length;
  buffer *buffer;
  struct ofp_queue_stats_request *queue_stats_request;

  //debug( "Creating a queue stats request ( xid = %#x, flags = %#x, port_no = %u, queue_id = %u ).",
         //transaction_id, flags, port_no, queue_id );

  length = ( uint16_t ) ( offsetof( struct ofp_stats_request, body )
                        + sizeof( struct ofp_queue_stats_request ) );
  buffer = cc_create_stats_request( transaction_id, OFPST_QUEUE, length, flags );
  //assert( buffer != NULL );

  queue_stats_request = ( struct ofp_queue_stats_request * ) ( ( char * ) buffer->data
                        + offsetof( struct ofp_stats_request, body ) );
  queue_stats_request->port_no = htons( port_no );
  memset( &queue_stats_request->pad, 0, sizeof( queue_stats_request->pad ) );
  queue_stats_request->queue_id = htonl( queue_id );

  return buffer;
}


buffer *
cc_create_vendor_stats_request( const uint32_t transaction_id, const uint16_t flags,
                             const uint32_t vendor, const buffer *body ) {
  void *b;
  uint16_t length;
  uint16_t data_length = 0;
  uint32_t *v;
  buffer *buffer;

  if ( ( body != NULL ) && ( body->length > 0 ) ) {
    data_length = ( uint16_t ) body->length;
  }

  //debug( "Creating a vendor stats request ( xid = %#x, flags = %#x, vendor = %#x, data length = %u ).",
         //transaction_id, flags, vendor, data_length );

  length = ( uint16_t ) ( offsetof( struct ofp_stats_request, body ) + sizeof( uint32_t )
                        + data_length );
  buffer = cc_create_stats_request( transaction_id, OFPST_VENDOR, length, flags );
  //assert( buffer != NULL );

  v = ( uint32_t * ) ( ( char * ) buffer->data + offsetof( struct ofp_stats_request, body ) );
  *v = htonl( vendor );

  if ( data_length > 0 ) {
    b = ( void * ) ( ( char * ) buffer->data
                   + offsetof( struct ofp_stats_request, body ) + sizeof( uint32_t ) );

    memcpy( b, body->data, data_length );
  }

  return buffer;
}


static buffer *
cc_create_stats_reply( const uint32_t transaction_id, const uint16_t type,
                    const uint16_t length, const uint16_t flags ) {
  buffer *buffer;
  struct ofp_stats_reply *stats_reply;

  //debug( "Creating a stats reply ( xid = %#x, type = %#x, length = %u, flags = %#x ).",
         //transaction_id, type, length, flags );

  buffer = cc_create_header( transaction_id, OFPT_STATS_REPLY, length );
  //assert( buffer != NULL );

  stats_reply = ( struct ofp_stats_reply * ) buffer->data;
  stats_reply->type = htons( type );
  stats_reply->flags = htons( flags );

  return buffer;
}


buffer *
cc_create_desc_stats_reply( const uint32_t transaction_id, const uint16_t flags,
                         const char mfr_desc[ DESC_STR_LEN ],
                         const char hw_desc[ DESC_STR_LEN ],
                         const char sw_desc[ DESC_STR_LEN ],
                         const char serial_num[ SERIAL_NUM_LEN ],
                         const char dp_desc[ DESC_STR_LEN ] ) {
  uint16_t length;
  buffer *buffer;
  struct ofp_stats_reply *stats_reply;
  struct ofp_desc_stats *desc_stats;

  //debug( "Creating a description stats reply "
        // "( xid = %#x, flags = %#x, mfr_desc = %s, hw_desc = %s, sw_desc = %s, serial_num = %s, dp_desc = %s ).",
        // transaction_id, flags, mfr_desc, hw_desc, sw_desc, serial_num, dp_desc );

  length = ( uint16_t ) ( offsetof( struct ofp_stats_reply, body )
                        + sizeof( struct ofp_desc_stats ) );
  buffer = cc_create_stats_reply( transaction_id, OFPST_DESC, length, flags );
  //assert( buffer != NULL );

  stats_reply = ( struct ofp_stats_reply * ) buffer->data;
  desc_stats = ( struct ofp_desc_stats * ) stats_reply->body;
  memcpy( desc_stats->mfr_desc, mfr_desc, DESC_STR_LEN );
  memcpy( desc_stats->hw_desc, hw_desc, DESC_STR_LEN );
  memcpy( desc_stats->sw_desc, sw_desc, DESC_STR_LEN );
  memcpy( desc_stats->serial_num, serial_num, DESC_STR_LEN );
  memcpy( desc_stats->dp_desc, dp_desc, DESC_STR_LEN );

  return buffer;
}


buffer *
cc_create_flow_stats_reply( const uint32_t transaction_id, const uint16_t flags,
                         const list_element *flows_stats_head ) {
  int n_flows = 0;
  uint16_t length = 0;
  buffer *buffer;
  list_element *f = NULL;
  list_element *flow = NULL;
  struct ofp_stats_reply *stats_reply;
  struct ofp_flow_stats *fs, *flow_stats;

  //debug( "Creating a flow stats reply ( xid = %#x, flags = %#x ).", transaction_id, flags );

  if ( flows_stats_head != NULL ) {
    f = ( list_element * ) malloc( sizeof( list_element ) );
    memcpy( f, flows_stats_head, sizeof( list_element ) );
  }

  flow = f;
  while ( flow != NULL ) {
    flow_stats = flow->data;
    length = ( uint16_t ) ( length + flow_stats->length );
    n_flows++;
    flow = flow->next;
  }

  //debug( "# of flows = %d.", n_flows );

  length = ( uint16_t ) ( offsetof( struct ofp_stats_reply, body ) + length );

  buffer = cc_create_stats_reply( transaction_id, OFPST_FLOW, length, flags );
  //assert( buffer != NULL );

  stats_reply = ( struct ofp_stats_reply * ) buffer->data;
  flow_stats = ( struct ofp_flow_stats * ) stats_reply->body;

  flow = f;
  while ( flow != NULL ) {
    fs = ( struct ofp_flow_stats * ) flow->data;
    hton_flow_stats( flow_stats, fs );
    flow_stats = ( struct ofp_flow_stats * ) ( ( char * ) flow_stats + fs->length );
    flow = flow->next;
  }

  if ( f != NULL ) {
    free( f );
  }

  return buffer;
}


buffer *
cc_create_aggregate_stats_reply( const uint32_t transaction_id, const uint16_t flags,
                              const uint64_t packet_count, const uint64_t byte_count,
                              const uint32_t flow_count ) {
  uint16_t length;
  buffer *buffer;
  struct ofp_stats_reply *stats_reply;
  struct ofp_aggregate_stats_reply *aggregate_stats_reply;

  //debug( "Creating an aggregate stats reply "
        // "( xid = %#x, flags = %#x, packet_count = %" PRIu64 ", byte_count = %" PRIu64 ", flow_count = %u ).",
        // transaction_id, flags, packet_count, byte_count, flow_count );

  length = ( uint16_t ) ( offsetof( struct ofp_stats_reply, body )
                        + sizeof( struct ofp_aggregate_stats_reply ) );
  buffer = cc_create_stats_reply( transaction_id, OFPST_AGGREGATE, length, flags );
  //assert( buffer != NULL );

  stats_reply = ( struct ofp_stats_reply * ) buffer->data;
  aggregate_stats_reply = ( struct ofp_aggregate_stats_reply * ) stats_reply->body;
  aggregate_stats_reply->packet_count = htonll( packet_count );
  aggregate_stats_reply->byte_count = htonll( byte_count );
  aggregate_stats_reply->flow_count = htonl( flow_count );
  memset( &aggregate_stats_reply->pad, 0, sizeof( aggregate_stats_reply->pad ) );

  return buffer;
}


buffer *
cc_create_table_stats_reply( const uint32_t transaction_id, const uint16_t flags,
                          const list_element *table_stats_head ) {
  uint16_t length;
  uint16_t n_tables = 0;
  buffer *buffer;
  list_element *t = NULL;
  list_element *table = NULL;
  struct ofp_stats_reply *stats_reply;
  struct ofp_table_stats *ts, *table_stats;

  //debug( "Creating a table stats reply ( xid = %#x, flags = %#x ).", transaction_id, flags );

  if ( table_stats_head != NULL ) {
    t = ( list_element * ) malloc( sizeof( list_element ) );
    memcpy( t, table_stats_head, sizeof( list_element ) );
  }

  table = t;
  while ( table != NULL ) {
    n_tables++;
    table = table->next;
  }

  //debug( "# of tables = %u.", n_tables );

  length = ( uint16_t ) ( offsetof( struct ofp_stats_reply, body )
                        + sizeof( struct ofp_table_stats ) * n_tables );
  buffer = cc_create_stats_reply( transaction_id, OFPST_TABLE, length, flags );
  //assert( buffer != NULL );

  stats_reply = ( struct ofp_stats_reply * ) buffer->data;
  table_stats = ( struct ofp_table_stats * ) stats_reply->body;

  table = t;
  while ( table != NULL ) {
    ts = ( struct ofp_table_stats * ) table->data;
    hton_table_stats( table_stats, ts );
    table = table->next;
    table_stats++;
  }

  if ( t != NULL ) {
    free( t );
  }

  return buffer;
}


buffer *
cc_create_port_stats_reply( const uint32_t transaction_id, const uint16_t flags,
                         const list_element *port_stats_head ) {
  uint16_t length;
  uint16_t n_ports = 0;
  buffer *buffer;
  list_element *p = NULL;
  list_element *port = NULL;
  struct ofp_stats_reply *stats_reply;
  struct ofp_port_stats *ps, *port_stats;

  //debug( "Creating a port stats reply ( xid = %#x, flags = %#x ).", transaction_id, flags );

  if ( port_stats_head != NULL ) {
    p = ( list_element * ) malloc( sizeof( list_element ) );
    memcpy( p, port_stats_head, sizeof( list_element ) );
  }

  port = p;
  while ( port != NULL ) {
    n_ports++;
    port = port->next;
  }

  //debug( "# of ports = %u.", n_ports );

  length = ( uint16_t ) ( offsetof( struct ofp_stats_reply, body )
                          + sizeof( struct ofp_port_stats ) * n_ports );
  buffer = cc_create_stats_reply( transaction_id, OFPST_PORT, length, flags );
  //assert( buffer != NULL );

  stats_reply = ( struct ofp_stats_reply * ) buffer->data;
  port_stats = ( struct ofp_port_stats * ) stats_reply->body;

  port = p;
  while ( port != NULL ) {
    ps = ( struct ofp_port_stats * ) port->data;
    hton_port_stats( port_stats, ps );
    port = port->next;
    port_stats++;
  }

  if ( p != NULL ) {
    free( p );
  }

  return buffer;
}


buffer *
cc_create_queue_stats_reply( const uint32_t transaction_id, const uint16_t flags,
                          const list_element *queue_stats_head ) {
  uint16_t length;
  uint16_t n_queues = 0;
  buffer *buffer;
  list_element *q = NULL;
  list_element *queue = NULL;
  struct ofp_stats_reply *stats_reply;
  struct ofp_queue_stats *qs, *queue_stats;

  //debug( "Creating a queue stats reply ( xid = %#x, flags = %#x ).", transaction_id, flags );

  if ( queue_stats_head != NULL ) {
    q = ( list_element * ) malloc( sizeof( list_element ) );
    memcpy( q, queue_stats_head, sizeof( list_element ) );
  }

  queue = q;
  while ( queue != NULL ) {
    n_queues++;
    queue = queue->next;
  }

  //debug( "# of queues = %u.", n_queues );

  length = ( uint16_t ) ( offsetof( struct ofp_stats_reply, body )
                          + sizeof( struct ofp_queue_stats ) * n_queues );
  buffer = cc_create_stats_reply( transaction_id, OFPST_QUEUE, length, flags );
  //assert( buffer != NULL );

  stats_reply = ( struct ofp_stats_reply * ) buffer->data;
  queue_stats = ( struct ofp_queue_stats * ) stats_reply->body;

  queue = q;
  while ( queue != NULL ) {
    qs = ( struct ofp_queue_stats * ) queue->data;
    hton_queue_stats( queue_stats, qs );
    queue = queue->next;
    queue_stats++;
  }

  if ( q != NULL ) {
    free( q );
  }

  return buffer;
}


buffer *
cc_create_vendor_stats_reply( const uint32_t transaction_id, const uint16_t flags,
                           const uint32_t vendor, const buffer *body ) {
  void *b;
  uint16_t length;
  uint16_t data_length = 0;
  uint32_t *v;
  buffer *buffer;
  struct ofp_stats_reply *stats_reply;

  if ( ( body != NULL ) && ( body->length > 0 ) ) {
    data_length = ( uint16_t ) body->length;
  }

  //debug( "Creating a vendor stats reply ( xid = %#x, flags = %#x, vendor = %#x, data length = %u ).",
        // transaction_id, flags, vendor, data_length );

  length = ( uint16_t ) ( offsetof( struct ofp_stats_reply, body )
                        + sizeof( uint32_t ) + data_length );
  buffer = cc_create_stats_reply( transaction_id, OFPST_VENDOR, length, flags );
  //assert( buffer != NULL );

  stats_reply = ( struct ofp_stats_reply * ) buffer->data;
  v = ( uint32_t * ) stats_reply->body;
  *v = htonl( vendor );

  if ( data_length > 0 ) {
    b = ( void * ) ( ( char * ) v + sizeof( uint32_t ) );
    memcpy( b, body->data, data_length );
  }

  return buffer;
}


buffer *
cc_create_barrier_request( const uint32_t transaction_id ) {
  //debug( "Creating a barrier request ( xid = %#x ).", transaction_id );

  return cc_create_header( transaction_id, OFPT_BARRIER_REQUEST, sizeof( struct ofp_header ) );
}


buffer *
cc_create_barrier_reply( const uint32_t transaction_id ) {
  //debug( "Creating a barrier reply ( xid = %#x ).", transaction_id );

  return cc_create_header( transaction_id, OFPT_BARRIER_REPLY, sizeof( struct ofp_header ) );
}


buffer *
cc_create_queue_get_config_request( const uint32_t transaction_id, const uint16_t port ) {
  buffer *buffer;
  struct ofp_queue_get_config_request *queue_get_config_request;

  //debug( "Creating a queue get config request ( xid = %#x, port = %u ).", transaction_id, port );

  buffer = cc_create_header( transaction_id, OFPT_QUEUE_GET_CONFIG_REQUEST,
                          sizeof( struct ofp_queue_get_config_request ) );
  //assert( buffer != NULL );

  queue_get_config_request = ( struct ofp_queue_get_config_request * ) buffer->data;
  queue_get_config_request->port = htons( port );
  memset( queue_get_config_request->pad, 0, sizeof( queue_get_config_request->pad ) );

  return buffer;
}


buffer *
cc_create_queue_get_config_reply( const uint32_t transaction_id, const uint16_t port,
                               const list_element *queues ) {
  uint16_t length;
  uint16_t n_queues = 0;
  uint16_t queues_length = 0;
  buffer *buffer;
  list_element *q, *queue;
  struct ofp_queue_get_config_reply *queue_get_config_reply;
  struct ofp_packet_queue *pq, *packet_queue;

  //debug( "Creating a queue get config reply ( xid = %#x, port = %u ).", transaction_id, port );

#ifndef UNIT_TESTING
  //assert( queues != NULL );
#endif

  if ( queues != NULL ) {
    q = ( list_element * ) malloc( sizeof( list_element ) );
    memcpy( q, queues, sizeof( list_element ) );

    queue = q;
    while ( queue != NULL ) {
      packet_queue = ( struct ofp_packet_queue * ) queue->data;
      queues_length = ( uint16_t ) ( queues_length + packet_queue->len );
      n_queues++;
      queue = queue->next;
    }
  }

  //debug( "# of queues = %u.", n_queues );

  length = ( uint16_t ) ( offsetof( struct ofp_queue_get_config_reply, queues ) + queues_length );
  buffer = cc_create_header( transaction_id, OFPT_QUEUE_GET_CONFIG_REPLY, length );
  //assert( buffer != NULL );

  queue_get_config_reply = ( struct ofp_queue_get_config_reply * ) buffer->data;
  queue_get_config_reply->port = htons( port );
  memset( &queue_get_config_reply->pad, 0, sizeof( queue_get_config_reply->pad ) );
  packet_queue = ( struct ofp_packet_queue * ) queue_get_config_reply->queues;

  if ( n_queues ) {
    queue = q;
    while ( queue != NULL ) {
      pq = ( struct ofp_packet_queue * ) queue->data;

      hton_packet_queue( packet_queue, pq );

      packet_queue = ( struct ofp_packet_queue * ) ( ( char * ) packet_queue + pq->len );
      queue = queue->next;
    }

    free( q );
  }

  return buffer;
}

