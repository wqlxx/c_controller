/*
 * cc_thread_pool functions.
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

#include "cc_thread_pool.h"  
    
void
pool_init (CThread_pool* pool,int max_thread_num)//初始化线程池   
{   
     pool = (CThread_pool *) malloc (sizeof (CThread_pool));   
  
     pthread_mutex_init (&(pool->queue_lock), NULL);   
     pthread_cond_init (&(pool->queue_ready), NULL);//？   
  
     pool->queue_head = NULL;   
  
     pool->max_thread_num = max_thread_num;   
     pool->cur_queue_size = 0;   
  
     pool->shutdown = 0;   
  
     pool->threadid =(pthread_t *) malloc (max_thread_num * sizeof (pthread_t)); 
	 pool->attr = (pthread_attr_t*)malloc (sizeof(pthread_attr_t));
     int i = 0;
	 /*set the pthread attr to detach, not joinable
	  *to make sure main pthread not wait for child 
	  *and child will handle memory it alloc for itself.
	  */
	 //pthread_attr_t attr;
	 pthread_attr_init(pool->attr);
	 pthread_attr_setdetachstate(pool->attr, PTHREAD_CREATE_DETACHED);
     for (i = 0; i < max_thread_num; i++)   
     {   
     	/* attr is to be set
         pthread_create (&(pool->threadid[i]), NULL, thread_routine,pool);   
		*/
		 pthread_create (&(pool->threadid[i]), pool->attr, thread_routine,pool);	
     }   
}   
  
int
pool_add_worker (CThread_pool* pool,void *(*process) (void *,void* ), void *arg))  
{   
     CThread_worker *newworker =(CThread_worker *) malloc (sizeof (CThread_worker));   
     newworker->process = process;   
     newworker->arg = arg;   
     newworker->next = NULL;/*别忘置空*/ 
	 //pthread_mutex_init(&(newworker->worker_lock),NULL);	 
     pthread_mutex_lock (&(pool->queue_lock)); //init the lock for the worker
     CThread_worker *member = pool->queue_head;   
	 if (member != NULL)   
     {   
        while (member->next != NULL)   
             member = member->next;   
         member->next = newworker;   
     }
	 else  
     {   
         pool->queue_head = newworker;   
     }   
  
  
     assert (pool->queue_head != NULL);   
  
  
     pool->cur_queue_size++;   
     pthread_mutex_unlock (&(pool->queue_lock));   
     pthread_cond_signal (&(pool->queue_ready));   
	 return CC_SUCCESS;;   
}   
  
int
pool_destroy (CThread_pool* pool,)   
{   
    if (pool->shutdown)   
        return -1;/*防止两次调用*/  
     pool->shutdown = 1;   
  
  
     pthread_cond_broadcast (&(pool->queue_ready));   
  
  
    int i;   
    for (i = 0; i < pool->max_thread_num; i++)   
         pthread_join (pool->threadid[i], NULL);   
     free (pool->threadid);   
  
  
     CThread_worker *head = NULL;   
     while (pool->queue_head != NULL)   
     {   
         head = pool->queue_head;   
         pool->queue_head = pool->queue_head->next;   
         free (head);   
     }   
     pthread_mutex_destroy(&(pool->queue_lock));   
     pthread_cond_destroy(&(pool->queue_ready));   
        
     free (pool);   
     pool=NULL;   
    return 0;   
}   

void*
thread_routine (void *arg)   
{   
	pthread_detach(pthread_self());//detach is prepare to let main thread go on 
									//and not wait for this pthtrad.
	CThread_pool* pool = (CThread_pool*)arg;
	printf ("starting thread 0x%x\n", pthread_self ());   
    while(1)   
    {   
        pthread_mutex_lock (&(pool->queue_lock));   
        while (pool->cur_queue_size == 0 && !pool->shutdown)   
        {   
             printf ("thread 0x%x is waiting\n", pthread_self ());   
             pthread_cond_wait(&(pool->queue_ready), &(pool->queue_lock));   
        }   
  
        if (pool->shutdown)   
		{   
             pthread_mutex_unlock (&(pool->queue_lock));   
             printf ("thread 0x%x will exit\n", pthread_self ());   
             pthread_exit (NULL);   
        }     
  
        printf ("thread 0x%x is starting to work\n", pthread_self ());   
  
        assert (pool->cur_queue_size != 0);   
        assert (pool->queue_head != NULL);   
            
        pool->cur_queue_size--;   
        CThread_worker *worker = pool->queue_head;   
        pool->queue_head = worker->next;   
        pthread_mutex_unlock (&(pool->queue_lock));   
  
  
		//pthread_mutex_lock(&(woker->work_lock));
		/*add the trans of work->arg to 
		 *    typedef struct{
		 *			sw_info* cc_sw_info;
		 *			buffer* buf;
		 *	  }worker_buf;
		 */
		worker_buf* tmp_buf 
		tmp_buf = (worker_buf*)(worker->arg);
        (*(worker->process)) (worker->arg);  
		//pthread_mutex_unlock(&(woker->work_lock));
        free (worker);   
        worker = NULL;   
     }   
     pthread_exit (NULL);   
}   

/*test code*/
/*
void *myprocess (void *arg)   
{   
     printf ("threadid is 0x%x, the msg in queue is %s\n", pthread_self (),arg);   
     sleep (1);
     return NULL;   
}   

int main (int argc, char **argv)   
{   
	pool_init(3);
	
	int listenfd,connfd;
	int len;
	struct sockaddr_in servaddr,cliaddr;
	char buff[BUFFER_SIZE];
	
	listenfd = socket(AF_INET,SOCK_STREAM,0);

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(6633);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int ret;
	ret = bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
	if(ret < 0)
	{
		printf("bind error!\n");
		return -1;
	}
	
	ret = listen(listenfd,10);
	int str_len;
	for(;;)
	{
		len = sizeof(cliaddr);
		connfd = accept(listenfd,(struct sockaddr*)&cliaddr,&len);
		if(connfd < 0)
		{
			printf("accept error!\n");
			return -1;
		}
		
		while((str_len = read(connfd,buff,1024)) > 0)
		{
			printf("recv:%s\n",buff);
			switch(buff){
				case 
			pool_add_worker(myprocess,buff);
			printf("back\n");
		}
	}
 	sleep(5);
	pool_destory();
	close(listenfd);
	return 0;  
}  
/*


