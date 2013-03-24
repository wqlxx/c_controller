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

#ifndef BUFFER_H
#define BUFFER_H 1

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "cc_basic.h"
#include "cc_utility.h"



typedef struct buffer {
  void* data;
  void* top;
  void* tail;
  size_t length;
  size_t real_length;
  pthread_mutex_t* mutex;
  //void *user_data;
  //void ( *user_data_free_function )( struct buffer *buffer );
} buffer;

#define dlf_length sizeof(buffer)

static size_t front_length_of( buffer* );
static bool	already_allocated( buffer *, size_t);
buffer* append_front( buffer* , size_t); 
buffer* append_back( buffer* , size_t);
buffer* alloc_empty_data(void);
buffer *alloc_buffer( void );
buffer *alloc_buffer_with_length( size_t length );
void free_buffer( buffer *buf );
void *append_front_buffer( buffer *buf, size_t length );
void *remove_front_buffer( buffer *buf, size_t length );
void *append_back_buffer( buffer *buf, size_t length );
buffer *duplicate_buffer( const buffer *buf );
//void dump_buffer( const buffer *buf, void dump_function( const char *format, ... ) );

#endif // BUFFER_H
