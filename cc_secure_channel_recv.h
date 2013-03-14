/*
 * Utility functions.
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

#ifndef CC_SECURE_CHANNLE_RECV_H
#define CC_SECURE_CHANNEL_RECV_H 1

#include "cc_basic.h"
#include "cc_of_msg_recv.h"

struct cc_writev_args {
	struct iovec *iov;
	int iovcnt;
};
typedef struct cc_writev_args cc_writev_args;

typedef int (*FUNC_CB)(sw_info*, buffer*);

FUNC_CB cc_select_handler(uint16_t );

static int cc_ofpmsg_handle(sw_info*, buffer*);

static int cc_recv_from_secure_channel(sw_info* );

static int cc_secure_channle_read(sw_info* );

static int cc_append_to_writev_args( buffer *, void *);

static int cc_send_to_secure_channel(sw_info* ,buffer*);

static int cc_flush_to_secure_channel(sw_info* cc_sw_info)

#endif

