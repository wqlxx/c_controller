/*
 * cc_memory_pook functions.
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
#ifndef CC_MEM_POOL_H
#define CC_MEM_POOL_H 1

typedef struct
{
	int unitsize;	//size of memory 
	int initnum;	//init num of memory unit
	int grownum;	//num of added memory
	int totalnum;	//the whole num of memory unit
	memblock_t* block;	//the head of link called memblock_t
	char* idleunit;		//the head point to the link of idle memory unit
#if define(__MEMPOOL_THREAD_SUPPORT__)
	pthread_mutex_t lock;
#endif 
}mempool_t;


/*the struct of memory block
 */
typedef struct
{
	int unitnum;	//the total num of memory unit
	int idlenum;	//the num of idle memory unit
	mempool_t *pool;	//the pool which this unit belong to
	char *lastunit;		//the end address of this unit
	memblock_t	*next;	//the next memblock_t
}memblock_t;

/*the struct of memory unit
 */
 typedef struct
{
	memblock_t *block;	//the memory block which this unit belong to
	char *next;			//the address of next memory
}memunit_info_t;

#endif
