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

#include "cc_basic.h"


static int
cc_init_sw_info_table(list_element* sw_info_table)
{
	list_element* header = malloc(sizeof(list_element));
	if( !(create_list(&sw_info_table)) ){		
		log_info_for_cc("create sw info table success!");
		return CC_ERROR;	
	}
	log_info_for_cc("create sw info table success!");
	return CC_SUCCESS;
}


static int
cc_insert_sw_info(list_element* sw_info_table, sw_info* cc_sw_info)
{
	bool ret;

	ret = append_to_tail(&sw_info_table,(void*)cc_sw_info);
	if( !ret )
	{
		log_err_for_cc("insert sw info failed!");
		return CC_ERROR;
	}

	return CC_SUCCESS;		
}


/*used to been called by find_list_custom in cc_find_sw_info
*/
bool
cc_find_sw_dpid(void* data, void* user_data)
{
	sw_info* tmp_info = (sw_info*)data;
	uint64_t* tmp_dpid_ = (uint64_t*)user_data;
	
	if( tmp_info->cc_switch->dpid == tmp_dpid_ )
		return true;

	return false;
}


sw_info*
cc_find_sw_info(list_element* sw_info_table, uint64_t dpid)
{
	sw_info* find_sw_info;
	
	find_sw_info = find_list_custom(sw_info_table, cc_find_sw_dpid, &dpid);

	return find_sw_info;
}


static int
cc_delete_sw_info(list_element* sw_info_table, sw_info* cc_sw_info)
{
	bool ret;

	ret = delete_element(&sw_info_table, (void*)cc_sw_info);
	if( !ret )
	{
		log_err_for_cc("delete sw info failed!");
		return CC_ERROR;
	}
	
	return CC_SUCCESS;
}


static int
cc_finalize_sw_info_table(list_element* sw_info_table)
{
	delete_list(sw_info_table);
	return CC_SUCCESS;
}

