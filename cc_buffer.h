#ifndef BUFFER_H
#define BUFFER_H

#include "cc_basic.h"
#include <stddef.h>


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

buffer *alloc_buffer( void );
buffer *alloc_buffer_with_length( size_t length );
void free_buffer( buffer *buf );
void *append_front_buffer( buffer *buf, size_t length );
void *remove_front_buffer( buffer *buf, size_t length );
void *append_back_buffer( buffer *buf, size_t length );
buffer *duplicate_buffer( const buffer *buf );
void dump_buffer( const buffer *buf, void dump_function( const char *format, ... ) );


#endif // BUFFER_H
