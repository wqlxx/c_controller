#include "cc_of_handler.h"

static inline uint32_t
make_inet_mask(uint8_t len)
{
    return (~((1 << (32 - (len))) - 1));
}

static void
cc_recv_hello_msg(c_switch_t *sw,struct cbuf *b)
{
}


char*
cc_dump_flow(struct flow* flow,uint32_t wildcards)
{
	char *str;

	uint32_t nw_dst_mask, nw_src_mask;
    uint32_t ip_mask;

    wildcards = ntohl(wildcards);
    ip_mask = ((wildcards & OFPFW_NW_DST_MASK) >> OFPFW_NW_DST_SHIFT);
    nw_dst_mask = ip_mask >= 32 ? 0 :
                           make_inet_mask(32-ip_mask);

    ip_mask = ((wildcards & OFPFW_NW_SRC_MASK) >> OFPFW_NW_SRC_SHIFT);
    nw_src_mask = ip_mask >= 32 ? 0 :
                           make_inet_mask(32-ip_mask);

	if(wildcards&OFPFW_ALL)
	{
			sprintf(str,"in_port is %d ,dl_vlan is %d, dl_src is %d.%d.%d.%d.%d.%d \
						dl_dst is %d.%d.%d.%d.%d.%d ,dl_type is 0x%x ,nw_proto is %d ,\
						nw_proto is %d ,tp_src is %d ,tp_dst is %d ,dl_vlan_pcp is %d \
						nw_tos is %d\n",flow->in_port,flow->dl_vlan,flow->dl_src[0],flow->dl_src[1],
						flow->dl_src[2],flow->dl_src[3],flow->dl_src[4],flow->dl_src[5],flow->dl_dst[0]
						flow->dl_dst[1],flow->dl_dst[2],flow->dl_dst[3],flow->dl_dst[4],flow->dl_dst[5],
						flow->dl_type,flow->nw_proto,flow->tp_src,flow->tp_dst,flow->dl_vlan_pcp,flow->nw_tos);
			printf(str);
	}
	else
	{
		if(wildcards&OFPFW_IN_PORT)
		{
			sprintf(str,"in_port is %d ",flow->in_port);
		}

		if(wildcards&OFPFW_DL_VLAN)
		{
			sprintf(str,"dl_vlan is %d",flow->dl_vlan);
		}

		if(wildcards&OFPFW_DL_SRC)
		{
			sprintf(str,"dl_src is %d.%d.%d.%d.%d.%d ",flow->dl_src[0]
														,flow->dl_src[1]
														,flow->dl_src[2]
														,flow->dl_src[3]
														,flow->dl_src[4]
														,flow->dl_src[5]);
		}

		if(wildcards&OFPFW_DL_DST)
		{
			sprintf(str,"dl_dst is %d.%d.%d.%d.%d.%d ",flow->dl_dst[0]
														,flow->dl_dst[1]
														,flow->dl_dst[2]
														,flow->dl_dst[3]
														,flow->dl_dst[4]
														,flow->dl_dst[5]);
		}

		if(wildcards&OFPFW_DL_TYPE)
		{
			sprintf(str,"dl_type is 0x%x ",flow->dl_type);
		}

		if(wildcards&OFPFW_NW_PROTO)
		{
			sprintf(str,"nw_proto is %d ",flow->nw_proto);
		}

		if(wildcards&OFPFW_TP_SRC)
		{
			sprintf(str,"tp_src is %d ",flow->tp_src);
		}
		
		if(wildcards&OFPFW_TP_DST)
		{
			sprintf(str,"tp_dst is %d ",flow->tp_dst);
		}
		
		if(wildcards&OFPFW_DL_VLAN_PCP)
		{
			sprintf(str,"dl_vlan_pcp is %d ",flow->dl_vlan_pcp);
		}
		
		if(wildcards&OFPFW_NW_TOS)
		{
			sprintf(str,"nw_tos is %d ",flow->nw_tos);
		}

		printf(str);
	}

}
	
int
cc_send_hello(struct cc_socket* cc_socket)
{
	struct ofp_header hello;
	hello.version = OFP_VERSION;
	hello.type = OFPT_HELLO;
	hello.length = htons(sizeof hello);
	hello.xid = 0;
}

static void
cc_recv_err_msg(c_switch_t *sw,struct cbuf *b)
{
	struct ofp_error_msg *ofp_err = (void*)(b->data);
	struct flow err_flow;
	char *str;
	struct ofp_flow_mod *ofm = (void*)(ofp_err->data);
	uint32_t wildcards = ofm->match.wildcards;

	printf("%s,dpid:0x%llx sent error msg,type %hu code %hu",FUNC_NAME,
		    sw->DPID,ntohs(ofp_err->type),ntohs(ofp_err->code));

	switch(ntohs(ofp_err->type))
	{
		case OFPET_HELLO_FAILED:         /* Hello protocol failed. */
    	case OFPET_BAD_REQUEST:         /* Request was not understood. */
    	case OFPET_BAD_ACTION:           /* Error in action description. */
    	case OFPET_PORT_MOD_FAILED:      /* Port mod request failed. */
    	case OFPET_QUEUE_OP_FAILED: 
			break;
		case OFPET_FLOW_MOD_FAILED:      /* Problem modifying flow entry. */
			{
				err_flow.in_port = ofm->match.in_port;
           		memcpy(err_flow.dl_src, ofm->match.dl_src, sizeof ofm->match.dl_src);
            	memcpy(err_flow.dl_dst, ofm->match.dl_dst, sizeof ofm->match.dl_dst);
            	err_flow.dl_vlan = ofm->match.dl_vlan;
            	err_flow.dl_type = ofm->match.dl_type;
            	err_flow.dl_vlan_pcp = ofm->match.dl_vlan_pcp;
            	err_flow.nw_src = ofm->match.nw_src;
            	err_flow.nw_dst = ofm->match.nw_dst;
            	err_flow.nw_proto = ofm->match.nw_proto;
            	err_flow.tp_src = ofm->match.tp_src;
            	err_flow.tp_dst = ofm->match.tp_dst;

				cc_dump_flow(&err_flow,wildcards);
				break;
			}
		default:
				break;
				
	}
}

static void
cc_recv_echo_request(c_switch_t *sw,struct cbuf *b)
{
}

static void
cc_recv_echo_reply(c_switch_t *sw,struct cbuf *b)
{
}

static void
cc_recv_features_reply(c_switch_t *sw,struct cbuf *b)
{
}

static void
cc_recv_packet_in(c_switch_t *sw,struct cbuf *b)
{
}

static void
cc_flow_removed(c_switch_t *sw,struct cbuf *b)
{
}

static void
cc_recv_port_status(c_switch_t *sw,struct cbuf *b)
{
}
	
struct of_handler of_handlers[] __aligned = {
    { cc_recv_hello_msg, OFP_HDR_SZ},                           /* OFPT_HELLO */
    { cc_recv_err_msg, sizeof(struct ofp_error_msg) },          /* OFPT_ERROR */
    { cc_recv_echo_request, OFP_HDR_SZ },                       /* OFPT_ECHO_REQUEST */
    { cc_recv_echo_reply, OFP_HDR_SZ },                         /* OFPT_ECHO_REPLY */
    NULL_OF_HANDLER,                                            /* OFPT_VENDOR */
    NULL_OF_HANDLER,                                            /* OFPT_FEATURES_REQUEST */
    { cc_recv_features_reply, OFP_HDR_SZ },                     /* OFPT_FEATURES_REPLY */
    NULL_OF_HANDLER,                                            /* OFPT_GET_CONFIG_REQUEST */
    NULL_OF_HANDLER,                                            /* OFPT_GET_CONFIG_REPLY */
    NULL_OF_HANDLER,                                            /* OFPT_SET_CONFIG */
    { cc_recv_packet_in, sizeof(struct ofp_packet_in) },        /* OFPT_PACKET_IN */
    { cc_flow_removed, sizeof(struct ofp_flow_removed) },       /* OFPT_FLOW_REMOVED */
    { cc_recv_port_status, sizeof(struct ofp_port_status) },    /* OFPT_PORT_STATUS */
    NULL_OF_HANDLER,                                            /* OFPT_PACKET_OUT */
    NULL_OF_HANDLER,                                            /* OFPT_FLOW_MOD */
    NULL_OF_HANDLER,                                            /* OFPT_PORT_MOD */
    NULL_OF_HANDLER,                                            /* OFPT_STATS_REQUEST */
    NULL_OF_HANDLER,                                            /* OFPT_STATS_REPLY */
    NULL_OF_HANDLER,                                            /* OFPT_BARRIER_REQUEST */
    NULL_OF_HANDLER,                                            /* OFPT_BARRIER_REPLY */
};
