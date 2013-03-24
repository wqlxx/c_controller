/*
 * cc_secure_channel_send functions.
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

#ifndef CC_SECURE_CHANNEL_SEND_H
#define CC_SECURE_CHANNEL_SEND_H 1

#include "cc_basic.h"
#include "cc_of_msg_send.h"
#include "queue.h"
#include "cc_buffer.h"

typedef int (*FUNC_CB_S)(sw_info* , buffer*);

FUNC_CB_S  cc_select_send_func(uint16_t type);

static int cc_send_to_secure_channel(sw_info* cc_sw_info,buffer* buf);

static int cc_send_to_secure_channel_app(sw_info* cc_sw_info,buffer* buf);

static int cc_flush_to_secure_channel(sw_info* cc_sw_info);

#endif
