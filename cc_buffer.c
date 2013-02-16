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
front_length_of( buffer *buff ) {
  assert( buff!= NULL );

  return ( size_t ) ( ( char * ) buff->data - ( char * ) buff->top );
}


static bool
already_allocated( buffer *buff, size_t length ) {
  assert( buff != NULL );

  size_t required_length = ( size_t ) front_length_of( buff ) + buff->length + length;

  return ( buff->real_length >= required_length );
}


static buffer *
alloc_new_data( buffer* buff, size_t length ) {
  assert( buff != NULL );

  buff->data = (void*)malloc( length );
  buff->length = length;
  buff->top = buff->data;
  buff->real_length = length;
  buff->tail = buff->data + length - 1;

  pthread_mutexattr_t attr;
  pthread_mutexattr_init( &attr );
  pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE_NP );
  new_buf->mutex = xmalloc( sizeof( pthread_mutex_t ) );
  pthread_mutex_init( buff->mutex, &attr );

  return buff;
}

static buffer *
append_front( buffer* buff, size_t length ) {
  assert( buff != NULL );

  size_t new_length = front_length_of( buff ) + buff->length + length;
  void *new_data = malloc( new_length );
  memcpy( ( char * ) new_data + front_length_of( buff ) + length, buff->data, buff->length );
  free( buff->top );

  buff->data = ( char * ) new_data + front_length_of( buff );
  buff->real_length = new_length;
  buff->top = new_data;
  buff->tail = new_data + new_length - 1;

  return buff;
}


static buffer *
append_back( buffer* buff, size_t length ) {
  assert( buff != NULL );

  size_t new_length = front_length_of( buff ) + buff->length + length;
  void *new_data = malloc( new_length );
  memcpy( ( char * ) new_data + front_length_of( buff ), buff->data, buff->length );
  free( buff->top );

  buff->data = ( char * ) new_data + front_length_of( buff );
  buff->real_length = new_length;
  buff->top = new_data;

  return buff;
}


buffer *
alloc_buffer() {
  return ( buffer * ) alloc_new_data();
}


buffer *
alloc_buffer_with_length( size_t length ) {
  assert( length != 0 );

  buffer *new_buf = xcalloc( 1, sizeof( buffer ) );
  new_buf->data = malloc( length );
  new_buf->length = 0;
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
  assert( buf != NULL );
  
  pthread_mutex_lock( ( ( buffer * ) buf )->mutex );
  buffer *delete_buf = ( buffer * ) buf;
  if ( delete_buf->top != NULL ) {
    xfree( delete_buf->top );
  }
  pthread_mutex_unlock( delete_buf->mutex );
  pthread_mutex_destroy( delete_buf->mutex );
  free( delete_buf->mutex );
  free( delete_buf );
}


void *
append_front_buffer( buffer *buf, size_t length ) {
  assert( buf != NULL );
  assert( length != 0 );

  pthread_mutex_lock( ( ( buffer * ) buf )->mutex );

  buffer* b = ( buffer * ) buf;

  if ( buff->top == NULL ) {
    alloc_new_data( buff, length );
    pthread_mutex_unlock( buff->mutex );
    return buff->data;
  }
  
  if ( already_allocated( b, length ) ) {
    memmove( ( char * ) b->data + length, b->data, b->length );
    memset( b->data, 0, length );
  }
  else {
    append_front( b, length );
  }
  b->length += length;

  pthread_mutex_unlock( buff->mutex );

  return b->data;
}


void *
remove_front_buffer( buffer *buf, size_t length ) {
  assert( buf != NULL );
  assert( length != 0 );

  pthread_mutex_lock( ( ( buffer * ) buf )->mutex );

  buffer* b = ( buffer * ) buf;
  assert( buff->length >= length );

  buff->data = ( char * ) buff->data + length;
  buff->length -= length;

  pthread_mutex_unlock( buff->mutex );

  return buff->data;
}


void *
append_back_buffer( buffer *buf, size_t length ) {
  assert( buf != NULL );
  assert( length != 0 );

  pthread_mutex_lock( ( ( buffer * ) buf )->mutex );

  buffer* b = ( buffer * ) buf;

  if ( buff->real_length == 0 ) {
    alloc_new_data( buff, length );
    pthread_mutex_unlock( buff->mutex );
    return ( char * ) buff->data;
  }

  if ( !already_allocated( buff, length ) ) {
    append_back( buff, length );
  }

  void *appended = ( char * ) buff->data + buff->length;
  buff->length += length;

  pthread_mutex_unlock( buff->mutex );

  return appended;
}


buffer *
duplicate_buffer( const buffer *buf ) {
  assert( buf != NULL );

  pthread_mutex_lock( ( ( const buffer * ) buf )->mutex );

  buffer *new_buffer = alloc_buffer();
  const buffer *old_buffer = ( const buffer * ) buf;

  if ( old_buffer->real_length == 0 ) {
    pthread_mutex_unlock( old_buffer->mutex );
    return ( buffer * ) new_buffer;
  }

  alloc_new_data( new_buffer, old_buffer->real_length );
  memcpy( new_buffer->top, old_buffer->top, old_buffer->real_length );

  new_buffer->public.length = old_buffer->public.length;
  new_buffer->public.user_data = old_buffer->public.user_data;
  new_buffer->public.user_data_free_function = NULL;
  new_buffer->public.data = ( char * ) ( new_buffer->public.data ) + front_length_of( old_buffer );

  pthread_mutex_unlock( old_buffer->mutex );

  return ( buffer * ) new_buffer;
}


void
dump_buffer( const buffer *buf, void dump_function( const char *format, ... ) ) {
  assert( dump_function != NULL );

  pthread_mutex_lock( ( ( const buffer * ) buf )->mutex );

  char *hex = xmalloc( sizeof( char ) * ( buf->length * 2 + 1 ) );
  uint8_t *datap = buf->data;
  char *hexp = hex;
  for ( unsigned int i = 0; i < buf->length; i++, datap++, hexp += 2 ) {
    snprintf( hexp, 3, "%02x", *datap );
  }
  ( *dump_function )( "%s", hex );

  xfree( hex );

  pthread_mutex_unlock( ( ( const buffer * ) buf )->mutex );
}
