/*
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


#include <assert.h>
#include <openflow.h>
#include <string.h>
//#include "trema.h"
#include "cc_xid.h"


uint32_t
cc_generate_xid(sw_info* cc_sw_info) {
  uint32_t initial_value = ( cc_sw_info->xid_latest != UINT32_MAX ) ? ++cc_sw_info->xid_latest : 0;

  while ( cc_lookup_xid_entry( cc_sw_info->xid_latest ) != NULL ) {
    if ( cc_sw_info->xid_latest != UINT32_MAX ) {
      cc_sw_info->xid_latest++;
    }
    else {
      cc_sw_info->xid_latest = 0;
    }
    if ( initial_value == cc_sw_info->xid_latest ) {
      error( "Failed to generate transaction id value." );
      cc_sw_info->xid_latest = 0;
      break;
    }
  }

  return cc_sw_info->xid_latest;
}


xid_entry*
cc_allocate_xid_entry( uint32_t original_xid, char *service_name, int index ) {
  xid_entry *new_entry;

  new_entry = (xid_entry*)malloc( sizeof( xid_entry ) );
  new_entry->xid = cc_generate_xid();
  new_entry->original_xid = original_xid;
  memcpy(new_entry->service_name, service_name , strlen(service_name)+1);
  new_entry->index = index;

  return new_entry;
}


static void
cc_free_xid_entry( xid_entry *free_entry ) {
  free( free_entry->service_name );
  free( free_entry );
}


void
cc_init_xid_table( sw_info* cc_sw_info ) {
  memset( &cc_sw_info->xid_table_, 0, sizeof( xid_table ) );
  cc_sw_info->xid_table_->hash = create_hash( compare_uint32, hash_uint32 );
  cc_sw_info->xid_table_->next_index = 0;
}


void
cc_finalize_xid_table( sw_info* cc_sw_info ) {
  for ( int i = 0; i < CC_XID_MAX_ENTRIES; i++ ) {
    if ( (cc_sw_info->xid_table_->entries)[ i ] != NULL ) {
      cc_free_xid_entry( (cc_sw_info->xid_table_->entries)[ i ] );
      (cc_sw_info->xid_table_->entries)[ i ] = NULL;
    }
  }
  delete_hash( cc_sw_info->xid_table_->hash );
  cc_sw_info->xid_table_->hash = NULL;
  cc_sw_info->xid_table_->next_index = 0;
}


uint32_t
cc_insert_xid_entry( sw_info* cc_sw_info, uint32_t original_xid, char *service_name ) {
  xid_entry *new_entry;

  log_info_for_cc("insert xid entry");

  if ( cc_sw_info->xid_table_->next_index >= CC_XID_MAX_ENTRIES ) {
    cc_sw_info->xid_table_->next_index = 0;
  }

  if ( cc_sw_info->xid_table_->entries[ cc_sw_info->xid_table_->next_index ] != NULL ) {
    cc_delete_xid_entry( cc_sw_info->xid_table_->entries[ cc_sw_info->xid_table_->next_index ] );
  }

  new_entry = cc_allocate_xid_entry( original_xid, service_name, cc_sw_info->xid_table_->next_index );
  xid_entry *old = insert_hash_entry( cc_sw_info->xid_table_->hash, &new_entry->xid, new_entry );
  if ( old != NULL ) {
    (cc_sw_info->xid_table_->entries)[ old->index ] = NULL;
    cc_free_xid_entry( old );
  }
  (cc_sw_info->xid_table_->entries)[ cc_sw_info->xid_table_->next_index ] = new_entry;
  cc_sw_info->xid_table_->next_index++;

  return new_entry->xid;
}


void
cc_delete_xid_entry( sw_info* cc_sw_info, xid_entry *delete_entry ) {

  log_info_for_cc("delete xid entry");
  xid_entry *deleted = delete_hash_entry( cc_sw_info->xid_table_->hash, &delete_entry->xid );

  if ( deleted == NULL ) {
    log_err_for_cc( "Failed to delete xid entry ");
    cc_free_xid_entry( ( xid_entry * ) delete_entry );
    return;
  }

  cc_sw_info->xid_table_->entries[ deleted->index ] = NULL;
  cc_free_xid_entry( deleted );
}


int
cc_lookup_xid_entry( sw_info* cc_sw_info, uint32_t xid )
{

  if( lookup_hash_entry( cc_sw_info->xid_table_->hash, &xid )== NULL )
  	return CC_ERROR;

  return CC_SUCCESS;
}


static void
cc_dump_xid_entry( xid_entry *entry ) {
/*
  log_info_for_cc( "xid = %#" PRIx32 ", original_xid = %#" PRIx32 ", service_name = %s, index = %d",
        entry->xid, entry->original_xid, entry->service_name, entry->index );
*/
}


void
cc_dump_xid_table(sw_info* cc_sw_info) {
  hash_iterator iter;
  hash_entry *e;

  log_info_for_cc( "#### XID TABLE ####" );
  init_hash_iterator( cc_sw_info->xid_table_->hash, &iter );
  while ( ( e = iterate_hash_next( &iter ) ) != NULL ) {
    cc_dump_xid_entry( e->value );
  }
  log_info_for_cc( "#### END ####" );
}


/*
 * Local variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */
