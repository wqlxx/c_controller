#include <unistd.h>   
#include <sys/types.h>   
#include <pthread.h>   
#include <assert.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>  
#include <sys/stat.h>
#include "cc_basic.h"

#define BUFFER_SIZE 1024  

void pool_init (CThread_pool* pool,int max_thread_num);
int pool_add_worker (CThread_pool* pool,void *(*process) (void *arg), void *arg);   
void *thread_routine (CThread_pool* pool,void *arg); 
int pool_destroy (CThread_pool* pool);

