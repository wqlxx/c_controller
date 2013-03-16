/*
 * OpenFlow Switch Manager
 *
 * Author: Kazushi SUGYO
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


#ifndef CC_XID_H
#define CC_XID_H 1

#include <sys/times.h>
#include "cc_basic.h"


uint32_t cc_generate_xid( sw_info* );

xid_entry* cc_allocate_xid_entry( uint32_t , char *, int);

static void cc_free_xid_entry(xid_entry *);

void cc_init_xid_table( sw_info* );

void cc_finalize_xid_table( sw_info* );

uint32_t cc_insert_xid_entry( uint32_t , char *);

void cc_delete_xid_entry( xid_entry *);

xid_entry *cc_lookup_xid_entry( uint32_t );

static void cc_dump_xid_entry(xid_entry *);

void cc_dump_xid_table( sw_info* );


#endif // CC_XID_H


/*
 * Local variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */

