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

#include "queue.h"

message_queue *
create_message_queue( void ) {
	message_queue *new_queue = (message_queue *)malloc( sizeof( message_queue ) );
    new_queue->head =(message_queue_element*)malloc( sizeof( message_queue_element ) );
    new_queue->head->data = NULL;
    new_queue->head->next = NULL;
    new_queue->divider = new_queue->tail = new_queue->head;
    new_queue->length = 0;
	pthread_mutex_init(new_queue->queue_lock, NULL);
	
  	return new_queue;
}


/*
 * don't use it in the dequeue_message
 * we should use it after send msg
 */
int
free_message_element(message_queue_element * element)
{
	free(element);
	return CC_SUCCESS;
}


int
delete_message_queue( message_queue *queue ) {

	message_queue_element* element;
  	pthread_mutex_lock(&(queue->queue_lock));
  	if ( queue == NULL ) {
    	perror( "queue must not be NULL" );
		pthread_mutex_unlock(&(queue->queue_lock));
		return CC_SUCCESS;
  	}

  	while ( queue->head != NULL ) {
    	element = queue->head;
    	if ( queue->head->data != NULL ) {
      		free_buffer( element->data );
    	}
		free_message_element(element);
    	queue->head = queue->head->next;
    	free( element );
 	}
  	pthread_mutex_unlock(&(queue->queue_lock));
	pthread_mutex_destory(&(queue->queue_lock));
  	free( queue );

  	return CC_SUCCESS;
}


static void
collect_garbage( message_queue *queue ) 
{
	pthread_mutex_lock(&(queue->queue_lock));
  	while ( queue->head != queue->divider ) {
    	message_queue_element *element = (message_queue_element*)malloc(sizeof(message_queue_element));
		element = queue->head;
    	queue->head = queue->head->next;
    	free_message_element( element );
  	}
	pthread_mutex_unlock(&(queue->queue_lock));
}


int
enqueue_message( message_queue *queue, buffer *buf ) 
{
  pthread_mutex_lock(&(queue->queue_lock));
  if ( queue == NULL ) {
    log_err_for_cc( "queues must not be NULL" );
	pthread_mutex_unlock(&(queue->queue_lock));
	return CC_ERROR;
  }
  if ( buf == NULL ) {
    log_err_for_cc( "message must not be NULL" );
	pthread_mutex_unlock(&(queue->queue_lock));
	return CC_ERROR;
  }

  message_queue_element *new_tail = (message_queue_element*)malloc( sizeof( message_queue_element ) );
  new_tail->data = buf;
  new_tail->next = NULL;

  queue->tail->next = new_tail;
  queue->tail = new_tail;
  queue->length++;

  //collect_garbage( queue );
  pthread_mutex_unlock(&(queue->queue_lock));
  
  return CC_SUCCESS;
}


buffer*
dequeue_message( message_queue *queue) {

  	buffer* buf;
 	message_queue_element* tmp_element;

  	pthread_mutex_lock(&(queue->queue_lock));
  	if ( queue == NULL ) {
    	perror( "queue must not be NULL" );	
		pthread_mutex_unlock(&(queue->queue_lock));
		return NULL;
  	}
  	if ( queue->head == queue->tail ) {
  		pthread_mutex_unlock(&(queue->queue_lock));
  		return NULL
  	}
	if( queue->length == 0 ){
  		pthread_mutex_unlock(&(queue->queue_lock));
		return NULL;
	}
#if 0
  message_queue_element *next = queue->head->next;
  //buffer *message = next->data;
  buf = next->data;
  next->data = NULL; // data must be freed by caller
  queue->divider = next;
  queue->length--;
#endif
	tmp_element = queue->head;
	tmp_element->next = NULL;
	buf = queue->head->data;
	
	queue->head = queue->head->next;
	queue->length--;
	free_message_element(tmp_element);
  	pthread_mutex_unlock(&(queue->queue_lock));
  
  //return message;
  return buf;
}

#if 0
int
peek_message( message_queue *queue, buffer* buf) {

  	pthread_rwlock_rdlock(&(queue->queue_lock));
  	if ( queue == NULL ) {
    	perror( "queue must not be NULL" );	
 		pthread_rwlock_unlock(&(queue->queue_lock));  	
		return CC_ERROR;
  	}

  	if ( queue->head == queue->tail ) {  	
 		pthread_rwlock_unlock(&(queue->queue_lock));  	
    	return CC_ERROR;
  	}

  	buf = queue->divider->next->data;

	pthread_rwlock_unlock(&(queue->queue_lock));
	return CC_SUCCESS;
  //return queue->divider->next->data;
}
#endif

void 
foreach_message_queue( message_queue *queue, bool function( buffer *message, void *user_data ), void *user_data ) {

  pthread_mutex_lock(&(queue->queue_lock));
  if ( queue->divider == queue->tail ) {
 	pthread_mutex_unlock(&(queue->queue_lock));  	
    return;
  }
  message_queue_element *element;
  for ( element = queue->divider->next; element != NULL; element = element->next ) {
    buffer *message = element->data;
    assert( message != NULL );
    if ( !function( message, user_data ) ) {
      break;
    }
  }
  pthread_mutex_unlock(&(queue->queue_lock));
  return;
}


