/*
 * An OpenFlow message library.
 *
 * Author: Yasunobu Chiba
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


#ifndef CC_OF_ERR_H
#define CC_OF_ERR_H 1


#include "openflow.h"
#include "cc_bool.h"
#include "cc_buffer.h"
#include "cc_byte_order.h"
#include "cc_single_link.h"
#include "cc_basic.h"

#define VLAN_VID_MASK 0x0fff  // 12 bits
#define VLAN_PCP_MASK 0x07    // 3 bits
#define NW_TOS_MASK 0xfc      // upper 6 bits
#define ARP_OP_MASK 0x00ff    // 8 bits
#define ICMP_TYPE_MASK 0x00ff // 8 bits
#define ICMP_CODE_MASK 0x00ff // 8 bits

#define PORT_CONFIG ( OFPPC_PORT_DOWN | OFPPC_NO_STP | OFPPC_NO_RECV      \
                      | OFPPC_NO_RECV_STP | OFPPC_NO_FLOOD | OFPPC_NO_FWD \
                      | OFPPC_NO_PACKET_IN )
#define PORT_STATE ( OFPPS_LINK_DOWN | OFPPS_STP_LISTEN | OFPPS_STP_LEARN \
                     | OFPPS_STP_FORWARD | OFPPS_STP_BLOCK | OFPPS_STP_MASK )
#define PORT_FEATURES ( OFPPF_10MB_HD | OFPPF_10MB_FD | OFPPF_100MB_HD    \
                        | OFPPF_100MB_FD |  OFPPF_1GB_HD | OFPPF_1GB_FD   \
                        | OFPPF_10GB_FD | OFPPF_COPPER | OFPPF_FIBER      \
                        | OFPPF_AUTONEG | OFPPF_PAUSE | OFPPF_PAUSE_ASYM )
#define FLOW_MOD_FLAGS ( OFPFF_SEND_FLOW_REM | OFPFF_CHECK_OVERLAP | OFPFF_EMERG )

// Return code definitions indicating the result of OpenFlow message validation.
enum {
  SUCCESS = 0,
  ERROR_UNSUPPORTED_VERSION = -60,
  ERROR_INVALID_LENGTH,
  ERROR_TOO_SHORT_MESSAGE,
  ERROR_TOO_LONG_MESSAGE,
  ERROR_INVALID_TYPE,
  ERROR_UNDEFINED_TYPE,
  ERROR_UNSUPPORTED_TYPE,
  ERROR_NO_TABLE_AVAILABLE,
  ERROR_INVALID_PORT_NO,
  ERROR_INVALID_PORT_CONFIG,
  ERROR_INVALID_PORT_STATE,
  ERROR_INVALID_PORT_FEATURES,
  ERROR_INVALID_SWITCH_CONFIG,
  ERROR_INVALID_PACKET_IN_REASON,
  ERROR_INVALID_FLOW_REMOVED_REASON,
  ERROR_INVALID_WILDCARDS,
  ERROR_INVALID_VLAN_VID,
  ERROR_INVALID_VLAN_PCP,
  ERROR_INVALID_NW_TOS,
  ERROR_INVALID_PORT_STATUS_REASON,
  ERROR_TOO_SHORT_QUEUE_DESCRIPTION,
  ERROR_TOO_SHORT_QUEUE_PROPERTY,
  ERROR_TOO_LONG_QUEUE_PROPERTY,
  ERROR_UNDEFINED_QUEUE_PROPERTY,
  ERROR_TOO_SHORT_ACTION,
  ERROR_UNDEFINED_ACTION_TYPE,
  ERROR_INVALID_ACTION_TYPE,
  ERROR_TOO_SHORT_ACTION_OUTPUT,
  ERROR_TOO_LONG_ACTION_OUTPUT,
  ERROR_TOO_SHORT_ACTION_VLAN_VID,
  ERROR_TOO_LONG_ACTION_VLAN_VID,
  ERROR_TOO_SHORT_ACTION_VLAN_PCP,
  ERROR_TOO_LONG_ACTION_VLAN_PCP,
  ERROR_TOO_SHORT_ACTION_STRIP_VLAN,
  ERROR_TOO_LONG_ACTION_STRIP_VLAN,
  ERROR_TOO_SHORT_ACTION_DL_SRC,
  ERROR_TOO_LONG_ACTION_DL_SRC,
  ERROR_TOO_SHORT_ACTION_DL_DST,
  ERROR_TOO_LONG_ACTION_DL_DST,
  ERROR_TOO_SHORT_ACTION_NW_SRC,
  ERROR_TOO_LONG_ACTION_NW_SRC,
  ERROR_TOO_SHORT_ACTION_NW_DST,
  ERROR_TOO_LONG_ACTION_NW_DST,
  ERROR_TOO_SHORT_ACTION_NW_TOS,
  ERROR_TOO_LONG_ACTION_NW_TOS,
  ERROR_TOO_SHORT_ACTION_TP_SRC,
  ERROR_TOO_LONG_ACTION_TP_SRC,
  ERROR_TOO_SHORT_ACTION_TP_DST,
  ERROR_TOO_LONG_ACTION_TP_DST,
  ERROR_TOO_SHORT_ACTION_ENQUEUE,
  ERROR_TOO_LONG_ACTION_ENQUEUE,
  ERROR_TOO_SHORT_ACTION_VENDOR,
  ERROR_UNSUPPORTED_STATS_TYPE,
  ERROR_INVALID_STATS_REPLY_FLAGS,
  ERROR_INVALID_FLOW_PRIORITY,
  ERROR_INVALID_FLOW_MOD_FLAGS,
  ERROR_INVALID_PORT_MASK,
  ERROR_INVALID_STATS_TYPE,
  ERROR_INVALID_STATS_REQUEST_FLAGS,
  ERROR_UNDEFINED_FLOW_MOD_COMMAND,
  ERROR_UNEXPECTED_ERROR = -255
};


// Functions for validating OpenFlow messages
int validate_hello( const buffer *message );
int validate_error( const buffer *message );
int validate_echo_request( const buffer *message );
int validate_echo_reply( const buffer *message );
int validate_vendor( const buffer *message );
int validate_features_request( const buffer *message );
int validate_features_reply( const buffer *message );
int validate_get_config_request( const buffer *message );
int validate_get_config_reply( const buffer *message );
int validate_set_config( const buffer *message );
int validate_packet_in( const buffer *message );
int validate_flow_removed( const buffer *message );
int validate_port_status( const buffer *message );
int validate_packet_out( const buffer *message );
int validate_flow_mod( const buffer *message );
int validate_port_mod( const buffer *message );
int validate_desc_stats_request( const buffer *message );
int validate_flow_stats_request( const buffer *message );
int validate_aggregate_stats_request( const buffer *message );
int validate_table_stats_request( const buffer *message );
int validate_port_stats_request( const buffer *message );
int validate_queue_stats_request( const buffer *message );
int validate_vendor_stats_request( const buffer *message );
int validate_stats_request( const buffer *message );
int validate_desc_stats_reply( const buffer *message );
int validate_flow_stats_reply( const buffer *message );
int validate_aggregate_stats_reply( const buffer *message );
int validate_table_stats_reply( const buffer *message );
int validate_port_stats_reply( const buffer *message );
int validate_queue_stats_reply( const buffer *message );
int validate_vendor_stats_reply( const buffer *message );
int validate_stats_reply( const buffer *message );
int validate_barrier_request( const buffer *message );
int validate_barrier_reply( const buffer *message );
int validate_queue_get_config_request( const buffer *message );
int validate_queue_get_config_reply( const buffer *message );
int validate_actions( struct ofp_action_header *actions_head, const uint16_t length );
int validate_action_output( const struct ofp_action_output *action );
int validate_action_set_vlan_vid( const struct ofp_action_vlan_vid *action );
int validate_action_set_vlan_pcp( const struct ofp_action_vlan_pcp *action );
int validate_action_strip_vlan( const struct ofp_action_header *action );
int validate_action_set_dl_src( const struct ofp_action_dl_addr *action );
int validate_action_set_dl_dst( const struct ofp_action_dl_addr *action );
int validate_action_set_nw_src( const struct ofp_action_nw_addr *action );
int validate_action_set_nw_dst( const struct ofp_action_nw_addr *action );
int validate_action_set_nw_tos( const struct ofp_action_nw_tos *action );
int validate_action_set_tp_src( const struct ofp_action_tp_port *action );
int validate_action_set_tp_dst( const struct ofp_action_tp_port *action );
int validate_action_enqueue( const struct ofp_action_enqueue *action );
int validate_action_vendor( const struct ofp_action_vendor_header *action );
int validate_openflow_message( const buffer *message );
bool valid_openflow_message( const buffer *message );

// Utility functions
bool cc_trans_error_type_and_code( const uint8_t , const int ,uint16_t *, uint16_t * ); 


#endif // OPENFLOW_MESSAGE_H


/*
 * Local variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */

