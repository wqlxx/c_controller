/*
 * cc_of_msg_action functions.
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
#ifndef CC_INIT_OF_H
#define CC_INIT_OF_H 1

#include "cc_socket.h"
#include "cc_of_handler.h"
#include "cc_switch_table.h"
#include "cc_basic_struct.h"

typedef void (*event_handler_callback)(int ,void* data);

extern static int cc_init_event_handler(int fd,event_handler_callback read_callback,void * read_data,
						event_handler_callback write_callbacl,void * write_data)
extern static int cc_init_of();
extern static int cc_init_app();

#endif

