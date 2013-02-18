/*
 *  mul_of_msg.h: MUL openflow message handling 
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
#ifndef __MUL_OF_MSG_H__
#define __MUL_OF_MSG_H__

static inline int
of_get_data_len(void *h)
{
    return ntohs(((struct ofp_header *)h)->length);
}

static inline bool 
of_hdr_valid(void *h_arg)
{
    struct ofp_header *h = h_arg;
    return ntohs(h->length) <= 4096 && h->version == 1 && h->type < OFPT_BARRIER_REPLY;
}

static inline bool
c_app_of_hdr_valid(void *h_arg)
{
    struct ofp_header *h = h_arg;
    return ntohs(h->length) <= 4096 && h->version == 1 &&
            ((h->type >= 0 && h->type <= C_OFPT_SET_FPOPS));
}

struct of_flow_mod_params {
    void        *app_owner;
    struct flow *flow;
    void        *actions;
    size_t      action_len;
    uint32_t    wildcards;
    uint32_t    buffer_id;
    uint16_t    prio;
    uint16_t    itimeo; 
    uint16_t    htimeo;
    uint8_t     flags;
    uint8_t     tbl_idx;
};

struct of_pkt_out_params {
    uint32_t buffer_id;
    uint16_t in_port;
    uint16_t action_len;
    void     *action_list;
    void     *data;
    uint16_t data_len;
    uint8_t  pad[2];
};  

size_t of_make_action_output(char **pbuf, size_t bufroom, uint16_t oport);
size_t of_make_action_set_vid(char **pbuf, size_t bufroom, uint16_t vid);
size_t of_make_action_set_dmac(char **pbuf, size_t bufroom, uint8_t *dmac);
char *of_dump_actions(void *actions, size_t action_len);
int of_validate_actions(void *actions, size_t action_len);
char *of_dump_wildcards(uint32_t wildcards);
void *of_prep_msg(size_t len, uint8_t type, uint32_t xid);
struct cbuf *of_prep_flow_mod(uint16_t command, const struct flow *flow, 
                              size_t actions_len, uint32_t wildcards);  
struct cbuf *of_prep_flow_add_msg(const struct flow *flow, uint32_t buffer_id,
                                  void *actions, size_t actions_len, 
                                  uint16_t i_timeo, uint16_t h_timeo, 
                                  uint32_t wildcards, uint16_t prio);
struct cbuf *of_prep_flow_del_msg(const struct flow *flow, uint32_t wildcards,
                                  uint16_t oport, bool strict);
void *of_prep_pkt_out_msg(struct of_pkt_out_params *parms);

static struct error_map {
  uint8_t type; // One of the OFPT_ constants.
  struct map {
    int error_no; // Internal error number.
    uint16_t error_type; // OpenFlow error type.
    uint16_t error_code; // OpenFlow error code.
  } maps[ 64 ];
}

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


#endif

