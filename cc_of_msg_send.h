/*
 * OpenFlow Switch Manager
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


#ifndef OFPMSG_SEND_H
#define OFPMSG_SEND_H 1



#include "cc_basic.h"
#include "cc_of_msg_prep.h"
#include "cc_secure_channel.h"

#define OFP_ERROR_MSG_MAX_DATA 64

int cc_send_hello( void );

int cc_send_echo_request( sw_info* , buffer*); 

int cc_send_echo_reply(sw_info* ,buffer*); 

int cc_send_features_request( sw_info* ,buffer*);

int cc_send_get_config_request(sw_info* , buffer*);

int cc_send_set_config(sw_info*, buffer*);

int cc_send_error_msg( sw_info* , uint16_t, uint16_t, buffer*);

int cc_senf_packet_out(sw_info* , buffer* );

int cc_send_port_mod(sw_info* , buffer* );

int cc_send_stats_request(sw_info* , buffer* );

int cc_send_get_desc_stats(sw_info* , buffer* );

int cc_send_flow_stats_request(sw_info* , buffer* );

int cc_send_aggregate_stats_request(sw_info* , buffer* );

int cc_send_table_stats_request(sw_info* , buffer* );

int cc_send_port_stats_request(sw_info* , buffer* );

int cc_send_queue_stats_request(sw_info* , buffer* );

int cc_send_vendor_stats_request(sw_info* , buffer* );

int cc_send_msg_to_switch(sw_info* , buffer* );

static int update_flowmod_cookie( buffer *, char * );

#if 0
struct of_msg_send_class {

	const uint8_t version;
	
	int (*cc_send_hello)(void *);
	
	int (*cc_send_echo_request)(buffer *);
	
	int (*cc_senf_echo_reply)(buffer *);
	
	int (*cc_send_features_request)(buffer *);
	
	int (*cc_send_set_config)(buffer *);
	
	int (*cc_send_error_msg)(buffer *);

};

extern struct of_msg_send_class of_msg_send;
#endif





#endif // OFPMSG_SEND_H


/*
 * Local variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */


