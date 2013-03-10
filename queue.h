#ifndef QUEUE_H
#define QUEUE_H 1
#include <malloc.h>

#include "bool.h"
#include "cc_buffer.h"


struct message_queue_element {
  buffer *data;
  struct message_queue_element *next;
};
typedef struct message_queue_element message_queue_element;


typedef struct message_queue{
  message_queue_element *head;
  message_queue_element *divider;
  message_queue_element *tail;
  unsigned int length;
  pthread_mutex_t queue_lock;
};
typedef struct message_queue message_queue; 


message_queue *create_message_queue( void );
int delete_message_queue( message_queue *queue );
int enqueue_message( message_queue *queue, buffer* buf );
int dequeue_message( message_queue *queue,buffer* buf );
int peek_message( message_queue *queue, buffer* buf );
void foreach_message_queue( message_queue *queue, bool function( buffer *message, void *user_data ), void *user_data );
int free_message_element( message_queue_element* element);

#endif // QUEUE_H

