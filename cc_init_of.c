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
	if( !(create_list(&sw_info_table)) ){		
		log_info_for_cc("create sw info table success!");
		return CC_ERROR;	
	}
	log_info_for_cc("create sw info table success!");
	return CC_SUCCESS;
}


static int
cc_init_of_socket(cc_socket* cc_socket_)
{
	int ret;

	ret = cc_init_listenfd(cc_socket_);
	if( ret < 0 )
	{
		log_err_for_cc("create fd error!");
		return CC_ERROR;
	}
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
cc_polling(list_element* sw_info_table, cc_socket* cc_socket_)
{
	int ret;
	fd_set listen_fdset;
	int max_fd = cc_socket->fd + 1;

	FD_ZERO(listen_fdset);
	FD_SET(&cc_socket->fd, listen_fdset);
	while(1)
	{
		
		FD_ZERO(&listen_fdset);
		FD_SET(cc_socket->fd,&listen_fdset);
		ret = select(max_fd,&listen_fdset,NULL,NULL,0);
		if( ret == -1 )
		{
			if( errno == EINTR )				
				continue;
			else
				return CC_ERROR;
		}else if( ret == 0 ){
			continue;
		}else{
			if(FD_ISSET(cc_socket->fd, &listen_fdset))
			{
				sw_info *cc_sw_info = (sw_info*)malloc(sizeof(sw_info));
				ret = cc_conn_accept(cc_socket_ , cc_sw_info);
				if( ret < 0 ){
					log_err_for_cc("accept failed!");
					return CC_ERROR;
				}
				ret = cc_insert_sw_info(sw_info_table, cc_sw_info);
				if( ret < 0 )
					return CC_ERROR;
			}
		}
	}

	return CC_SUCCESS;
}

static int
cc_finalize_of(list_element* sw_info_table, cc_socket* cc_socket_)
{
	if( !(delete_list(sw_info_table)))
		return CC_ERROR;
	close(cc_socket_->fd);
	free(cc_socket_);
	
	return CC_SUCCESS;
}

#if 0
static void
cc_app_init(const char** app_name,int num)
{
	int ret;
	int i;
	
	for(i = 0; i < num; i++)
	{
		ret = cc_app_register(*app_name);
		if(ret < 0)
		{
			log_err_for_cc("register app error!");
		}
	}
}


void
cc_app_run(struct cc_app* cc_app)
{
	
}
#endif
