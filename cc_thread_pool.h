/*
 * cc_thread_pool functions.
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
 
#ifndef CC_THREAD_POOL
#define CC_THREAD_POOL 1

#include <unistd.h>   
#include <sys/types.h>   
#include <pthread.h>   
#include <assert.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>  
#include <sys/stat.h>
#include "cc_basic.h"


void pool_init (CThread_pool* pool,int max_thread_num);
int pool_add_worker (CThread_pool* pool,void *(*process) (void *arg, void *arg));   
void *thread_routine (CThread_pool* pool,void *arg); 
int pool_destroy (CThread_pool* pool);

#endif
