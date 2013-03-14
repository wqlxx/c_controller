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

#include "cc_of_msg_recv.h"


static int
cc_insert_to_send_queue(sw_info* cc_sw_info,buffer* buf)
{
	int ret;
	buffer* msg;

	if( sw_info->send_queue == NULL )
		sw_info->send_queue = create_message_queue();

	ret = enqueue_message(cc_sw_info->send_queue, buf);

	return ret;
}


static int
cc_insert_to_app_queue(sw_info* cc_sw_info, buffer* buf)
{
	int ret;
	buffer* msg;

	if( sw_info->app_queue== NULL )
		sw_info->app_queue = create_message_queue();

	ret = enqueue_message(cc_sw_info->app_queue, buf);

	return ret;
}


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
	validate_hello(buf);
	if( ret < 0 )
	{
		free_buffer(buf);
		return CC_ERROR;
	}
	
	log_info_for_cc("recv a hello message");

	ret = cc_timer_start();

	buffer* send_buf;
	uint32_t xid = cc_generate_xid(cc_sw_info);
	send_buf = cc_create_features_request(xid);
	if( send_buf == NULL )
	{
		log_err_for_cc("cc_insert_to_send_queue error!");
		return CC_ERROR;
	}
	
	ret = cc_insert_to_send_queue(cc_sw_info, send_buf);
	if( ret < 0 )
	{
		log_err_for_cc("cc_insert_to_send_queue error!");
		free_buffer(send_buf);
		return CC_ERROR;
	}
	free_buffer(buf);
	return ret;
}


static int
cc_recv_err_msg(sw_info* cc_sw_info,buffer* buf)
{
	
	struct ofp_error_msg *ofp_err = (void*)(buf->data);
	struct flow err_flow;
	char *str;
	struct ofp_flow_mod *ofm = (void*)(ofp_err->data);
	uint32_t wildcards = ofm->match.wildcards;
	int ret;
	
	ret = validate_error(buf);
	if( ret < 0 )
	{
		free_buffer(buf);
		return CC_ERROR;
	}
	
	log_info_for_cc("recv a error message");

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
	free_buffer(buf);
	return CC_SUCCESS;
}


static int
cc_recv_echo_request(sw_info* cc_sw_info,buffer* buf)
{
	struct ofp_header *header = buf->data;
	uint32_t xid = htonl(header->xid);
	uint16_t length = htons( header->length);
	int ret;
	buffer* send_buf;

	ret = validate_echo_request(buf);
	if( ret < 0 )
	{
		free_buffer(buf);
		return CC_ERROR;
	}
	
	log_info_for_cc("recv a echo request message");

	cc_sw_info->xid = xid;// here we need to fix the bug
	
	send_buf = cc_create_echo_reply(xid, buf);
	if( send_buf == NULL )
	{
		log_err_for_cc("cc_insert_to_send_queue error!");
		return CC_ERROR;
	}
	
	ret = cc_insert_to_send_queue(cc_sw_info, send_buf);
	if( ret < 0 )
	{
		log_err_for_cc("cc_insert_to_send_queue error!");
		free_buffer(send_buf);
		return CC_ERROR;
	}	
	
	return ret;//here the xid should be correct!
}


static int
cc_recv_echo_reply(sw_info* cc_sw_info,buffer* buf)
{
	int ret;
	struct ofp_header* header = buf->data;
	buffer* send_buf;
	uint32_t xid;
	
	ret = validate_echo_reply(buf);
	if( ret < 0 )
	{
		free_buffer(buf);
		return CC_ERROR;
	}
	
	log_info_for_cc("recv a echo reply message");
	
	if( cc_lookup_xid_entry( header->xid )) {

		xid = cc_generate_xid(cc_sw_info);
		send_buf = cc_create_echo_request(xid, buf);
		if( send_buf == NULL )
		{
			log_err_for_cc("cc_insert_to_send_queue error!");
			return CC_ERROR;
		}
		
		ret = cc_insert_to_send_queue(cc_sw_info, send_buf);
		if( ret < 0 )
		{
			log_err_for_cc("cc_insert_to_send_queue error!");
			free_buffer(send_buf);
			return CC_ERROR;
		}	
		
		free_buffer(buf);
	}else{
		log_err_for_cc("transction id is not complete!");
		free_buffer(buf);
		return CC_ERROR;
	}
		
	return CC_SUCCESS;
}


static int
cc_recv_vendor(sw_info* cc_sw_info, buffer* buf)
{
	int ret;

	ret = validate_vendor(buf);
	if( ret < 0 )
	{
		free_buffer(buf);
		return CC_ERROR;
	}
	
	log_info_for_cc("recv a vendor msg");

	ret = cc_insert_to_app_queue(cc_sw_info, buf);
	if( ret < 0 )
	{
		log_err_for_cc("cc_insert_to_app_queue error!");
		free_buffer(buf);
		return CC_ERROR;
	}

	return CC_SUCCESS;
}


static int
cc_recv_get_config_reply(sw_info* cc_sw_info, buffer* buf)
{
	
	/* DO NOTHING*/
	int ret;
	uint32_t xid;
	buffer* send_buf = NULL;
	ret = validate_get_config_reply(buf);
	if( ret < 0 )
	{
		free_buffer(buf);
		return CC_ERROR;
	}
	
	log_info_for_cc("recv a get config reply");
	xid = cc_generate_xid(cc_sw_info);
	
	send_buf = cc_create_echo_request(xid , buf);
	if( send_buf == NULL )
	{
		log_err_for_cc("cc_create_echo_request error!");
		free_buffer(buf);
		return CC_ERROR;
	}
	
	ret = cc_insert_to_send_queue(cc_sw_info, send_buf);
	if( ret < 0 )
	{
		log_err_for_cc("cc_insert_echo_request error!");
		free_buffer(send_buf);
		return CC_ERROR;
	}
	
	return CC_SUCCESS;
	
}


static int
cc_recv_flow_removed(sw_info* cc_sw_info, buffer* buf)
{
	int ret;

	ret = validate_flow_mod(buf);
	if( ret < 0 )
	{
		free_buffer(buf);
		return CC_ERROR;
	}
	
	log_info_for_cc("recv a flow removed msg");
	ret = cc_insert_to_app_queue(cc_sw_info, buf);
	if( ret < 0 )
	{
		log_err_for_cc("cc_insert_to_app_queue error!");
		free_buffer(buf);
		return CC_ERROR;
	}
	return ret;
}


static int
cc_recv_barrier_reply(sw_info* cc_sw_info, buffer* buf)
{
	int ret;
	
	ret = validate_barrier_reply(buf);
	if( ret < 0 )
	{
		free_buffer(buf);
		return CC_ERROR;
	}
	
	log_info_for_cc("recv a barrier reply");
	ret = cc_insert_to_app_queue(cc_sw_info, buf);
	if( ret < 0 )
	{
		log_err_for_cc("cc_insert_to_app_queue error!");
		free_buffer(buf);
		return CC_ERROR;
	}
	return ret;
}


static void
cc_recv_features_reply(sw_info* cc_sw_info, buffer* buf)
{
	int ret;
	size_t n_ports;
	struct ofp_switch_features *feat_rep;
	uint64_t datapath_id;

	ret = validate_error(buf);
	if( ret < 0 )
	{
		free_buffer(buf);
		return CC_ERROR;
	}
	
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

	buffer* send_buf;
	uint32_t xid = cc_generate_xid(cc_sw_info);
	send_buf = cc_create_set_config(xid, cc_sw_info->flags, cc_sw_info->miss_send_len);
	if( send_buf == NULL )
	{
		log_err_for_cc("cc_create_set_config error!");
		return CC_ERROR;
	}
	ret = cc_insert_to_send_queue(cc_sw_info,send_buf);
	if( ret < 0 )
	{
		log_err_for_cc("cc_create_echo_request error!");
		free_buffer(send_buf);
		return CC_ERROR;
	}
	
	free_buf(buf);
	return ret;
}

static int
cc_recv_packet_in(sw_info* cc_sw_info,buffer* buf)
{
	int ret;

	ret = validate_packet_in(buf);
	if( ret < 0 )
	{
		free_buffer(buf);
		return CC_ERROR;
	}
	
	log_info_for_cc("recv packet-in msg from switch");
	ret = cc_insert_to_app_queue(cc_sw_info, buf);
	if( ret < 0 )
	{
		log_err_for_cc("cc_insert_to_app_queue error!");
		free_buffer(buf);
		return CC_ERROR;
	}

	return ret
}

static int
cc_recv_port_status(sw_info* cc_sw_info, buffer* buf)
{
	int ret;
	
	ret = validate_port_status(buf);
	if( ret < 0 )
	{
		free_buffer(buf);
		return CC_ERROR;
	}
	
	log_info_for_cc("recv a port status");
	//struct ofp_port_status *ops = (void*)(buf->data);
	//ret = cc_process_phy_port(cc_sw_info,&ops->desc,ops->reason);
	ret = cc_insert_to_app_queue(cc_sw_info, buf);
	if( ret < 0 )
	{
		log_err_for_cc("cc_insert_to_app_queue error!");
		free_buffer(buf);
		return CC_ERROR;
	}
	
	return ret;
}


static int
cc_recv_stats_reply(sw_info* cc_sw_info, buffer* buf)
{
	int ret;

	ret = validate_stats_reply(buf);
	if( ret < 0 )
	{
		free_buffer(buf);
		return CC_ERROR;
	}
	
	log_info_for_cc("recv a stats reply");
	ret = cc_insert_to_app_queue(cc_sw_info, buf);
	if( ret < 0 )
	{
		log_err_for_cc("cc_insert_to_app_queue error!");
		free_buffer(buf);
		return CC_ERROR;
	}

	return ret;
}


int
cc_recv_flow_stats_reply(sw_info* cc_sw_info, buffer* buf)
{
	int ret;

	ret = validate_flow_stats_reply(buf);
	if( ret < 0 )
	{
		free_buffer(buf);
		return CC_ERROR;
	}
	
	log_info_for_cc("recv a flow stats reply");
	ret = cc_insert_to_app_queue(cc_sw_info, buf);
	if( ret < 0 )
	{
		log_err_for_cc("cc_insert_to_app_queue error!");
		free_buffer(buf);
		return CC_ERROR;
	}

	return ret;
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

#if 0
const struct of_handler_class of10_handler = {
	0x01,
	cc_recv_hello_msg,
	cc_recv_features_reply,
	cc_recv_echo_request,
	cc_recv_echo_reply,
	cc_recv_vendor,
	cc_recv_get_config_reply,
	cc_recv_flow_removed,
	cc_recv_stats_reply,
	cc_recv_barrier_reply,
	cc_recv_packet_in,
	cc_recv_err_msg,
	cc_recv_port_status
};	
#endif
