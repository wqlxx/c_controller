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
#include "openflow.h"
#include <string.h>
//#include "trema.h"
#include "cc_xid.h"


uint32_t
cc_generate_xid(sw_info* cc_sw_info) {
  uint32_t initial_value = ( cc_sw_info->xid_latest != UINT32_MAX ) ? ++cc_sw_info->xid_latest : 0;

  while ( cc_lookup_xid_entry( cc_sw_info->xid_table_, initial_value) >= 0 ) {
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
cc_allocate_xid_entry(sw_info* cc_sw_info ) {
  xid_entry *new_entry;

  new_entry = (xid_entry*)malloc( sizeof( xid_entry ) );
  new_entry->xid = cc_generate_xid(cc_sw_info);
  gettimeofday(new_entry->tv,NULL);  

  return new_entry;
}


static void
cc_free_xid_entry( xid_entry *free_entry ) {
  free( free_entry );
}


static void
cc_free_xid_entry_(void* data, void* user_data)
{
	free(data);
}


void
cc_init_xid_table( sw_info* cc_sw_info ) {

  log_debug_for_cc("cc_init_xid_table_");
  //memset( &cc_sw_info->xid_table_, 0, sizeof( xid_table ) );
  cc_sw_info->xid_table_ = (xid_table*)malloc(cc_sw_info->xid_table_);
  cc_sw_info->xid_table_->xid_hash = create_hash( compare_uint32, hash_uint32 );
  list_element* header = malloc(sizeof(list_element));
  cc_sw_info->xid_table_->xid_entry_list = NULL;
  return CC_SUCCESS;
}

void
cc_finalize_xid_table( sw_info* cc_sw_info )
{
	iterate_list(cc_sw_info->xid_table_->xid_entry_list, cc_free_xid_entry_, NULL);
  	delete_hash( cc_sw_info->xid_table_->xid_hash);
	delete_list( cc_sw_info->xid_table_->xid_entry_list);
}


uint32_t
cc_insert_xid_entry( sw_info* cc_sw_info ) {
	xid_entry *new_entry;
	int ret;
 	log_info_for_cc("insert xid entry");

	new_entry = cc_allocate_xid_entry(cc_sw_info);
	list_element* new_list_e = malloc(sizeof(list_element));
	if( cc_sw_info->xid_table_->xid_entry_list == NULL )
	{
		new_list_e->data = (void*)new_entry;
		new_list_e->next = NULL;
		cc_sw_info->xid_table_->xid_entry_list = new_list_e;
	}else{ 
		ret = append_to_tail( &(cc_sw_info->xid_table_->xid_entry_list), new_entry);
		if( ret < 0 )
		{
			log_err_for_cc("ppend_to_tail fail");
			return CC_ERROR;
		}
	}

	insert_hash_entry(cc_sw_info->xid_table_->xid_hash, &(new_entry->xid), new_entry);

 	return new_entry->xid;
}

bool
compare_xid_entry(const void* entry, const void* find_entry)
{
	xid_entry *entry_ = (xid_entry*)entry;
	xid_entry *find_entry_ = (xid_entry*)find_entry;

	if( entry_->xid != find_entry_->xid )
		return false;

	return true;
}


int
cc_delete_xid_entry( sw_info* cc_sw_info, xid_entry *delete_entry ) 
{

  	log_info_for_cc("delete xid entry");
  	xid_entry *deleted = delete_hash_entry( cc_sw_info->xid_table_->xid_hash, &delete_entry->xid );

  	if ( deleted == NULL ) {
    	log_err_for_cc( "Failed to delete xid entry ");
    	cc_free_xid_entry( ( xid_entry * ) delete_entry );
		return CC_ERROR;
 	}

	
	deleted = find_list_custom(cc_sw_info->xid_table_->xid_entry_list, compare_xid_entry, delete_entry);
	delete_element(&(cc_sw_info->xid_table_->xid_entry_list), deleted);
 	cc_free_xid_entry( deleted );

	return CC_SUCCESS;
}


int
cc_lookup_xid_entry( sw_info* cc_sw_info, uint32_t xid )
{

  if( lookup_hash_entry( cc_sw_info->xid_table_->hash, &xid ) == NULL )
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
