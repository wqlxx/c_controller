#ifndef CC_OF_HANDLER_H
#define CC_OF_HANDLER_H 1


/*from mul_of */
#include "cc_basic.h"

#define NULL_OF_HANDLER     {NULL,sizeof(struct ofp_header)}
#define OFP_HDR_SZ          sizeof(struct ofp_header)

#define LENGTH_OF_MAC 6
/*4*2+2*5+6*2+1*4+2*/
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

typedef void (*ofp_handler_t)(c_switch_t *sw, struct cbuf *b);

struct of_handler {
    ofp_handler_t handler;
    size_t min_size;
};

#define RET_OF_MSG_HANDLER(sw, h, b, type, length)                              \
do {                                                                            \
    if (unlikely(length < h[type].min_size || OF_UNK_MSG(h, type))) {           \
        c_log_err("unexpected length(%u) or type(%u)", (unsigned)length, type); \
        return;                                                                 \
    }                                                                           \
    return h[type].handler(sw, (void *)b);                                      \
} while(0)


#endif

