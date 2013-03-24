/*
 * queue functions.
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

#ifndef QUEUE_H
#define QUEUE_H 1
#include <malloc.h>

#include "cc_bool.h"
#include "cc_buffer.h"
#include "cc_basic.h"


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
void foreach_message_queue( message_queue *queue, bool function( buffer *message, void *user_data ), void *user_data );
int free_message_element( message_queue_element* element);

#endif // QUEUE_H

