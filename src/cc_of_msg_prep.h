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

#include "cc_random.h"
#include "cc_basic.h"
#include "cc_xid.h"
#include "cc_ether.h"

#if 0
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
#endif
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

static inline uint32_t make_inet_mask(uint8_t len)
{
    return (~((1 << (32 - (len))) - 1));
}

#if 0
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
#endif //END OF 0
// Functions for creating OpenFlow messages
buffer *cc_create_hello( const uint32_t  transaction_id);
buffer *cc_create_error( const uint32_t transaction_id, const uint16_t type,
                      const uint16_t code, const buffer *data );
buffer *cc_create_echo_request( const uint32_t transaction_id, const buffer *body );
buffer *cc_create_echo_reply( const uint32_t transaction_id, const buffer *body );
buffer *cc_create_vendor( const uint32_t transaction_id, const uint32_t vendor,
                       const buffer *body );
buffer *cc_create_features_request( const uint32_t transaction_id );
buffer *cc_create_features_reply( const uint32_t transaction_id, const uint64_t datapath_id,
                               const uint32_t n_buffers, const uint8_t n_tables,
                               const uint32_t capabilities, const uint32_t actions,
                               const list_element *ports );
buffer *cc_create_get_config_request( const uint32_t transaction_id );
buffer *cc_create_get_config_reply( const uint32_t transaction_id, const uint16_t flags,
                                 const uint16_t miss_send_len );
buffer *cc_create_set_config( const uint32_t transaction_id, const uint16_t flags,
                           const uint16_t miss_send_len );
buffer *cc_create_packet_in( const uint32_t transaction_id, const uint32_t buffer_id,
                          const uint16_t total_len, uint16_t in_port,
                          const uint8_t reason, const buffer *data );
buffer *cc_create_flow_removed( const uint32_t transaction_id, const struct ofp_match match,
                            const uint64_t cookie, const uint16_t priority,
                            const uint8_t reason, const uint32_t duration_sec,
                            const uint32_t duration_nsec, const uint16_t idle_timeout,
                            const uint64_t packet_count, const uint64_t byte_count );
buffer *cc_create_port_status( const uint32_t transaction_id, const uint8_t reason,
                            const struct ofp_phy_port desc );
buffer *cc_create_packet_out( const uint32_t transaction_id, const uint32_t buffer_id,
                           const uint16_t in_port, const openflow_actions *actions,
                           const buffer *data );
buffer *cc_create_flow_mod(
  const uint32_t transaction_id,
  const struct ofp_match match,
  const uint64_t cookie,
  const uint16_t command,
  const uint16_t idle_timeout,
  const uint16_t hard_timeout,
  const uint16_t priority,
  const uint32_t buffer_id,
  const uint16_t out_port,
  const uint16_t flags,
  const openflow_actions *actions
);
buffer *cc_create_port_mod( const uint32_t transaction_id, const uint16_t port_no,
                         const uint8_t hw_addr[ OFP_ETH_ALEN ], const uint32_t config,
                         const uint32_t mask, const uint32_t advertise );
buffer *cc_create_desc_stats_request( const uint32_t transaction_id, const uint16_t flags );
buffer *cc_create_flow_stats_request( const uint32_t transaction_id, const uint16_t flags,
                                   const struct ofp_match match, const uint8_t table_id,
                                   const uint16_t out_port );
buffer *cc_create_aggregate_stats_request( const uint32_t transaction_id,
                                        const uint16_t flags, const struct ofp_match match,
                                        const uint8_t table_id, const uint16_t out_port );
buffer *cc_create_table_stats_request( const uint32_t transaction_id, const uint16_t flags );
buffer *cc_create_port_stats_request( const uint32_t transaction_id, const uint16_t flags,
                                   const uint16_t port_no );
buffer *cc_create_queue_stats_request( const uint32_t transaction_id, const uint16_t flags,
                                    const uint16_t port_no, const uint32_t queue_id );
buffer *cc_create_vendor_stats_request( const uint32_t transaction_id, const uint16_t flags,
                                     const uint32_t vendor, const buffer *body );
buffer *cc_create_desc_stats_reply( const uint32_t transaction_id, const uint16_t flags,
                                 const char mfr_desc[ DESC_STR_LEN ],
                                 const char hw_desc[ DESC_STR_LEN ],
                                 const char sw_desc[ DESC_STR_LEN ],
                                 const char serial_num[ SERIAL_NUM_LEN ],
                                 const char dp_desc[ DESC_STR_LEN ] );
buffer *cc_create_flow_stats_reply( const uint32_t transaction_id, const uint16_t flags,
                                 const list_element *flows_stats_head );
buffer *cc_create_aggregate_stats_reply( const uint32_t transaction_id,
                                      const uint16_t flags,
                                      const uint64_t packet_count, const uint64_t byte_count,
                                      const uint32_t flow_count );
buffer *cc_create_table_stats_reply( const uint32_t transaction_id, const uint16_t flags,
                                  const list_element *table_stats_head );
buffer *cc_create_port_stats_reply( const uint32_t transaction_id, const uint16_t flags,
                                 const list_element *port_stats_head );
buffer *cc_create_queue_stats_reply( const uint32_t transaction_id, const uint16_t flags,
                                  const list_element *queue_stats_head );
buffer *cc_create_vendor_stats_reply( const uint32_t transaction_id, const uint16_t flags,
                                   const uint32_t vendor, const buffer *body );
buffer *cc_create_barrier_request( const uint32_t transaction_id );
buffer *cc_create_barrier_reply( const uint32_t transaction_id );
buffer *cc_create_queue_get_config_request( const uint32_t transaction_id, const uint16_t port );
buffer *cc_create_queue_get_config_reply( const uint32_t transaction_id, const uint16_t port,
                                       const list_element *queues );


#endif

