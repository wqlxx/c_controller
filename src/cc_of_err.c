/*
 * cc_of_msg_err functions.
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

#include "cc_of_err.h"

static int
validate_header( const buffer *message, const uint8_t type,
                 const uint16_t min_length, const uint16_t max_length ) {
	ofp_header *header;

	assert( message != NULL );
  	if ( message->length < sizeof( struct ofp_header ) ) {
    	return ERROR_TOO_SHORT_MESSAGE;
  	}

  	header = ( struct ofp_header * ) message->data;
  	if ( header->version != OFP_VERSION ) {
    	return ERROR_UNSUPPORTED_VERSION;
  	}
  	if ( header->type > OFPT_QUEUE_GET_CONFIG_REPLY ) {
    	return ERROR_UNDEFINED_TYPE;
  	}
  	if ( header->type != type ) {
    	return ERROR_INVALID_TYPE;
  	}
  	if ( ntohs( header->length ) > max_length ) {
    	return ERROR_TOO_LONG_MESSAGE;
  	}
  	else if ( ntohs( header->length ) < min_length ) {
    	return ERROR_TOO_SHORT_MESSAGE;
  	}
  	if ( ntohs( header->length ) < message->length ) {
    	return ERROR_TOO_LONG_MESSAGE;
  	}
  	else if ( ntohs( header->length ) > message->length ) {
    	return ERROR_TOO_SHORT_MESSAGE;
  	}

  	if ( message->length > max_length ) {
    	return ERROR_TOO_LONG_MESSAGE;
  	}

  	return 0;
}


int
validate_hello( const buffer *message ) {
  assert( message != NULL );
  return validate_header( message, OFPT_HELLO, sizeof( struct ofp_header ), sizeof( struct ofp_header ) );
}


int
validate_error( const buffer *message ) {
  int ret;

  assert( message != NULL );

  ret = validate_header( message, OFPT_ERROR, sizeof( struct ofp_error_msg ), USHRT_MAX );
  if ( ret < 0 ) {
    return ret;
  }

  return 0;
}


int
validate_echo_request( const buffer *message ) {
  int ret;
  struct ofp_header *header;

  assert( message != NULL );

  ret = validate_header( message, OFPT_ECHO_REQUEST, sizeof( struct ofp_header ), USHRT_MAX );
  if ( ret < 0 ) {
    return ret;
  }

  header = ( struct ofp_header * ) message->data;
  if ( message->length != ntohs( header->length ) ) {
    return ERROR_INVALID_LENGTH;
  }

  return 0;
}


int
validate_echo_reply( const buffer *message ) {
  int ret;
  struct ofp_header *header;

  assert( message != NULL );

  ret = validate_header( message, OFPT_ECHO_REPLY, sizeof( struct ofp_header ), USHRT_MAX );
  if ( ret < 0 ) {
    return ret;
  }

  header = ( struct ofp_header * ) message->data;
  if ( message->length != ntohs( header->length ) ) {
    return ERROR_INVALID_LENGTH;
  }

  return 0;
}


int
validate_vendor( const buffer *message ) {
  int ret;
  struct ofp_vendor_header *vendor_header;

  assert( message != NULL );

  ret = validate_header( message, OFPT_VENDOR, sizeof( struct ofp_vendor_header ), USHRT_MAX );
  if ( ret < 0 ) {
    return ret;
  }

  vendor_header = ( struct ofp_vendor_header * ) message->data;
  if ( message->length != ntohs( vendor_header->header.length ) ) {
    return ERROR_INVALID_LENGTH;
  }

  return 0;
}


int
validate_features_request( const buffer *message ) {
  assert( message != NULL );
  return validate_header( message, OFPT_FEATURES_REQUEST, sizeof( struct ofp_header ),
                          sizeof( struct ofp_header ) );
}


static int
validate_phy_port_no( const uint16_t port_no ) {
  if ( ( port_no == 0 ) || ( ( port_no > OFPP_MAX ) && ( port_no < OFPP_IN_PORT ) ) ) {
    return ERROR_INVALID_PORT_NO;
  }

  return 0;
}


static int
validate_phy_port( struct ofp_phy_port *port ) {
  int ret;
  struct ofp_phy_port port_h;

  assert( port != NULL );

  ntoh_phy_port( &port_h, port );

  ret = validate_phy_port_no( port_h.port_no );
  if ( ret < 0 ) {
    return ret;
  }

  if ( ( port_h.config & ( uint32_t ) ~PORT_CONFIG ) != 0 ) {
    return ERROR_INVALID_PORT_CONFIG;
  }
  if ( ( port_h.state & ( uint32_t ) ~PORT_STATE ) != 0 ) {
    return ERROR_INVALID_PORT_STATE;
  }
  if ( ( port_h.curr & ( uint32_t ) ~PORT_FEATURES ) != 0
       || ( port_h.advertised & ( uint32_t ) ~PORT_FEATURES ) != 0
       || ( port_h.supported & ( uint32_t ) ~PORT_FEATURES ) != 0
       || ( port_h.peer & ( uint32_t ) ~PORT_FEATURES ) != 0 ) {
    return ERROR_INVALID_PORT_FEATURES;
  }

  return 0;
}


static int
validate_phy_ports( struct ofp_phy_port *ports, const int n_ports ) {
  int i;
  int ret;
  struct ofp_phy_port *port;

  assert( ports != NULL );
  assert( n_ports );

  port = ports;
  for ( i = 0; i < n_ports; i++ ) {
    ret = validate_phy_port( port );
    if ( ret < 0 ) {
      return ret;
    }
    port++;
  }

  return 0;
}


int
validate_features_reply( const buffer *message ) {
  void *p;
  int ret;
  int n_ports;
  uint16_t port_length;
  struct ofp_switch_features *switch_features;

  assert( message != NULL );

  ret = validate_header( message, OFPT_FEATURES_REPLY, sizeof( struct ofp_switch_features ), USHRT_MAX );
  if ( ret < 0 ) {
    return ret;
  }

  switch_features = ( struct ofp_switch_features * ) message->data;

  // switch_features->datapath_id
  // switch_features->n_buffers

  if ( switch_features->n_tables == 0 ) {
    return ERROR_NO_TABLE_AVAILABLE;
  }

  port_length = ( uint16_t ) ( ntohs( switch_features->header.length )
                             - sizeof( struct ofp_switch_features ) );
  if ( port_length % sizeof( struct ofp_phy_port ) != 0 ) {
    return ERROR_INVALID_LENGTH;
  }

  if ( port_length > 0 ) {
    p = ( void * ) ( ( char * ) message->data + offsetof( struct ofp_switch_features, ports ) );
    n_ports = port_length / sizeof( struct ofp_phy_port );

    ret = validate_phy_ports( p, n_ports );
    if ( ret < 0 ) {
      return ret;
    }
  }

  return 0;
}


int
validate_get_config_request( const buffer *message ) {
  assert( message != NULL );
  return validate_header( message, OFPT_GET_CONFIG_REQUEST, sizeof( struct ofp_header ),
                          sizeof( struct ofp_header ) );
}


static int
validate_switch_config( const buffer *message, const uint8_t type ) {
  int ret;
  struct ofp_switch_config *switch_config;

  assert( message != NULL );
  assert( ( type == OFPT_GET_CONFIG_REPLY ) || ( type == OFPT_SET_CONFIG ) );

  ret = validate_header( message, type, sizeof( struct ofp_switch_config ),
                         sizeof( struct ofp_switch_config ) );
  if ( ret < 0 ) {
    return ret;
  }

  switch_config = ( struct ofp_switch_config * ) message->data;
  if ( ntohs( switch_config->flags ) > OFPC_FRAG_MASK ) {
    return ERROR_INVALID_SWITCH_CONFIG;
  }

  // switch_config->miss_send_len

  return 0;
}


int
validate_get_config_reply( const buffer *message ) {
  assert( message != NULL );
  return validate_switch_config( message, OFPT_GET_CONFIG_REPLY );
}


int
validate_set_config( const buffer *message ) {
  assert( message != NULL );
  return validate_switch_config( message, OFPT_SET_CONFIG );
}


int
validate_packet_in( const buffer *message ) {
  int ret;
  uint16_t data_length;
  struct ofp_packet_in *packet_in;

  assert( message != NULL );

  ret = validate_header( message, OFPT_PACKET_IN, offsetof( struct ofp_packet_in, data ), USHRT_MAX );
  if ( ret < 0 ) {
    return ret;
  }

  packet_in = ( struct ofp_packet_in * ) message->data;

  // packet_in->buffer_id
  // packet_in->total_len
  // packet_in->in_port

  ret = validate_phy_port_no( ntohs( packet_in->in_port ) );
  if ( ret < 0 ) {
    return ret;
  }

  if ( packet_in->reason > OFPR_ACTION ) {
    return ERROR_INVALID_PACKET_IN_REASON;
  }

  data_length = ( uint16_t ) ( ntohs( packet_in->header.length ) - offsetof( struct ofp_packet_in, data ) );
  if ( data_length > 0 ) {
    // FIXME: it may be better to check if this is a valid Ethernet frame or not.
  }

  return 0;
}


static int
validate_wildcards( const uint32_t wildcards ) {
  if ( ( wildcards & ( uint32_t ) ~OFPFW_ALL ) != 0 ) {
    return ERROR_INVALID_WILDCARDS;
  }

  return 0;
}


static int
validate_vlan_vid( const uint16_t vid ) {
  if ( ( vid != USHRT_MAX ) && ( ( vid & ~VLAN_VID_MASK ) != 0 ) ) {
    return ERROR_INVALID_VLAN_VID;
  }

  return 0;
}


static int
validate_vlan_pcp( const uint8_t pcp ) {
  if ( ( pcp & ~VLAN_PCP_MASK ) != 0 ) {
    return ERROR_INVALID_VLAN_PCP;
  }

  return 0;
}


static int
validate_nw_tos( const uint8_t tos ) {
  if ( ( tos & ~NW_TOS_MASK ) != 0 ) {
    return ERROR_INVALID_NW_TOS;
  }

  return 0;
}


static int
validate_match( const struct ofp_match match ) {
  int ret;

  ret = validate_wildcards( match.wildcards );
  if ( ret < 0 ) {
    return ret;
  }

  ret = validate_vlan_vid( match.dl_vlan );
  if ( ret < 0 ) {
    return ret;
  }

  ret = validate_vlan_pcp( match.dl_vlan_pcp );
  if ( ret < 0 ) {
    return ret;
  }

  ret = validate_nw_tos( match.nw_tos );
  if ( ret < 0 ) {
    return ret;
  }

  return 0;
}


int
validate_flow_removed( const buffer *message ) {
  int ret;
  struct ofp_match match;
  struct ofp_flow_removed *flow_removed;

  assert( message != NULL );

  ret = validate_header( message, OFPT_FLOW_REMOVED, sizeof( struct ofp_flow_removed ),
                         sizeof( struct ofp_flow_removed ) );
  if ( ret < 0 ) {
    return ret;
  }

  flow_removed = ( struct ofp_flow_removed * ) message->data;

  ntoh_match( &match, &flow_removed->match );

  ret = validate_match( match );
  if ( ret < 0 ) {
    return ret;
  }

  // flow_removed->cookie

  if ( ( ( match.wildcards & OFPFW_ALL ) == 0 ) && ( ntohs( flow_removed->priority ) != USHRT_MAX ) ) {
    return ERROR_INVALID_FLOW_PRIORITY;
  }

  if ( flow_removed->reason > OFPRR_DELETE ) {
    return ERROR_INVALID_FLOW_REMOVED_REASON;
  }

  // flow_removed->duration_sec
  // flow_removed->duration_nsec
  // flow_removed->idle_timeout
  // flow_removed->packet_count
  // flow_removed->byte_count

  return 0;
}


int
validate_port_status( const buffer *message ) {
  int ret;
  struct ofp_port_status *port_status;

  assert( message != NULL );

  ret = validate_header( message, OFPT_PORT_STATUS, sizeof( struct ofp_port_status ),
                         sizeof( struct ofp_port_status ) );
  if ( ret < 0 ) {
    return ret;
  }

  port_status = ( struct ofp_port_status * ) message->data;
  if ( port_status->reason > OFPPR_MODIFY ) {
    return ERROR_INVALID_PORT_STATUS_REASON;
  }

  ret = validate_phy_port( &port_status->desc );
  if ( ret < 0 ) {
    return ret;
  }

  return 0;
}


int
validate_packet_out( const buffer *message ) {
  int ret;
  uint16_t data_length;
  struct ofp_packet_out *packet_out;

  assert( message != NULL );

  ret = validate_header( message, OFPT_PACKET_OUT, offsetof( struct ofp_packet_out, actions ),
                         USHRT_MAX );
  if ( ret < 0 ) {
    return ret;
  }

  packet_out = ( struct ofp_packet_out * ) message->data;

  ret = validate_phy_port_no( ntohs( packet_out->in_port ) );
  if ( ret < 0 ) {
    return ret;
  }

  if ( ntohs( packet_out->actions_len ) > 0 ) {
    ret = validate_actions( packet_out->actions, ntohs( packet_out->actions_len ) );
    if ( ret < 0 ) {
      return ret;
    }
  }

  data_length = ( uint16_t ) ( ntohs( packet_out->header.length )
                             - offsetof( struct ofp_packet_out, actions )
                             - ntohs( packet_out->actions_len ) );

  if ( data_length > 0 ) {
    // FIXME: it may be better to check if this is a valid Ethernet frame or not.
  }

  return 0;
}


int
validate_flow_mod( const buffer *message ) {
  int ret;
  uint16_t actions_length;
  struct ofp_match match;
  struct ofp_flow_mod *flow_mod;

  assert( message != NULL );

  ret = validate_header( message, OFPT_FLOW_MOD, offsetof( struct ofp_flow_mod, actions ),
                         USHRT_MAX );
  if ( ret < 0 ) {
    return ret;
  }

  flow_mod = ( struct ofp_flow_mod * ) message->data;

  ntoh_match( &match, &flow_mod->match );

  ret = validate_match( match );
  if ( ret < 0 ) {
    return ret;
  }

  // flow_mod->cookie

  if ( ntohs( flow_mod->command ) > OFPFC_DELETE_STRICT ) {
    return ERROR_UNDEFINED_FLOW_MOD_COMMAND;
  }

  // flow_mod->idle_timeout
  // flow_mod->hard_timeout

  if ( ( ( match.wildcards & OFPFW_ALL ) == 0 ) && ( ntohs( flow_mod->priority ) != USHRT_MAX ) ) {
    return ERROR_INVALID_FLOW_PRIORITY;
  }

  // flow_mod->buffer_id

  if ( ( ntohs( flow_mod->command ) == OFPFC_DELETE )
       || ( ntohs( flow_mod->command ) == OFPFC_DELETE_STRICT ) ) {
    if ( ntohs( flow_mod->out_port ) != OFPP_NONE ) {
      ret = validate_phy_port_no( ntohs( flow_mod->out_port ) );
      if ( ret < 0 ) {
        return ret;
      }
    }
  }

  if ( ( ntohs( flow_mod->flags ) & ~FLOW_MOD_FLAGS ) != 0 ) {
    return ERROR_INVALID_FLOW_MOD_FLAGS;
  }

  actions_length = ( uint16_t ) ( ntohs( flow_mod->header.length )
                                - offsetof( struct ofp_flow_mod, actions ) );

  if ( actions_length > 0 ) {
    ret = validate_actions( flow_mod->actions, actions_length );
    if ( ret < 0 ) {
      return ret;
    }

  }

  return 0;
}


int
validate_port_mod( const buffer *message ) {
  int ret;
  struct ofp_port_mod *port_mod;

  assert( message != NULL );

  ret = validate_header( message, OFPT_PORT_MOD, sizeof( struct ofp_port_mod ),
                         sizeof( struct ofp_port_mod ) );
  if ( ret < 0 ) {
    return ret;
  }

  port_mod = ( struct ofp_port_mod * ) message->data;

  ret = validate_phy_port_no( ntohs( port_mod->port_no ) );
  if ( ret < 0 ) {
    return ret;
  }
  if ( ( ntohs( port_mod->port_no ) > OFPP_MAX ) && ( ntohs( port_mod->port_no ) != OFPP_LOCAL ) ) {
    return ERROR_INVALID_PORT_NO;
  }

  // port_mod->hw_addr

  if ( ( ntohl( port_mod->config ) & ( uint32_t ) ~PORT_CONFIG ) != 0 ) {
    return ERROR_INVALID_PORT_CONFIG;
  }
  if ( ( ntohl( port_mod->mask ) & ( uint32_t ) ~PORT_CONFIG ) != 0 ) {
    return ERROR_INVALID_PORT_MASK;
  }
  if ( ( ntohl( port_mod->advertise ) & ( uint32_t ) ~PORT_CONFIG ) != 0 ) {
    return ERROR_INVALID_PORT_FEATURES;
  }

  return 0;
}


int
validate_desc_stats_request( const buffer *message ) {
  int ret;
  struct ofp_stats_request *stats_request;

  assert( message != NULL );

  ret = validate_header( message, OFPT_STATS_REQUEST, sizeof( struct ofp_stats_request ),
                         sizeof( struct ofp_stats_request ) );
  if ( ret < 0 ) {
    return ret;
  }

  stats_request = ( struct ofp_stats_request * ) message->data;

  if ( ntohs( stats_request->type ) != OFPST_DESC ) {
    return ERROR_INVALID_STATS_TYPE;
  }
  if ( ntohs( stats_request->flags ) != 0 ) {
    return ERROR_INVALID_STATS_REQUEST_FLAGS;
  }

  return 0;
}


int
validate_flow_stats_request( const buffer *message ) {
  int ret;
  struct ofp_match match;
  struct ofp_stats_request *stats_request;
  struct ofp_flow_stats_request *flow_stats_request;

  assert( message != NULL );

  ret = validate_header( message, OFPT_STATS_REQUEST,
                         offsetof( struct ofp_stats_request, body )
                         + sizeof( struct ofp_flow_stats_request ),
                         offsetof( struct ofp_stats_request, body )
                         + sizeof( struct ofp_flow_stats_request ) );
  if ( ret < 0 ) {
    return ret;
  }

  stats_request = ( struct ofp_stats_request * ) message->data;

  if ( ntohs( stats_request->type ) != OFPST_FLOW ) {
    return ERROR_INVALID_STATS_TYPE;
  }

  if ( ntohs( stats_request->flags ) != 0 ) {
    return ERROR_INVALID_STATS_REQUEST_FLAGS;
  }

  flow_stats_request = ( struct ofp_flow_stats_request * ) stats_request->body;
  ntoh_match( &match, &flow_stats_request->match );

  ret = validate_match( match );
  if ( ret < 0 ) {
    return ret;
  }

  // flow_stats_request->table_id

  ret = validate_phy_port_no( ntohs( flow_stats_request->out_port ) );
  if ( ret < 0 ) {
    return ret;
  }

  return 0;
}


int
validate_aggregate_stats_request( const buffer *message ) {
  int ret;
  struct ofp_match match;
  struct ofp_stats_request *stats_request;
  struct ofp_aggregate_stats_request *aggregate_stats_request;

  assert( message != NULL );

  ret = validate_header( message, OFPT_STATS_REQUEST,
                         offsetof( struct ofp_stats_request, body )
                         + sizeof( struct ofp_aggregate_stats_request ),
                         offsetof( struct ofp_stats_request, body )
                         + sizeof( struct ofp_aggregate_stats_request ) );
  if ( ret < 0 ) {
    return ret;
  }

  stats_request = ( struct ofp_stats_request * ) message->data;

  if ( ntohs( stats_request->type ) != OFPST_AGGREGATE ) {
    return ERROR_INVALID_STATS_TYPE;
  }
  if ( ntohs( stats_request->flags ) != 0 ) {
    return ERROR_INVALID_STATS_REQUEST_FLAGS;
  }

  aggregate_stats_request = ( struct ofp_aggregate_stats_request * ) stats_request->body;
  ntoh_match( &match, &aggregate_stats_request->match );

  ret = validate_match( match );
  if ( ret < 0 ) {
    return ret;
  }

  // aggregate_stats_request->table_id

  ret = validate_phy_port_no( ntohs( aggregate_stats_request->out_port ) );
  if ( ret < 0 ) {
    return ret;
  }

  return 0;
}


int
validate_table_stats_request( const buffer *message ) {
  int ret;
  struct ofp_stats_request *stats_request;

  assert( message != NULL );

  ret = validate_header( message, OFPT_STATS_REQUEST, offsetof( struct ofp_stats_request, body ),
                         offsetof( struct ofp_stats_request, body ) );
  if ( ret < 0 ) {
    return ret;
  }

  stats_request = ( struct ofp_stats_request * ) message->data;

  if ( ntohs( stats_request->type ) != OFPST_TABLE ) {
    return ERROR_INVALID_STATS_TYPE;
  }
  if ( ntohs( stats_request->flags ) != 0 ) {
    return ERROR_INVALID_STATS_REQUEST_FLAGS;
  }

  return 0;
}


int
validate_port_stats_request( const buffer *message ) {
  int ret;
  struct ofp_stats_request *stats_request;
  struct ofp_port_stats_request *port_stats_request;

  assert( message != NULL );

  ret = validate_header( message, OFPT_STATS_REQUEST,
                         offsetof( struct ofp_stats_request, body )
                         + sizeof( struct ofp_port_stats_request ),
                         offsetof( struct ofp_stats_request, body )
                         + sizeof( struct ofp_port_stats_request ) );
  if ( ret < 0 ) {
    return ret;
  }

  stats_request = ( struct ofp_stats_request * ) message->data;

  if ( ntohs( stats_request->type ) != OFPST_PORT ) {
    return ERROR_INVALID_STATS_TYPE;
  }
  if ( ntohs( stats_request->flags ) != 0 ) {
    return ERROR_INVALID_STATS_REQUEST_FLAGS;
  }

  port_stats_request = ( struct ofp_port_stats_request * ) stats_request->body;

  ret = validate_phy_port_no( ntohs( port_stats_request->port_no ) );
  if ( ret < 0 ) {
    return ret;
  }

  if ( ntohs( port_stats_request->port_no ) > OFPP_MAX
       && ntohs( port_stats_request->port_no ) != OFPP_NONE
       && ntohs( port_stats_request->port_no ) != OFPP_LOCAL ) {
    return ERROR_INVALID_PORT_NO;
  }

  return 0;
}


int
validate_queue_stats_request( const buffer *message ) {
  int ret;
  struct ofp_stats_request *stats_request;
  struct ofp_queue_stats_request *queue_stats_request;

  assert( message != NULL );

  ret = validate_header( message, OFPT_STATS_REQUEST,
                         offsetof( struct ofp_stats_request, body )
                         + sizeof( struct ofp_queue_stats_request ),
                         offsetof( struct ofp_stats_request, body )
                         + sizeof( struct ofp_queue_stats_request ) );
  if ( ret < 0 ) {
    return ret;
  }

  stats_request = ( struct ofp_stats_request * ) message->data;

  if ( ntohs( stats_request->type ) != OFPST_QUEUE ) {
    return ERROR_INVALID_STATS_TYPE;
  }
  if ( ntohs( stats_request->flags ) != 0 ) {
    return ERROR_INVALID_STATS_REQUEST_FLAGS;
  }

  queue_stats_request = ( struct ofp_queue_stats_request * ) stats_request->body;

  ret = validate_phy_port_no( ntohs( queue_stats_request->port_no ) );
  if ( ret < 0 ) {
    return ret;
  }

  // queue_stats_request->queue_id
  return 0;
}


int
validate_vendor_stats_request( const buffer *message ) {
  int ret;
  struct ofp_stats_request *stats_request;

  assert( message != NULL );

  ret = validate_header( message, OFPT_STATS_REQUEST,
                         offsetof( struct ofp_stats_request, body ) + sizeof( uint32_t ),
                         USHRT_MAX );
  if ( ret < 0 ) {
    return ret;
  }

  stats_request = ( struct ofp_stats_request * ) message->data;

  if ( ntohs( stats_request->type ) != OFPST_VENDOR ) {
    return ERROR_INVALID_STATS_TYPE;
  }
  if ( ntohs( stats_request->flags ) != 0 ) {
    return ERROR_INVALID_STATS_REQUEST_FLAGS;
  }

  // vendor_id
  return 0;
}


int
validate_stats_request( const buffer *message ) {
  struct ofp_stats_request *request;

  assert( message != NULL );

  request = ( struct ofp_stats_request * ) message->data;

  // TODO: if ( request->header.type != OFPT_STATS_REQUEST ) { ... }

  switch ( ntohs( request->type ) ) {
  case OFPST_DESC:
    return validate_desc_stats_request( message );
  case OFPST_FLOW:
    return validate_flow_stats_request( message );
  case OFPST_AGGREGATE:
    return validate_aggregate_stats_request( message );
  case OFPST_TABLE:
    return validate_table_stats_request( message );
  case OFPST_PORT:
    return validate_port_stats_request( message );
  case OFPST_QUEUE:
    return validate_queue_stats_request( message );
  case OFPST_VENDOR:
    return validate_vendor_stats_request( message );
  default:
    break;
  }

  return ERROR_UNSUPPORTED_STATS_TYPE;
}


int
validate_desc_stats_reply( const buffer *message ) {
  int ret;
  struct ofp_stats_reply *stats_reply;

  assert( message != NULL );

  ret = validate_header( message, OFPT_STATS_REPLY,
                         offsetof( struct ofp_stats_reply, body ),
                         offsetof( struct ofp_stats_reply, body ) + sizeof( struct ofp_desc_stats ) );
  if ( ret < 0 ) {
    return ret;
  }

  stats_reply = ( struct ofp_stats_reply * ) message->data;
  if ( ntohs( stats_reply->flags ) != 0 ) {
    return ERROR_INVALID_STATS_REPLY_FLAGS;
  }

  return 0;
}


int
validate_flow_stats_reply( const buffer *message ) {
  int ret;
  uint16_t offset;
  uint16_t flow_length;
  uint16_t actions_length;
  struct ofp_stats_reply *stats_reply;
  struct ofp_flow_stats *flow_stats;
  struct ofp_action_header *actions_head;
  struct ofp_match match;

  assert( message != NULL );

  ret = validate_header( message, OFPT_STATS_REPLY, offsetof( struct ofp_stats_reply, body ),
                         USHRT_MAX );
  if ( ret < 0 ) {
    return ret;
  }

  stats_reply = ( struct ofp_stats_reply * ) message->data;
  if ( ( ntohs( stats_reply->flags ) & ~OFPSF_REPLY_MORE ) != 0 ) {
    return ERROR_INVALID_STATS_REPLY_FLAGS;
  }

  flow_length = ( uint16_t ) ( ntohs( stats_reply->header.length )
                              - offsetof( struct ofp_stats_reply, body ) );
  offset = offsetof( struct ofp_stats_reply, body );
  flow_stats = ( struct ofp_flow_stats * ) ( ( char * ) message->data + offset );

  while ( flow_length > 0 ) {
    // flow_stats->length
    // flow_stats->table_id

    ntoh_match( &match, &flow_stats->match );

    ret = validate_match( match );
    if ( ret < 0 ) {
      return ret;
    }

    // flow_stats->duration_sec
    // flow_stats->duration_nsec

    if ( ( ( match.wildcards & OFPFW_ALL ) == 0 ) && ( ntohs( flow_stats->priority ) < USHRT_MAX ) ) {
      return ERROR_INVALID_FLOW_PRIORITY;
    }

    // flow_stats->idle_timeout
    // flow_stats->hard_timeout
    // flow_stats->cookie
    // flow_stats->packet_count
    // flow_stats->byte_count

    actions_length = ( uint16_t ) ( ntohs( flow_stats->length )
                                   - offsetof( struct ofp_flow_stats, actions ) );

    if ( actions_length > 0 ) {
      actions_head = ( struct ofp_action_header * ) ( ( char * ) flow_stats
                                                    + offsetof( struct ofp_flow_stats, actions ) );

      ret = validate_actions( actions_head, actions_length );
      if ( ret < 0 ) {
        return ret;
      }
    }

    flow_length = ( uint16_t ) ( flow_length - ntohs( flow_stats->length ) );
    flow_stats = ( struct ofp_flow_stats * ) ( ( char * ) flow_stats + ntohs( flow_stats->length ) );
  }

  return 0;
}


int
validate_aggregate_stats_reply( const buffer *message ) {
  int ret;
  struct ofp_stats_reply *stats_reply;

  assert( message != NULL );

  ret = validate_header( message, OFPT_STATS_REPLY,
                         offsetof( struct ofp_stats_reply, body ),
                         offsetof( struct ofp_stats_reply, body ) + sizeof( struct ofp_aggregate_stats_reply ) );
  if ( ret < 0 ) {
    return ret;
  }

  stats_reply = ( struct ofp_stats_reply * ) message->data;
  if ( ntohs( stats_reply->flags ) != 0 ) {
    return ERROR_INVALID_STATS_REPLY_FLAGS;
  }

  // uint16_t offset = offsetof( struct ofp_stats_reply, body );
  // struct ofp_aggregate_stats_reply *aggregate_stats = ( struct ofp_aggregate_stats_reply * ) ( ( char * ) message->data + offset );

  // aggregate_stats->packet_count
  // aggregate_stats->byte_count
  // aggregate_stats->flow_count

  return 0;
}


int
validate_table_stats_reply( const buffer *message ) {
  int i;
  int ret;
  uint16_t tables_length;
  uint16_t n_tables;
  uint16_t offset;
  struct ofp_stats_reply *stats_reply;
  struct ofp_table_stats *table_stats;

  assert( message != NULL );

  ret = validate_header( message, OFPT_STATS_REPLY,
                         offsetof( struct ofp_stats_reply, body ),
                         USHRT_MAX );
  if ( ret < 0 ) {
    return ret;
  }

  stats_reply = ( struct ofp_stats_reply * ) message->data;
  if ( ( ntohs( stats_reply->flags ) & ~OFPSF_REPLY_MORE ) != 0 ) {
    return ERROR_INVALID_STATS_REPLY_FLAGS;
  }

  tables_length = ( uint16_t ) ( ntohs( stats_reply->header.length )
                                - offsetof( struct ofp_stats_reply, body ) );
  if ( tables_length % sizeof( struct ofp_table_stats ) != 0 ) {
    return ERROR_INVALID_LENGTH;
  }

  offset = offsetof( struct ofp_stats_reply, body );
  table_stats = ( struct ofp_table_stats * ) ( ( char * ) message->data + offset );

  n_tables = tables_length / sizeof( struct ofp_table_stats );

  for ( i = 0; i < n_tables; i++ ) {
    // table_stats->table_id

    ret = validate_wildcards( ntohl( table_stats->wildcards ) );
    if ( ret < 0 ) {
      return ret;
    }

    // table_stats->max_entries
    // table_stats->active_count
    // table_stats->lookup_count
    // table_stats->matched_count

    table_stats++;
  }

  return 0;
}


int
validate_port_stats_reply( const buffer *message ) {
  int i;
  int ret;
  uint16_t ports_length;
  uint16_t n_ports;
  uint16_t offset;
  struct ofp_stats_reply *stats_reply;
  struct ofp_port_stats *port_stats;

  assert( message != NULL );

  ret = validate_header( message, OFPT_STATS_REPLY,
                         offsetof( struct ofp_stats_reply, body ),
                         USHRT_MAX );
  if ( ret < 0 ) {
    return ret;
  }

  stats_reply = ( struct ofp_stats_reply * ) message->data;
  if ( ( ntohs( stats_reply->flags ) & ~OFPSF_REPLY_MORE ) != 0 ) {
    return ERROR_INVALID_STATS_REPLY_FLAGS;
  }

  ports_length = ( uint16_t ) ( ntohs( stats_reply->header.length )
                              - offsetof( struct ofp_stats_reply, body ) );
  if ( ports_length % sizeof( struct ofp_port_stats ) != 0 ) {
    return ERROR_INVALID_LENGTH;
  }

  offset = offsetof( struct ofp_stats_reply, body );
  port_stats = ( struct ofp_port_stats * ) ( ( char * ) message->data + offset );

  n_ports = ports_length / sizeof( struct ofp_port_stats );
  for ( i = 0; i < n_ports; i++ ) {
    ret = validate_phy_port_no( ntohs( port_stats->port_no ) );

    if ( ret < 0 ) {
      return ret;
    }

    // port_stats->rx_packets
    // port_stats->tx_packets
    // port_stats->rx_bytes
    // port_stats->tx_bytes
    // port_stats->rx_dropped
    // port_stats->tx_dropped
    // port_stats->rx_errors
    // port_stats->tx_errors
    // port_stats->rx_frame_err
    // port_stats->rx_over_err
    // port_stats->rx_crc_err
    // port_stats->collisions

    port_stats++;
  }

  return 0;
}


int
validate_queue_stats_reply( const buffer *message ) {
  int i;
  int ret;
  uint16_t queues_length;
  uint16_t n_queues;
  uint16_t offset;
  struct ofp_stats_reply *stats_reply;
  struct ofp_queue_stats *queue_stats;

  assert( message != NULL );

  ret = validate_header( message, OFPT_STATS_REPLY,
                         offsetof( struct ofp_stats_reply, body ),
                         USHRT_MAX );
  if ( ret < 0 ) {
    return ret;
  }

  stats_reply = ( struct ofp_stats_reply * ) message->data;
  if ( ( ntohs( stats_reply->flags ) & ~OFPSF_REPLY_MORE ) != 0 ) {
    return ERROR_INVALID_STATS_REPLY_FLAGS;
  }

  queues_length = ( uint16_t ) ( ntohs( stats_reply->header.length )
                                 - offsetof( struct ofp_stats_reply, body ) );
  if ( queues_length % sizeof( struct ofp_queue_stats ) != 0 ) {
    return ERROR_INVALID_LENGTH;
  }

  offset = offsetof( struct ofp_stats_reply, body );
  queue_stats = ( struct ofp_queue_stats * ) ( ( char * ) message->data + offset );

  n_queues = queues_length / sizeof( struct ofp_queue_stats );
  for ( i = 0; i < n_queues; i++ ) {
    ret = validate_phy_port_no( ntohs( queue_stats->port_no ) );
    if ( ret < 0 ) {
      return ret;
    }

    // queue_stats->queue_id
    // queue_stats->tx_bytes
    // queue_stats->tx_packets
    // queue_stats->tx_errors

    queue_stats++;
  }

  return 0;
}


int
validate_vendor_stats_reply( const buffer *message ) {
  void *body;
  int ret;
  uint16_t body_length;
  uint16_t offset;
  struct ofp_stats_reply *stats_reply;

  assert( message != NULL );

  ret = validate_header( message, OFPT_STATS_REPLY,
                         offsetof( struct ofp_stats_reply, body ),
                         USHRT_MAX );
  if ( ret < 0 ) {
    return ret;
  }

  stats_reply = ( struct ofp_stats_reply * ) message->data;

  if ( ( ntohs( stats_reply->flags ) & ~OFPSF_REPLY_MORE ) != 0 ) {
    return ERROR_INVALID_STATS_REPLY_FLAGS;
  }

  body_length = ( uint16_t ) ( ntohs( stats_reply->header.length )
                             - offsetof( struct ofp_stats_reply, body ) );

  offset = offsetof( struct ofp_stats_reply, body );
  body = ( void * ) ( ( char * ) message->data + offset );
  if ( ( body_length > 0 ) && ( body != NULL ) ) {
    // FIXME: validate body here
  }

  return 0;
}


int
validate_stats_reply( const buffer *message ) {
  struct ofp_stats_reply *reply;

  assert( message != NULL );
  assert( message->data != NULL );

  reply = ( struct ofp_stats_reply * ) message->data;

  // TODO: if ( reply->header.type != OFPT_STATS_REPLY ) { ... }

  switch ( ntohs( reply->type ) ) {
  case OFPST_DESC:
    return validate_desc_stats_reply( message );
  case OFPST_FLOW:
    return validate_flow_stats_reply( message );
  case OFPST_AGGREGATE:
    return validate_aggregate_stats_reply( message );
  case OFPST_TABLE:
    return validate_table_stats_reply( message );
  case OFPST_PORT:
    return validate_port_stats_reply( message );
  case OFPST_QUEUE:
    return validate_queue_stats_reply( message );
  case OFPST_VENDOR:
    return validate_vendor_stats_reply( message );
  default:
    break;
  }

  return ERROR_UNSUPPORTED_STATS_TYPE;
}


int
validate_barrier_request( const buffer *message ) {
  return validate_header( message, OFPT_BARRIER_REQUEST, sizeof( struct ofp_header ),
                          sizeof( struct ofp_header ) );
}


int
validate_barrier_reply( const buffer *message ) {
  return validate_header( message, OFPT_BARRIER_REPLY, sizeof( struct ofp_header ),
                          sizeof( struct ofp_header ) );
}


int
validate_queue_get_config_request( const buffer *message ) {
  int ret;
  struct ofp_queue_get_config_request *queue_get_config_request;

  ret = validate_header( message, OFPT_QUEUE_GET_CONFIG_REQUEST,
                         sizeof( struct ofp_queue_get_config_request ),
                         sizeof( struct ofp_queue_get_config_request ) );
  if ( ret < 0 ) {
    return ret;
  }

  queue_get_config_request = ( struct ofp_queue_get_config_request * ) message->data;

  ret = validate_phy_port_no( ntohs( queue_get_config_request->port ) );
  if ( ret < 0 ) {
    return ret;
  }

  return 0;
}


static int
validate_queue_property( const struct ofp_queue_prop_header *property ) {
  uint16_t property_length = ntohs( property->len );

  if ( property_length < sizeof( struct ofp_queue_prop_header ) ) {
    return ERROR_TOO_SHORT_QUEUE_PROPERTY;
  }

  switch ( ntohs( property->property ) ) {
  case OFPQT_NONE:
    if ( property_length < sizeof( struct ofp_queue_prop_header ) ) {
      return ERROR_TOO_SHORT_QUEUE_PROPERTY;
    }
    else if ( property_length > sizeof( struct ofp_queue_prop_header ) ) {
      return ERROR_TOO_LONG_QUEUE_PROPERTY;
    }
    break;
  case OFPQT_MIN_RATE:
    if ( property_length < sizeof( struct ofp_queue_prop_min_rate ) ) {
      return ERROR_TOO_SHORT_QUEUE_PROPERTY;
    }
    else if ( property_length > sizeof( struct ofp_queue_prop_min_rate ) ) {
      return ERROR_TOO_LONG_QUEUE_PROPERTY;
    }
    break;
  default:
    return ERROR_UNDEFINED_QUEUE_PROPERTY;
  }

  return 0;
}


static int
validate_queue_properties( struct ofp_queue_prop_header *prop_head,
                           const uint16_t properties_length ) {
  int ret;
  uint16_t offset = 0;
  struct ofp_queue_prop_header *property;

  property = prop_head;
  while ( offset < properties_length ) {
    ret = validate_queue_property( property );
    if ( ret < 0 ) {
      return ret;
    }

    offset = ( uint16_t ) ( offset + ntohs( property->len ) );
    property = ( struct ofp_queue_prop_header * ) ( ( char * ) prop_head + offset );
  }

  return 0;
}


static int
validate_packet_queue( struct ofp_packet_queue *queue ) {
  int ret;
  uint16_t properties_length;
  struct ofp_queue_prop_header *prop_head;

  assert( queue != NULL );

  // queue->queue_id

  if ( ntohs( queue->len ) < ( offsetof( struct ofp_packet_queue, properties )
                             + sizeof( struct ofp_queue_prop_header ) ) ) {
    return ERROR_TOO_SHORT_QUEUE_DESCRIPTION;
  }

  prop_head =  ( struct ofp_queue_prop_header * ) ( ( char * ) queue
               + offsetof( struct ofp_packet_queue, properties ) );
  properties_length = ( uint16_t ) ( ntohs( queue->len )
                                   - offsetof( struct ofp_packet_queue, properties ) );

  ret = validate_queue_properties( prop_head, properties_length );
  if ( ret < 0 ) {
    return ret;
  }

  return 0;
}


static int
validate_packet_queues( struct ofp_packet_queue *queue_head, const int n_queues ) {
  int i;
  int ret;
  struct ofp_packet_queue *queue;

  assert( queue_head != NULL );

  queue = queue_head;
  for ( i = 0; i < n_queues; i++ ) {
    ret = validate_packet_queue( queue );
    if ( ret < 0 ) {
      return ret;
    }
    queue = ( struct ofp_packet_queue * ) ( ( char * ) queue + ntohs( queue->len ) );
  }

  return 0;
}


int
validate_queue_get_config_reply( const buffer *message ) {
  int ret;
  int n_queues = 0;
  uint16_t queues_length;
  struct ofp_queue_get_config_reply *queue_get_config_reply;
  struct ofp_packet_queue *queue_head, *queue;

  assert( message != NULL );

  ret = validate_header( message, OFPT_QUEUE_GET_CONFIG_REPLY,
                         sizeof( struct ofp_queue_get_config_reply ) + sizeof( struct ofp_packet_queue ),
                         USHRT_MAX );
  if ( ret < 0 ) {
    return ret;
  }

  queue_get_config_reply = ( struct ofp_queue_get_config_reply * ) message->data;

  ret = validate_phy_port_no( ntohs( queue_get_config_reply->port ) );
  if ( ret < 0 ) {
    return ret;
  }

  queues_length = ( uint16_t ) ( ntohs( queue_get_config_reply->header.length )
                 - offsetof( struct ofp_queue_get_config_reply, queues ) );

  queue_head = ( struct ofp_packet_queue * ) ( ( char * ) message->data
               + offsetof( struct ofp_queue_get_config_reply, queues ) );

  queue = queue_head;
  while ( queues_length > offsetof( struct ofp_packet_queue, properties ) ) {
    queues_length = ( uint16_t ) ( queues_length - ntohs( queue->len ) );
    queue = ( struct ofp_packet_queue * ) ( ( char * ) queue + ntohs( queue->len ) );
    n_queues++;
  }

  if ( queues_length != 0 ) {
    return ERROR_INVALID_LENGTH;
  }

  if ( n_queues > 0 ) {
    ret = validate_packet_queues( queue_head, n_queues );
    if ( ret < 0 ) {
      return ret;
    }
  }

  return 0;
}


static int
validate_action( struct ofp_action_header *action ) {
  if ( ntohs( action->len ) < sizeof( struct ofp_action_header ) ) {
    return ERROR_TOO_SHORT_ACTION;
  }

  switch ( ntohs( action->type ) ) {
  case OFPAT_OUTPUT:
    return validate_action_output( ( struct ofp_action_output * ) action );
  case OFPAT_SET_VLAN_VID:
    return validate_action_set_vlan_vid( ( struct ofp_action_vlan_vid * ) action );
  case OFPAT_SET_VLAN_PCP:
    return validate_action_set_vlan_pcp( ( struct ofp_action_vlan_pcp * ) action );
  case OFPAT_STRIP_VLAN:
    return validate_action_strip_vlan( ( struct ofp_action_header * ) action );
  case OFPAT_SET_DL_SRC:
    return validate_action_set_dl_src( ( struct ofp_action_dl_addr * ) action );
  case OFPAT_SET_DL_DST:
    return validate_action_set_dl_dst( ( struct ofp_action_dl_addr * ) action );
  case OFPAT_SET_NW_SRC:
    return validate_action_set_nw_src( ( struct ofp_action_nw_addr * ) action );
  case OFPAT_SET_NW_DST:
    return validate_action_set_nw_dst( ( struct ofp_action_nw_addr * ) action );
  case OFPAT_SET_NW_TOS:
    return validate_action_set_nw_tos( ( struct ofp_action_nw_tos * ) action );
  case OFPAT_SET_TP_SRC:
    return validate_action_set_tp_src( ( struct ofp_action_tp_port * ) action );
  case OFPAT_SET_TP_DST:
    return validate_action_set_tp_dst( ( struct ofp_action_tp_port * ) action );
  case OFPAT_ENQUEUE:
    return validate_action_enqueue( ( struct ofp_action_enqueue * ) action );
  case OFPAT_VENDOR:
    return validate_action_vendor( ( struct ofp_action_vendor_header * ) action );
  default:
    break;
  }

  return ERROR_UNDEFINED_ACTION_TYPE;
}


int
validate_actions( struct ofp_action_header *actions_head, const uint16_t length ) {
  int ret;
  uint16_t offset = 0;
  struct ofp_action_header *action;

  action = actions_head;
  while ( offset < length ) {
    ret = validate_action( action );
    if ( ret < 0 ) {
      return ret;
    }

    offset = ( uint16_t ) ( offset + ntohs( action->len ) );
    action = ( struct ofp_action_header * ) ( ( char * ) actions_head + offset );
  }

  return 0;
}


int
validate_action_output( const struct ofp_action_output *action ) {
  int ret;
  struct ofp_action_output output;

  ntoh_action_output( &output, action );
  if ( output.type != OFPAT_OUTPUT ) {
    return ERROR_INVALID_ACTION_TYPE;
  }
  if ( output.len < sizeof( struct ofp_action_output ) ) {
    return ERROR_TOO_SHORT_ACTION_OUTPUT;
  }
  else if ( output.len > sizeof( struct ofp_action_output ) ) {
    return ERROR_TOO_LONG_ACTION_OUTPUT;
  }

  ret = validate_phy_port_no( output.port );
  if ( ret < 0 ) {
    return ret;
  }

  // output.max_len

  return 0;
}


int
validate_action_set_vlan_vid( const struct ofp_action_vlan_vid *action ) {
  int ret;
  struct ofp_action_vlan_vid vlan_vid;

  ntoh_action_vlan_vid( &vlan_vid, action );

  if ( vlan_vid.type != OFPAT_SET_VLAN_VID ) {
    return ERROR_INVALID_ACTION_TYPE;
  }
  if ( vlan_vid.len < sizeof( struct ofp_action_vlan_vid ) ) {
    return ERROR_TOO_SHORT_ACTION_VLAN_VID;
  }
  else if ( vlan_vid.len > sizeof( struct ofp_action_vlan_vid ) ) {
    return ERROR_TOO_LONG_ACTION_VLAN_VID;
  }

  ret = validate_vlan_vid( vlan_vid.vlan_vid );
  if ( ret < 0 ) {
    return ret;
  }

  return 0;
}


int
validate_action_set_vlan_pcp( const struct ofp_action_vlan_pcp *action ) {
  int ret;
  struct ofp_action_vlan_pcp vlan_pcp;

  ntoh_action_vlan_pcp( &vlan_pcp, action );

  if ( vlan_pcp.type != OFPAT_SET_VLAN_PCP ) {
    return ERROR_INVALID_ACTION_TYPE;
  }
  if ( vlan_pcp.len < sizeof( struct ofp_action_vlan_pcp ) ) {
    return ERROR_TOO_SHORT_ACTION_VLAN_PCP;
  }
  else if ( vlan_pcp.len > sizeof( struct ofp_action_vlan_pcp ) ) {
    return ERROR_TOO_LONG_ACTION_VLAN_PCP;
  }

  ret = validate_vlan_pcp( vlan_pcp.vlan_pcp );
  if ( ret < 0 ) {
    return ret;
  }

  return 0;
}


int
validate_action_strip_vlan( const struct ofp_action_header *action ) {
  struct ofp_action_header strip_vlan;

  ntoh_action_strip_vlan( &strip_vlan, action );

  if ( strip_vlan.type != OFPAT_STRIP_VLAN ) {
    return ERROR_INVALID_ACTION_TYPE;
  }
  if ( strip_vlan.len < sizeof( struct ofp_action_header ) ) {
    return ERROR_TOO_SHORT_ACTION_STRIP_VLAN;
  }
  else if ( strip_vlan.len > sizeof( struct ofp_action_header ) ) {
    return ERROR_TOO_LONG_ACTION_STRIP_VLAN;
  }

  return 0;
}


int
validate_action_set_dl_src( const struct ofp_action_dl_addr *action ) {
  struct ofp_action_dl_addr dl_src;

  ntoh_action_dl_addr( &dl_src, action );

  if ( dl_src.type != OFPAT_SET_DL_SRC ) {
    return ERROR_INVALID_ACTION_TYPE;
  }
  if ( dl_src.len < sizeof( struct ofp_action_dl_addr ) ) {
    return ERROR_TOO_SHORT_ACTION_DL_SRC;
  }
  else if ( dl_src.len > sizeof( struct ofp_action_dl_addr ) ) {
    return ERROR_TOO_LONG_ACTION_DL_SRC;
  }

  return 0;
}


int
validate_action_set_dl_dst( const struct ofp_action_dl_addr *action ) {
  struct ofp_action_dl_addr dl_dst;

  ntoh_action_dl_addr( &dl_dst, action );

  if ( dl_dst.type != OFPAT_SET_DL_DST ) {
    return ERROR_INVALID_ACTION_TYPE;
  }
  if ( dl_dst.len < sizeof( struct ofp_action_dl_addr ) ) {
    return ERROR_TOO_SHORT_ACTION_DL_DST;
  }
  else if ( dl_dst.len > sizeof( struct ofp_action_dl_addr ) ) {
    return ERROR_TOO_LONG_ACTION_DL_DST;
  }

  return 0;
}


int
validate_action_set_nw_src( const struct ofp_action_nw_addr *action ) {
  struct ofp_action_nw_addr nw_src;

  ntoh_action_nw_addr( &nw_src, action );

  if ( nw_src.type != OFPAT_SET_NW_SRC ) {
    return ERROR_INVALID_ACTION_TYPE;
  }
  if ( nw_src.len < sizeof( struct ofp_action_nw_addr ) ) {
    return ERROR_TOO_SHORT_ACTION_NW_SRC;
  }
  else if ( nw_src.len > sizeof( struct ofp_action_nw_addr ) ) {
    return ERROR_TOO_LONG_ACTION_NW_SRC;
  }

  return 0;
}


int
validate_action_set_nw_dst( const struct ofp_action_nw_addr *action ) {
  struct ofp_action_nw_addr nw_dst;

  ntoh_action_nw_addr( &nw_dst, action );

  if ( nw_dst.type != OFPAT_SET_NW_DST ) {
    return ERROR_INVALID_ACTION_TYPE;
  }
  if ( nw_dst.len < sizeof( struct ofp_action_nw_addr ) ) {
    return ERROR_TOO_SHORT_ACTION_NW_DST;
  }
  else if ( nw_dst.len > sizeof( struct ofp_action_nw_addr ) ) {
    return ERROR_TOO_LONG_ACTION_NW_DST;
  }

  return 0;
}


int
validate_action_set_nw_tos( const struct ofp_action_nw_tos *action ) {
  int ret;
  struct ofp_action_nw_tos nw_tos;

  ntoh_action_nw_tos( &nw_tos, action );

  if ( nw_tos.type != OFPAT_SET_NW_TOS ) {
    return ERROR_INVALID_ACTION_TYPE;
  }
  if ( nw_tos.len < sizeof( struct ofp_action_nw_addr ) ) {
    return ERROR_TOO_SHORT_ACTION_NW_TOS;
  }
  else if ( nw_tos.len > sizeof( struct ofp_action_nw_addr ) ) {
    return ERROR_TOO_LONG_ACTION_NW_TOS;
  }

  ret = validate_nw_tos( nw_tos.nw_tos );
  if ( ret < 0 ) {
    return ret;
  }

  return 0;
}


int
validate_action_set_tp_src( const struct ofp_action_tp_port *action ) {
  struct ofp_action_tp_port tp_src;

  ntoh_action_tp_port( &tp_src, action );

  if ( tp_src.type != OFPAT_SET_TP_SRC ) {
    return ERROR_INVALID_ACTION_TYPE;
  }
  if ( tp_src.len < sizeof( struct ofp_action_tp_port ) ) {
    return ERROR_TOO_SHORT_ACTION_TP_SRC;
  }
  else if ( tp_src.len > sizeof( struct ofp_action_tp_port ) ) {
    return ERROR_TOO_LONG_ACTION_TP_SRC;
  }

  return 0;
}


int
validate_action_set_tp_dst( const struct ofp_action_tp_port *action ) {
  struct ofp_action_tp_port tp_dst;

  ntoh_action_tp_port( &tp_dst, action );

  if ( tp_dst.type != OFPAT_SET_TP_DST ) {
    return ERROR_INVALID_ACTION_TYPE;
  }
  if ( tp_dst.len < sizeof( struct ofp_action_tp_port ) ) {
    return ERROR_TOO_SHORT_ACTION_TP_DST;
  }
  else if ( tp_dst.len > sizeof( struct ofp_action_tp_port ) ) {
    return ERROR_TOO_LONG_ACTION_TP_DST;
  }

  return 0;
}


int
validate_action_enqueue( const struct ofp_action_enqueue *action ) {
  int ret;
  struct ofp_action_enqueue enqueue;

  ntoh_action_enqueue( &enqueue, action );

  if ( enqueue.type != OFPAT_ENQUEUE ) {
    return ERROR_INVALID_ACTION_TYPE;
  }
  if ( enqueue.len < sizeof( struct ofp_action_enqueue ) ) {
    return ERROR_TOO_SHORT_ACTION_ENQUEUE;
  }
  else if ( enqueue.len > sizeof( struct ofp_action_enqueue ) ) {
    return ERROR_TOO_LONG_ACTION_ENQUEUE;
  }

  ret = validate_phy_port_no( enqueue.port );
  if ( ret < 0 ) {
    return ret;
  }

  // enqueue.queue_id

  return 0;
}


int
validate_action_vendor( const struct ofp_action_vendor_header *action ) {
  if ( ntohs( action->type ) != OFPAT_VENDOR ) {
    return ERROR_INVALID_ACTION_TYPE;
  }
  if ( ntohs( action->len ) < sizeof( struct ofp_action_vendor_header ) ) {
    return ERROR_TOO_SHORT_ACTION_VENDOR;
  }

  // action->vendor

  return 0;
}


int
validate_openflow_message( const buffer *message ) {
  int ret;

  assert( message != NULL );
  assert( message->data != NULL );

  struct ofp_header *header = ( struct ofp_header * ) message->data;

  debug( "Validating an OpenFlow message ( version = %#x, type = %#x, length = %u, xid = %#x ).",
         header->version, header->type, ntohs( header->length ), ntohl( header->xid ) );

  switch ( header->type ) {
  case OFPT_HELLO:
    ret = validate_hello( message );
    break;
  case OFPT_ERROR:
    ret = validate_error( message );
    break;
  case OFPT_ECHO_REQUEST:
    ret = validate_echo_request( message );
    break;
  case OFPT_ECHO_REPLY:
    ret = validate_echo_reply( message );
    break;
  case OFPT_VENDOR:
    ret = validate_vendor( message );
    break;
  case OFPT_FEATURES_REQUEST:
    ret = validate_features_request( message );
    break;
  case OFPT_FEATURES_REPLY:
    ret = validate_features_reply( message );
    break;
  case OFPT_GET_CONFIG_REQUEST:
    ret = validate_get_config_request( message );
    break;
  case OFPT_GET_CONFIG_REPLY:
    ret = validate_get_config_reply( message );
    break;
  case OFPT_SET_CONFIG:
    ret = validate_set_config( message );
    break;
  case OFPT_PACKET_IN:
    ret = validate_packet_in( message );
    break;
  case OFPT_FLOW_REMOVED:
    ret = validate_flow_removed( message );
    break;
  case OFPT_PORT_STATUS:
    ret = validate_port_status( message );
    break;
  case OFPT_PACKET_OUT:
    ret = validate_packet_out( message );
    break;
  case OFPT_FLOW_MOD:
    ret = validate_flow_mod( message );
    break;
  case OFPT_PORT_MOD:
    ret = validate_port_mod( message );
    break;
  case OFPT_STATS_REQUEST:
    ret = validate_stats_request( message );
    break;
  case OFPT_STATS_REPLY:
    ret = validate_stats_reply( message );
    break;
  case OFPT_BARRIER_REQUEST:
    ret = validate_barrier_request( message );
    break;
  case OFPT_BARRIER_REPLY:
    ret = validate_barrier_reply( message );
    break;
  case OFPT_QUEUE_GET_CONFIG_REQUEST:
    ret = validate_queue_get_config_request( message );
    break;
  case OFPT_QUEUE_GET_CONFIG_REPLY:
    ret = validate_queue_get_config_reply( message );
    break;
  default:
    ret = ERROR_UNDEFINED_TYPE;
    break;
  }

  debug( "Validation completed ( ret = %d ).", ret );

  return ret;
}


bool
valid_openflow_message( const buffer *message ) {
  if ( validate_openflow_message( message ) < 0 ) {
    return false;
  }

  return true;
}

static struct error_map {
  uint8_t type; // One of the OFPT_ constants.
  struct map {
    int error_no; // Internal error number.
    uint16_t error_type; // OpenFlow error type.
    uint16_t error_code; // OpenFlow error code.
  } maps[ 64 ];
} error_maps[] = {
  {
    OFPT_HELLO,
    {
      { ERROR_UNSUPPORTED_VERSION, OFPET_HELLO_FAILED, OFPHFC_INCOMPATIBLE },
      { ERROR_TOO_SHORT_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_TOO_LONG_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_UNDEFINED_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { ERROR_INVALID_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { 0, 0, 0 },
    }
  },
  {
    OFPT_ERROR,
    {
      { ERROR_UNSUPPORTED_VERSION, OFPET_BAD_REQUEST, OFPBRC_BAD_VERSION },
      { ERROR_TOO_SHORT_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_TOO_LONG_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_UNDEFINED_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { ERROR_INVALID_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { 0, 0, 0 },
    }
  },
  {
    OFPT_ECHO_REQUEST,
    {
      { ERROR_UNSUPPORTED_VERSION, OFPET_BAD_REQUEST, OFPBRC_BAD_VERSION },
      { ERROR_TOO_SHORT_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_TOO_LONG_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_UNDEFINED_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { ERROR_INVALID_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { 0, 0, 0 },
    }
  },
  {
    OFPT_ECHO_REPLY,
    {
      { ERROR_UNSUPPORTED_VERSION, OFPET_BAD_REQUEST, OFPBRC_BAD_VERSION },
      { ERROR_TOO_SHORT_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_TOO_LONG_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_UNDEFINED_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { ERROR_INVALID_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { 0, 0, 0 },
    }
  },
  {
    OFPT_VENDOR,
    {
      { ERROR_UNSUPPORTED_VERSION, OFPET_BAD_REQUEST, OFPBRC_BAD_VERSION },
      { ERROR_TOO_SHORT_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_TOO_LONG_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_UNDEFINED_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { ERROR_INVALID_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { 0, 0, 0 },
    }
  },
  {
    OFPT_FEATURES_REQUEST,
    {
      { ERROR_UNSUPPORTED_VERSION, OFPET_BAD_REQUEST, OFPBRC_BAD_VERSION },
      { ERROR_TOO_SHORT_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_TOO_LONG_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_UNDEFINED_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { ERROR_INVALID_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { 0, 0, 0 },
    }
  },
  {
    OFPT_FEATURES_REPLY,
    {
      { ERROR_UNSUPPORTED_VERSION, OFPET_BAD_REQUEST, OFPBRC_BAD_VERSION },
      { ERROR_TOO_SHORT_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_TOO_LONG_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_UNDEFINED_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { ERROR_INVALID_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { 0, 0, 0 },
    }
  },
  {
    OFPT_GET_CONFIG_REQUEST,
    {
      { ERROR_UNSUPPORTED_VERSION, OFPET_BAD_REQUEST, OFPBRC_BAD_VERSION },
      { ERROR_TOO_SHORT_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_TOO_LONG_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_UNDEFINED_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { ERROR_INVALID_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { 0, 0, 0 },
    }
  },
  {
    OFPT_GET_CONFIG_REPLY,
    {
      { ERROR_UNSUPPORTED_VERSION, OFPET_BAD_REQUEST, OFPBRC_BAD_VERSION },
      { ERROR_TOO_SHORT_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_TOO_LONG_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_UNDEFINED_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { ERROR_INVALID_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { 0, 0, 0 },
    }
  },
  {
    OFPT_SET_CONFIG,
    {
      { ERROR_UNSUPPORTED_VERSION, OFPET_BAD_REQUEST, OFPBRC_BAD_VERSION },
      { ERROR_TOO_SHORT_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_TOO_LONG_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_UNDEFINED_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { ERROR_INVALID_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { ERROR_INVALID_SWITCH_CONFIG, OFPET_BAD_REQUEST, OFPBRC_EPERM }, // FIXME
      { 0, 0, 0 },
    }
  },
  {
    OFPT_PACKET_IN, // FIXME: Should we return an error for packet_in ?
    {
      { ERROR_UNSUPPORTED_VERSION, OFPET_BAD_REQUEST, OFPBRC_BAD_VERSION },
      { ERROR_TOO_SHORT_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_TOO_LONG_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_UNDEFINED_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { ERROR_INVALID_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { ERROR_INVALID_PACKET_IN_REASON, OFPET_BAD_REQUEST, OFPBRC_EPERM }, // FIXME
      { 0, 0, 0 },
    }
  },
  {
    OFPT_FLOW_REMOVED,  // FIXME: Should we return an error for flow_removed ?
    {
      { ERROR_UNSUPPORTED_VERSION, OFPET_BAD_REQUEST, OFPBRC_BAD_VERSION },
      { ERROR_TOO_SHORT_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_TOO_LONG_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_UNDEFINED_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { ERROR_INVALID_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { ERROR_INVALID_FLOW_PRIORITY, OFPET_BAD_REQUEST, OFPBRC_EPERM }, // FIXME
      { ERROR_INVALID_FLOW_REMOVED_REASON, OFPET_BAD_REQUEST, OFPBRC_EPERM }, // FIXME
      { 0, 0, 0 },
    }
  },
  {
    OFPT_PORT_STATUS,  // FIXME: Should we return an error for port_status ?
    {
      { ERROR_UNSUPPORTED_VERSION, OFPET_BAD_REQUEST, OFPBRC_BAD_VERSION },
      { ERROR_TOO_SHORT_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_TOO_LONG_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_UNDEFINED_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { ERROR_INVALID_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { ERROR_INVALID_PORT_STATUS_REASON, OFPET_BAD_REQUEST, OFPBRC_EPERM }, // FIXME
      { 0, 0, 0 },
    }
  },
  {
    OFPT_PACKET_OUT,
    {
      { ERROR_UNSUPPORTED_VERSION, OFPET_BAD_REQUEST, OFPBRC_BAD_VERSION },
      { ERROR_TOO_SHORT_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_TOO_LONG_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_UNDEFINED_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { ERROR_INVALID_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { ERROR_INVALID_ACTION_TYPE, OFPET_BAD_ACTION, OFPBAC_BAD_TYPE },
      { ERROR_TOO_SHORT_ACTION_OUTPUT, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_LONG_ACTION_OUTPUT, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_INVALID_PORT_NO, OFPET_BAD_ACTION, OFPBAC_BAD_OUT_PORT },
      { ERROR_TOO_SHORT_ACTION_VLAN_VID, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_LONG_ACTION_VLAN_VID, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_INVALID_VLAN_VID, OFPET_BAD_ACTION, OFPBAC_BAD_ARGUMENT },
      { ERROR_TOO_SHORT_ACTION_VLAN_PCP, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_LONG_ACTION_VLAN_PCP, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_INVALID_VLAN_PCP, OFPET_BAD_ACTION, OFPBAC_BAD_ARGUMENT },
      { ERROR_TOO_SHORT_ACTION_STRIP_VLAN, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_LONG_ACTION_STRIP_VLAN, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_SHORT_ACTION_DL_SRC, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_LONG_ACTION_DL_SRC, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_SHORT_ACTION_DL_DST, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_LONG_ACTION_DL_DST, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_SHORT_ACTION_NW_SRC, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_LONG_ACTION_NW_SRC, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_SHORT_ACTION_NW_DST, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_LONG_ACTION_NW_DST, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_SHORT_ACTION_NW_TOS, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_LONG_ACTION_NW_TOS, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_SHORT_ACTION_TP_SRC, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_LONG_ACTION_TP_SRC, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_SHORT_ACTION_TP_DST, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_LONG_ACTION_TP_DST, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_SHORT_ACTION_ENQUEUE, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_LONG_ACTION_ENQUEUE, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_INVALID_PORT_NO, OFPET_BAD_ACTION, OFPBAC_BAD_OUT_PORT },
      { ERROR_TOO_SHORT_ACTION_VENDOR, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_UNDEFINED_ACTION_TYPE, OFPET_BAD_ACTION, OFPBAC_BAD_TYPE },
      { 0, 0, 0 },
    }
  },
  {
    OFPT_FLOW_MOD,
    {
      { ERROR_UNSUPPORTED_VERSION, OFPET_BAD_REQUEST, OFPBRC_BAD_VERSION },
      { ERROR_TOO_SHORT_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_TOO_LONG_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_UNDEFINED_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { ERROR_INVALID_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { ERROR_INVALID_ACTION_TYPE, OFPET_BAD_ACTION, OFPBAC_BAD_TYPE },
      { ERROR_TOO_SHORT_ACTION_OUTPUT, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_LONG_ACTION_OUTPUT, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_INVALID_PORT_NO, OFPET_BAD_ACTION, OFPBAC_BAD_OUT_PORT },
      { ERROR_TOO_SHORT_ACTION_VLAN_VID, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_LONG_ACTION_VLAN_VID, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_INVALID_VLAN_VID, OFPET_BAD_ACTION, OFPBAC_BAD_ARGUMENT },
      { ERROR_TOO_SHORT_ACTION_VLAN_PCP, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_LONG_ACTION_VLAN_PCP, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_INVALID_VLAN_PCP, OFPET_BAD_ACTION, OFPBAC_BAD_ARGUMENT },
      { ERROR_TOO_SHORT_ACTION_STRIP_VLAN, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_LONG_ACTION_STRIP_VLAN, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_SHORT_ACTION_DL_SRC, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_LONG_ACTION_DL_SRC, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_SHORT_ACTION_DL_DST, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_LONG_ACTION_DL_DST, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_SHORT_ACTION_NW_SRC, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_LONG_ACTION_NW_SRC, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_SHORT_ACTION_NW_DST, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_LONG_ACTION_NW_DST, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_SHORT_ACTION_NW_TOS, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_LONG_ACTION_NW_TOS, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_SHORT_ACTION_TP_SRC, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_LONG_ACTION_TP_SRC, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_SHORT_ACTION_TP_DST, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_LONG_ACTION_TP_DST, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_SHORT_ACTION_ENQUEUE, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_TOO_LONG_ACTION_ENQUEUE, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_INVALID_PORT_NO, OFPET_BAD_ACTION, OFPBAC_BAD_OUT_PORT },
      { ERROR_TOO_SHORT_ACTION_VENDOR, OFPET_BAD_ACTION, OFPBAC_BAD_LEN },
      { ERROR_UNDEFINED_ACTION_TYPE, OFPET_BAD_ACTION, OFPBAC_BAD_TYPE },
      { ERROR_INVALID_WILDCARDS, OFPET_FLOW_MOD_FAILED, OFPFMFC_EPERM }, // FIXME
      { ERROR_UNDEFINED_FLOW_MOD_COMMAND, OFPET_FLOW_MOD_FAILED, OFPFMFC_BAD_COMMAND },
      { ERROR_INVALID_FLOW_PRIORITY, OFPET_FLOW_MOD_FAILED, OFPFMFC_EPERM }, // FIXME
      { ERROR_INVALID_FLOW_MOD_FLAGS, OFPET_FLOW_MOD_FAILED, OFPFMFC_EPERM }, // FIXME
      { 0, 0, 0 },
    }
  },
  {
    OFPT_PORT_MOD,
    {
      { ERROR_UNSUPPORTED_VERSION, OFPET_BAD_REQUEST, OFPBRC_BAD_VERSION },
      { ERROR_TOO_SHORT_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_TOO_LONG_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_UNDEFINED_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { ERROR_INVALID_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { ERROR_INVALID_PORT_NO, OFPET_PORT_MOD_FAILED, OFPPMFC_BAD_PORT },
      { ERROR_INVALID_PORT_CONFIG, OFPET_BAD_REQUEST, OFPBRC_EPERM }, // FIXME
      { ERROR_INVALID_PORT_MASK, OFPET_BAD_REQUEST, OFPBRC_EPERM }, // FIXME
      { ERROR_INVALID_PORT_FEATURES, OFPET_BAD_REQUEST, OFPBRC_EPERM }, // FIXME
      { 0, 0, 0 },
    }
  },
  {
    OFPT_STATS_REQUEST,
    {
      { ERROR_UNSUPPORTED_VERSION, OFPET_BAD_REQUEST, OFPBRC_BAD_VERSION },
      { ERROR_TOO_SHORT_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_TOO_LONG_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_UNDEFINED_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { ERROR_INVALID_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { ERROR_UNSUPPORTED_STATS_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_STAT },
      { ERROR_INVALID_STATS_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_STAT },
      { ERROR_INVALID_STATS_REQUEST_FLAGS, OFPET_BAD_REQUEST, OFPBRC_EPERM }, // FIXME
      { ERROR_INVALID_PORT_NO, OFPET_BAD_REQUEST, OFPBRC_EPERM }, // FIXME
      { ERROR_INVALID_WILDCARDS, OFPET_BAD_REQUEST, OFPBRC_EPERM }, // FIXME
      { ERROR_INVALID_VLAN_VID, OFPET_BAD_REQUEST, OFPBRC_EPERM }, // FIXME
      { ERROR_INVALID_VLAN_PCP, OFPET_BAD_REQUEST, OFPBRC_EPERM }, // FIXME
      { ERROR_INVALID_NW_TOS, OFPET_BAD_REQUEST, OFPBRC_EPERM }, // FIXME
      { 0, 0, 0 },
    }
  },
  {
    OFPT_STATS_REPLY,
    {
      { ERROR_UNSUPPORTED_VERSION, OFPET_BAD_REQUEST, OFPBRC_BAD_VERSION },
      { ERROR_TOO_SHORT_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_TOO_LONG_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_UNDEFINED_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { ERROR_INVALID_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { 0, 0, 0 },
    }
  },
  {
    OFPT_BARRIER_REQUEST,
    {
      { ERROR_UNSUPPORTED_VERSION, OFPET_BAD_REQUEST, OFPBRC_BAD_VERSION },
      { ERROR_TOO_SHORT_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_TOO_LONG_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_UNDEFINED_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { ERROR_INVALID_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { 0, 0, 0 },
    }
  },
  {
    OFPT_BARRIER_REPLY,
    {
      { ERROR_UNSUPPORTED_VERSION, OFPET_BAD_REQUEST, OFPBRC_BAD_VERSION },
      { ERROR_TOO_SHORT_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_TOO_LONG_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_UNDEFINED_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { ERROR_INVALID_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { 0, 0, 0 },
    }
  },
  {
    OFPT_QUEUE_GET_CONFIG_REQUEST,
    {
      { ERROR_UNSUPPORTED_VERSION, OFPET_BAD_REQUEST, OFPBRC_BAD_VERSION },
      { ERROR_TOO_SHORT_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_TOO_LONG_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_UNDEFINED_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { ERROR_INVALID_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { ERROR_INVALID_PORT_NO, OFPET_QUEUE_OP_FAILED, OFPQOFC_BAD_PORT },
      { 0, 0, 0 },
    }
  },
  {
    OFPT_QUEUE_GET_CONFIG_REPLY,
    {
      { ERROR_UNSUPPORTED_VERSION, OFPET_BAD_REQUEST, OFPBRC_BAD_VERSION },
      { ERROR_TOO_SHORT_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_TOO_LONG_MESSAGE, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN },
      { ERROR_UNDEFINED_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { ERROR_INVALID_TYPE, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE },
      { 0, 0, 0 },
    }
  },
};


bool
cc_trans_error_type_and_code( const uint8_t type, const int error_no,
                         uint16_t *error_type, uint16_t *error_code ) {
  if ( type > OFPT_QUEUE_GET_CONFIG_REPLY ) {
    *error_type = OFPET_BAD_REQUEST;
    *error_code = OFPBRC_BAD_TYPE;
    debug( "Undefined OpenFlow message type ( type = %u ).", type );
    return true;
  }

  int i = 0;
  for ( i = 0; error_maps[ type ].maps[ i ].error_no != 0; i++ ) {
    if ( error_no == error_maps[ type ].maps[ i ].error_no ) {
      *error_type = error_maps[ type ].maps[ i ].error_type;
      *error_code = error_maps[ type ].maps[ i ].error_code;
      return true;
    }
  }

  return false;
}

