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

#include "cc_basic.h"

static int cc_init_sw_info_table(list_element* );

static int cc_init_of_socket(cc_socket* );

static int cc_insert_sw_info(list_element* , sw_info* );

static int cc_delete_sw_info(list_element* , sw_info* );

static int cc_polling(list_element* , cc_socket* );

static int cc_finalize_of(list_element* , cc_socket* );

#endif

