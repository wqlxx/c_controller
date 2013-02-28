#include "cc_of_handler.h"

static inline uint32_t
make_inet_mask(uint8_t len)
{
    return (~((1 << (32 - (len))) - 1));
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

static int
cc_recv_hello_msg(sw_info* cc_sw_info,buffer* buf)
{
	int ret;
	log_info_for_cc("recv a hello message");
	free_buffer(buf);
	
	return CC_SUCCESS;
}

int
cc_send_hello_msg(sw_info* cc_sw_info)
{
	int ret;
	buffer* buf;
	
	struct ofp_header hello;
	hello.version = OFP_VERSION;
	hello.type = OFPT_HELLO;
	hello.length = htons(sizeof hello);
	hello.xid = 0;

	buffer* buff;
	buff->data = &hello;
	buff->length = sizeof(hello);

	/*
	ssize_t ret;
	ret = write(cc_sw_info->cc_switch->cc_socket->fd,buff,sizeof(buff));
	if(ret != sizeof(hello))
	{
		perror("send of hello failed");
		log_err_for_cc("send of hello failed");
		return CC_ERROR;
	}
	*/
	cc_send_to_secure_channel(cc_sw_info,buff);
	return CC_SUCCESS;
}


/*
int
cc_send_err_msg(sw_info* cc_sw_info,uint16_t type,uint16_t code,buffer* b)
{
	int ret;
	buffer* buff;

	if( 
}
*/


static int
cc_recv_err_msg(sw_info* cc_sw_info,buffer* buf)
{
	
	struct ofp_error_msg *ofp_err = (void*)(buf->data);
	struct flow err_flow;
	char *str;
	struct ofp_flow_mod *ofm = (void*)(ofp_err->data);
	uint32_t wildcards = ofm->match.wildcards;

	printf("%s,dpid:0x%llx sent error msg,type %hu code %hu",FUNC_NAME,
		    cc_sw_info->cc_switch->dpid,ntohs(ofp_err->type),ntohs(ofp_err->code));

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
	return CC_SUCCESS;
}


static int
cc_recv_echo_request(sw_info* cc_sw_info,buffer* buf)
{
	struct ofp_header *header = buf->data;
	uint32_t transaction_id = htonl(header->xid);
	uint16_t length = htons( header->length);

	cc_sw_info->xid = transaction_id;
	//remove_front_buffer(data,sizeof(struct ofp_header));
	buffer* body = NULL;
	if((length - sizeof(struct ofp_header))>0)
	{
		body = duplicate_buffer(buf);
		remove_front_buffer(body,sizeof(struct ofp_header));
	}	

	if( body != NULL )
		free_buffer(body);

	return cc_send_echo_reply(cc_sw_info,header->xid);//here the xid should be correct!
}


static int
cc_recv_echo_reply(sw_info* cc_sw_info,buffer* buf)
{
	/*Nothing to do*/
}

static int
cc_process_phy_port(sw_info* cc_sw_info, struct ofp_phy_port* port, uint8_t reason)
{
	
	switch(reason){
			
		case OFPPR_DELETE:
			cc_sw_info->cc_switch.sw_port[port->port_no].valid = 0;

		case OFPPR_MODIFY:
			
		case OFPPR_ADD:
			cc_sw_info->cc_switch.sw_port[port->port_no].port= *port;
			cc_sw_info->cc_switch.sw_port[port->port_no].valid = 1;
		default:
			return CC_ERROR;
	}
	return CC_SUCCESS;
			
}

static void
cc_recv_features_reply(sw_info* cc_sw_info,buffer* buf)
{
	int ret;
	size_t n_ports;
	struct ofp_switch_features *feat_rep;
	uint64_t datapath_id;

	log_info_for_cc("get the feature reply from the switch");
	feat_rep = (void*)buf->data;

	cc_sw_info->cc_switch->dpid		 = ntohll(feat_rep->datapath_id);
	cc_sw_info->cc_switch->version 	 = ntohl(feat_rep->header->version);
	cc_sw_info->cc_switch->n_buffers = ntohs(feat_rep->n_buffers);
	cc_sw_info->cc_switch->actions   = ntohl(feat_rep->actions);
	cc_sw_info->cc_switch->capabilities = ntohl(feat_rep->capabilities);
	
	n_ports = ((ntohs(feat_rep->header.length)
                - offsetof(struct ofp_switch_features, ports))
            / sizeof *feat_rep->ports);
	if(n_ports > CC_MAX_PORT)
	{
		log_err_for_cc("the port num of switch is out of range of the max mount");
		n_ports = n_ports < CC_MAX_PORT ? n_ports : CC_MAX_PORT; 
	}
	
	for(int i = 0; i < n_ports; i++)
	{
		cc_process_phy_port(cc_sw_info,&feat_rep->ports[i],OFPPR_ADD);
	}

	free_buffer(buf);
	return ;
}

static void
cc_recv_packet_in(sw_info* cc_sw_info,buffer* buf)
{
	log_info_for_cc("recv packet-in msg from switch");
	//cc_app_handler_run();
	return;
}

static void
cc_recv_port_status(sw_info* cc_sw_info, buffer* buf)
{
	struct ofp_port_status *ops = (void*)(buf->data);
	cc_process_phy_port(cc_sw_info,&ops->desc,ops->reason);
	//cc_app_handler_run();
	return;
}

static void
cc_flow_removed(sw_info* cc_sw_info,buffer* buf)
{
	struct flow                 flow;
    struct ofp_flow_removed     *ofm = (void *)(b->data);
    struct of_flow_mod_params   fl_parms;

    memset(&fl_parms, 0, sizeof(fl_parms));
    memset(&flow, 0, sizeof(flow));

    fl_parms.wildcards = ofm->match.wildcards;
    fl_parms.prio = ntohs(ofm->priority);

    flow.in_port = ofm->match.in_port;
    memcpy(flow.dl_src, ofm->match.dl_src, sizeof ofm->match.dl_src);
    memcpy(flow.dl_dst, ofm->match.dl_dst, sizeof ofm->match.dl_dst);
    flow.dl_vlan = ofm->match.dl_vlan;
    flow.dl_type = ofm->match.dl_type;
    flow.dl_vlan_pcp = ofm->match.dl_vlan_pcp;
    flow.nw_src = ofm->match.nw_src;
    flow.nw_dst = ofm->match.nw_dst;
    flow.nw_proto = ofm->match.nw_proto;
    flow.tp_src = ofm->match.tp_src;
    flow.tp_dst = ofm->match.tp_dst;

    fl_parms.flow = &flow;    
    fl_parms.tbl_idx = C_RULE_FLOW_TBL_DFL;
    
    //c_signal_app_event(sw, b, C_FLOW_REMOVED, NULL, &fl_parms);
    return;
}


struct of_handler of_handlers[] __aligned = {
    cc_recv_hello_msg,              /* OFPT_HELLO */
    cc_recv_err_msg,        		/* OFPT_ERROR */
	cc_recv_echo_request,           /* OFPT_ECHO_REQUEST */
    cc_recv_echo_reply,             /* OFPT_ECHO_REPLY */
    NULL,                			/* OFPT_VENDOR */
    NULL,               			/* OFPT_FEATURES_REQUEST */
    cc_recv_features_reply,         /* OFPT_FEATURES_REPLY */
    NULL,                			/* OFPT_GET_CONFIG_REQUEST */
    NULL,                			/* OFPT_GET_CONFIG_REPLY */
    NULL,                			/* OFPT_SET_CONFIG */
    cc_recv_packet_in,        		/* OFPT_PACKET_IN */
    cc_flow_removed,      			/* OFPT_FLOW_REMOVED */
    cc_recv_port_status,   			/* OFPT_PORT_STATUS */
    NULL,                			/* OFPT_PACKET_OUT */
    NULL,                			/* OFPT_FLOW_MOD */
    NULL,                			/* OFPT_PORT_MOD */
    NULL,                			/* OFPT_STATS_REQUEST */
    NULL,                			/* OFPT_STATS_REPLY */
    NULL,                			/* OFPT_BARRIER_REQUEST */
    NULL,                			/* OFPT_BARRIER_REPLY */
};


