/*
 * cc_of_msg_recv functions.
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

#ifndef CC_OF_HANDLER_H
#define CC_OF_HANDLER_H 1

#include "cc_basic.h"
#include "cc_of_err.h"
#include "cc_of_msg_prep.h"
#include "cc_of_action.h"
#include "cc_xid.h"

struct flow {
    uint32_t            nw_src;            /* IP source address. */
    uint32_t            nw_dst;            /* IP destination address. */
    uint16_t            in_port;           /* Input switch port. */
    uint16_t            dl_vlan;           /* Input VLAN id. */
    uint16_t            dl_type;           /* Ethernet frame type. */
    uint16_t            tp_src;            /* TCP/UDP source port. */
    uint16_t            tp_dst;            /* TCP/UDP destination port. */
    uint8_t             dl_src[6];         /* Ethernet source address. */
    uint8_t             dl_dst[6];         /* Ethernet destination address. */
    uint8_t             dl_vlan_pcp;       /* Input VLAN priority. */
    uint8_t             nw_tos;            /* IPv4 DSCP. */
    uint8_t             nw_proto;          /* IP protocol. */
	//uint8_t				mpls_label;		   /* MPLS LABLE*/
    uint8_t             pad[3];
};

/*declear the recv function
*/
static int cc_insert_to_send_queue(sw_info* cc_sw_info,buffer* buf);

static int cc_insert_to_app_queue(sw_info* cc_sw_info, buffer* buf);

char* cc_dump_flow(struct flow* flow,uint32_t wildcards);

static int cc_recv_err_msg(sw_info* cc_sw_info,buffer* buf);

static int cc_recv_echo_request(sw_info* cc_sw_info,buffer* buf);

static int cc_recv_echo_reply(sw_info* cc_sw_info,buffer* buf);

static int cc_recv_hello_msg(sw_info* cc_sw_info,buffer* buf);

static int cc_recv_vendor(sw_info* cc_sw_info, buffer* buf);

static int cc_recv_flow_removed(sw_info* cc_sw_info, buffer* buf);

static int cc_recv_get_config_reply(sw_info* cc_sw_info, buffer* buf);

static int cc_recv_barrier_reply(sw_info* cc_sw_info, buffer* buf);

static int cc_recv_features_reply(sw_info* cc_sw_info, buffer* buf);

static int cc_recv_packet_in(sw_info* cc_sw_info,buffer* buf);

static int cc_recv_port_status(sw_info* cc_sw_info, buffer* buf);

static int cc_recv_stats_reply(sw_info* cc_sw_info, buffer* buf);

static int cc_recv_flow_stats_reply(sw_info* cc_sw_info, buffer* buf);

static int cc_process_phy_port(sw_info* cc_sw_info, struct ofp_phy_port* port, uint8_t reason);

/*
typedef void (*ofp_handler_t)(sw_info *cc_sw_info, buffer *b);

struct of_handler {
    ofp_handler_t handler;
};
*/
#if 0
/*handler different verison's message
*/
extern const struct of_handler_class of10_handler;
extern const struct of_handler_class of12_handler;
extern const struct of_handler_class of13_handler;


struct of_handler_class {

	/*
	 * Version of the msg between the switch and controller
	 */
	const uint8_t* version;

	int (*cc_recv_hello_msg)(sw_info* , buffer *);

	int (*cc_recv_features_reply)(sw_info* , buffer *);

	int (*cc_recv_echo_request)(sw_info* , buffer *);

	int (*cc_recv_echo_reply)(sw_info* , buffer *);

	int (*cc_recv_vendor)(sw_info* , buffer *);

	int (*cc_recv_get_config_reply)(sw_info* , buffer *);

	int (*cc_recv_flow_removed)(sw_info* , buffer *);

	int (*cc_recv_stats_reply)(sw_info* , buffer *);

	int (*cc_recv_barrier_reply)(sw_info* , buffer *);

	int (*cc_recv_packet_in)(sw_info* , buffer *);

	int (*cc_recv_err_msg)(sw_info* , buffer *);

	int (*cc_recv_port_status)(sw_info* , buffer *);
	
};

#define RET_OF_MSG_HANDLER(sw, h, b, type, length)                              \
do {                                                                            \
    if (unlikely(length < h[type].min_size || OF_UNK_MSG(h, type))) {           \
        c_log_err("unexpected length(%u) or type(%u)", (unsigned)length, type); \
        return;                                                                 \
    }                                                                           \
    return h[type].handler(sw, (void *)b);                                      \
} while(0)
#endif	//end of 0
	
#endif //end of cc_of_msg_recv
