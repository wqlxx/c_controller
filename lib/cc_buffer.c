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

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buffer.h"
#include "checks.h"
#include "utility.h"
#include "wrapper.h"


static size_t
front_length_of( buffer *buf )
{
  return ( size_t ) ( ( char * ) buf->data - ( char * ) buf->top );
}


static bool
already_allocated( buffer *buf, size_t length )
{
	size_t front_len,required_len;

	front_len = ( size_t ) front_length_of( buf );
  	required_len = front_len + buf->length + length;

  	return ( buf->real_length >= required_len );
}


static buffer *
alloc_new_data(buffer* buf,size_t length = dlf_length)
{
  
  buf->data= (void*)malloc( length );
  buf->length = length;
  buf->top = buf->data;
  buf->real_length = length;
  buf->tail = buf->data + length;

  new_buf->mutex = (pthread_mutex_t*)malloc( sizeof( pthread_mutex_t ) );
  pthread_mutex_init( buff->mutex, NULL );

  return buf;
}


static buffer *
append_front( buffer* buf, size_t length ) 
{

  	size_t new_length = front_length_of( buf ) + buf->length + length;
  	void *new_data = malloc( new_length );
  	if(new_data == NULL)
  	{
  		return NULL;
  	}
	
  	memcpy( ( char * ) new_data + front_length_of( buf ) + length, buf->data, buf->length );
  	free( buf->top );

  	buf->data = ( char * ) new_data + front_length_of( buf );
  	buf->real_length = new_length;
  	buf->top = new_data;
  	buf->tail = new_data + new_length - 1;

  	return buf;
}


static buffer *
append_back( buffer* buf, size_t length )
{
	size_t len;
	len = front_length_of( buf );
  	size_t new_length = len + buf->length + length;
  	void *new_data = malloc( new_length );
  	if(new_data == NULL)
  	{
  		return NULL;
  	}
  	memcpy( ( char * ) new_data + len, buf->data, buf->length );
  	//free the memory alloced before
  	free( buf->top );

  	buf->data = ( char* ) new_data + front_length_of( buf );
  	buf->real_length = new_length;
  	buf->top = ( char* )new_data;
  	buf->tail = ( char* )new_data + new_length - 1;
  
  	return buf;
}


buffer*
alloc_empty_data()
{
	buffer* buf = (buffer*)malloc(sizeof(buffer));
	buf->data = NULL;
	buf->top = buf->data;
	buf->tail = buf->data;
	buf->length = 0;
	buf->real_length = 0;

	//pthread_mutexattr_t attr;
 	//pthread_mutexattr_init( &attr );
  	//pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE_NP );
  	buf->mutex = malloc( sizeof( pthread_mutex_t ) );
  	pthread_mutex_init( buf->mutex, NULL );

	return buf;
}

buffer*
alloc_buffer() 
{
	buffer* buf;
	buf = alloc_empty_data();
	return buf;
}


buffer *
alloc_buffer_with_length( size_t length ) 
{

  buffer *new_buf = malloc( sizeof( buffer ) );
  new_buf->data = malloc( length );
  new_buf->length = length;
  new_buf->top = new_buf->data;
  new_buf->real_length = length;
  new_buf->length = length;
  new_buf->tail = new_buf->top + length;
  
  //pthread_mutexattr_t attr;
  //pthread_mutexattr_init( &attr );
  //pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE_NP );
  new_buf->mutex = malloc( sizeof( pthread_mutex_t ) );
  pthread_mutex_init( new_buf->mutex, NULL);

  return new_buf;
}


void
free_buffer( buffer *buf ) {
  
  pthread_mutex_lock( buf ->mutex );
  if ( buf->top != NULL ) {
    free( buf->top );
  }
  pthread_mutex_unlock( buf->mutex );
  pthread_mutex_destroy( buf->mutex );
  free( buf->mutex );
  free( buf );
}


void *
append_front_buffer( buffer *buf, size_t length ) 
{

  pthread_mutex_lock( buf->mutex );

  if ( buf->top == NULL ) {
    alloc_new_data( buf, length );
    pthread_mutex_unlock( buf->mutex );
    return buf->data;
  }
  
  if ( already_allocated( buf, length ) ) {
    memmove( ( char * ) buf->data + length, buf->data, buf->length );
    memset( buf->data, 0, length );
  }
  else {
    append_front( buf, length );
  }
  //buf->length += length;

  pthread_mutex_unlock( buf->mutex );

  return buf->data;
}


void *
remove_front_buffer( buffer *buf, size_t length )
{

  pthread_mutex_lock( buf->mutex );

  buf->data = ( char * ) buf->data + length;
  buf->length -= length;  

  pthread_mutex_unlock( buf->mutex );

  return buf->data;
}


void *
append_back_buffer( buffer *buf, size_t length ) 
{

  pthread_mutex_lock( buf->mutex );

  if ( buf->real_length == 0) {
    alloc_new_data( buf, length );
    pthread_mutex_unlock( buf->mutex );
    return ( char * ) buf->data;
  }

  if ( !already_allocated( buf, length ) ) {
    append_back( buf, length );
  }

  //void *appended = ( char * ) buf->data + buf->length;
  buf->length += length;

  pthread_mutex_unlock( buf->mutex );

  return buf->data;
}


buffer *
duplicate_buffer( const buffer *buf)
{

  pthread_mutex_lock( buf->mutex );

  buffer *new_buffer = alloc_buffer();
  const buffer *old_buffer = ( const buffer * ) buf;

  if ( old_buffer->real_length == 0 ) {
    pthread_mutex_unlock( old_buffer->mutex );
    return NULL;
  }

  alloc_new_data( new_buffer, old_buffer->real_length );
  memcpy( new_buffer->top, old_buffer->top, old_buffer->real_length );

  new_buffer->length = old_buffer->length;
  new_buffer->real_length = old_buffer->real_length;
  new_buffer->data = new_buffer->top + front_length_of(old_buffer);
  new_buffer->tail = new_buffer->top + new_buffer->length - 1;

  //pthread_mutexattr_t attr;
  //pthread_mutexattr_init( &attr );
  //pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE_NP );
  new_buffer->mutex = malloc( sizeof( pthread_mutex_t ) );
  pthread_mutex_init( new_buffer->mutex, NULL );
  
  pthread_mutex_unlock( old_buffer->mutex );

  return ( buffer * ) new_buffer;
}


