#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buffer.h"
#include "checks.h"
#include "utility.h"
#include "wrapper.h"

/*
typedef struct {
  buffer public;
  size_t real_length;
  void *top;
  pthread_mutex_t *mutex;
} private_buffer;
*/

static size_t
front_length_of( buffer *buf )
{
  return ( size_t ) ( ( char * ) buf->data - ( char * ) buf->top );
}


static bool
already_allocated( buffer *buf, size_t length )
{

  size_t required_length = ( size_t ) front_length_of( buf ) + buf->length + length;

  return ( buf->real_length >= required_length );
}


static buffer *
alloc_new_data(buffer* buf,size_t length = dlf_length)
{
  
  buf->data= (void*)malloc( length );
  buf->length = length;
  buf->top = buf->data;
  buf->real_length = length;
  buf->tail = buf->data + length;

  pthread_mutexattr_t attr;
  pthread_mutexattr_init( &attr );
  pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE_NP );
  new_buf->mutex = xmalloc( sizeof( pthread_mutex_t ) );
  pthread_mutex_init( buff->mutex, &attr );

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

  size_t new_length = front_length_of( buf ) + buf->length + length;
  void *new_data = malloc( new_length );
  if(new_data == NULL)
  {
  	return NULL;
  }
  memcpy( ( char * ) new_data + front_length_of( buf ), buf->data, buf->length );
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

	pthread_mutexattr_t attr;
 	pthread_mutexattr_init( &attr );
  	pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE_NP );
  	buf->mutex = malloc( sizeof( pthread_mutex_t ) );
  	pthread_mutex_init( buf->mutex, &attr );

	return buf;
}

buffer *
alloc_buffer() 
{
  return ( buffer * ) alloc_empty_data();
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
  
  pthread_mutexattr_t attr;
  pthread_mutexattr_init( &attr );
  pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE_NP );
  new_buf->mutex = malloc( sizeof( pthread_mutex_t ) );
  pthread_mutex_init( new_buf->mutex, &attr );

  return ( buffer * ) new_buf;
}


void
free_buffer( buffer *buf ) {
  
  pthread_mutex_lock( ( ( buffer * ) buf )->mutex );
  buffer *delete_buf = ( buffer * ) buf;
  if ( delete_buf->top != NULL ) {
    free( delete_buf->top );
  }
  pthread_mutex_unlock( delete_buf->mutex );
  pthread_mutex_destroy( delete_buf->mutex );
  free( delete_buf->mutex );
  free( delete_buf );
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
    return ( buffer * ) new_buffer;
  }

  alloc_new_data( new_buffer, old_buffer->real_length );
  memcpy( new_buffer->top, old_buffer->top, old_buffer->real_length );

  new_buffer->length = old_buffer->length;
  new_buffer->data = ( char * ) ( new_buffer->data ) + front_length_of( old_buffer );
  new_buffer->real_length = old_buffer->real_length;
  new_buffer->tail = old_buffer->tail;

  pthread_mutexattr_t attr;
  pthread_mutexattr_init( &attr );
  pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE_NP );
  new_buffer->mutex = malloc( sizeof( pthread_mutex_t ) );
  pthread_mutex_init( new_buffer->mutex, &attr );
  
  pthread_mutex_unlock( old_buffer->mutex );

  return ( buffer * ) new_buffer;
}


