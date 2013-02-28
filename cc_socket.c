#include "cc_basic.h"

extern struct cc_switch_proc cc_s_p[CC_MAX_NUM_SWITCH];

int 
cc_set_socket_fd(void)
{
	int fd;
	int connect_status;
	fd = socket(AF_INET,SOCK_STREAM,0);
	return fd;
}

int
cc_set_socket_nonblocking(int fd)
{
    int ret;
    if ((ret = fcntl(fd, F_GETFL, NULL)) < 0) {
        return CC_ERROR;
    }
    if ((ret = fcntl((fd, F_SETFL, flags | O_NONBLOCK)) < 0) {
        return CC_ERROR;
    }
    return CC_SUCCESS;
}

int
cc_set_socket_nodelay(int fd)
{
	int flag = 1;
  	int ret = setsockopt( fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof( flag ) );
  	if ( ret < 0 ) {
    	error( "Failed to set socket options ( fd = %d, ret = %d, errno = %s [%d] ).",
           fd, ret, strerror( errno ), errno );
    	return CC_ERROR;
 	}
	return CC_SUCCESS
}

int 
cc_set_recvbuf(int fd,size_t size)
{
	return setsockopt(fd,SOL_SOCKET,SO_RCVBUF,&size,sizeof(size));
}

int 
cc_server_conn_create(struct cc_socket *cc_socket)
{
	
	if((cc_socket->fd= cc_set_socket_fd()) < 0)
	{
		printf("|ERR|socket create failed\n");
		return 	CC_CONN_ERR;
	}

	memset(cc_socket->cc_addr,0,sizeof(struct sockaddr_in));
	cc_socket->cc_addr.sin_family = AF_INET;
	cc_socket->cc_addr.sin_addr.s_addr = get_local_ip_main();
	cc_socket->cc_addr.sin_port = CC_LISTEN_PORT;
	
	int flag = 1;
	int ret;
	
	ret = setsockopt(cc_socket->fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof( flag ))
	if ( ret < 0 ) 
	{
    	printf("|ERR|socket create failed\n");
    	return CC_CONN_ERR;
  	}

	ret = setsockopt(cc_socket->fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof( flag ));
	if ( ret < 0 ) 
	{
    	printf("|ERR|socket create failed\n");
    	return CC_CONN_ERR;
  	}

	ret = bind(cc_socket->fd,(struct sockaddr*)&cc_socket->cc_addr,sizeof(struct sockaddr_in));
	if(ret < 0)
	{
    	printf("|ERR|socket create failed\n");
		close( cc_switch_proc.listen_sock.fd );
    	return CC_CONN_ERR;
  	}

	if(listen(cc_socket->fd,CC_LENGTH_OF_LISTEN_QUEUE))
	{
    	printf("|ERR|socket create failed\n");
		close( cc_switch_proc.listen_sock.fd );
    	return CC_CONN_ERR;
  	}

	return cc_socket->fd;
}

/*
int
cc_client_socket_create(char *server_ip, uint16_t port)
{
    struct sockaddr_in sin;
    int                fd;
    int                one = 1;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return fd;
    }

    sin.sin_family = AF_INET; 
    sin.sin_port = htons(port);
    if (!inet_aton(server_ip, &sin.sin_addr)) {
        return -1;
    }

    memset(sin.sin_zero, 0, sizeof sin.sin_zero);
    if (connect(fd, (struct sockaddr *)&sin, sizeof(struct sockaddr)) == -1) {
        perror("connect");
        return -1;
    }

    cc_make_socket_nonblocking(fd);
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));    

    return fd;
}
*/

int 
cc_close_socket(struct cc_socket *cc_socket)
{
	close(cc_socket->listen_fd);
	free(cc_socket);
}


/*hello,echo_request,need get the reply in a given time*/
/*
static void
switch_set_timeout( long sec, timer_callback callback, void *user_data ) {
  struct itimerspec interval;

  interval.it_value.tv_sec = sec;
  interval.it_value.tv_nsec = 0;
  interval.it_interval.tv_sec = 0;
  interval.it_interval.tv_nsec = 0;
  add_timer_event_callback( &interval, callback, user_data );
  switch_info.running_timer = true;
}
*/
/*int
cc_socket_read_nonblock_loop()
*/

static int
cc_conn_init(struct cc_socket* cc_socket)
{
	int ret;
	if((ret = (cc_server_conn_create(cc_socket)))<0)
		return CC_ERROR;
	else	
		return CC_SUCCESS;
	
}

static int
cc_init_listenfd(struct cc_socket* cc_socket)
{
	int ret;
	ret = cc_conn_init(cc_socket);
	if( ret == CC_ERROR )
		return CC_ERROR;
	else
		return CC_SUCCESS;
}

static int 
cc_conn_accept(switch_table* cc_switch_table,sw_info* cc_sw_info)
{
	struct sockaddr_in switch_addr;
	socklen_t addr_len;
	int pid;
	int accept_fd;

	addr_len = sizeof(struct sockaddr_in);
	accept_fd = accept(cc_switch_table->listen_socket->fd,(struct sockaddr*)&switch_addr,sizeof(switch_addr));
	if(accept_fd < 0)
	{
		printf("|ERR|accept failed\n");
		close(accept_fd);
		return CC_ERROR;
	}else if( accept_fd > FD_SETSIZE ) {
		close(accept_fd);
	} else {
		cc_set_socket_nonblocking(accept_fd);
		cc_set_recvbuf(accept_fd,CC_MAX_SOCKET_BUFF);
		cc_set_socket_nodelay(accept_fd);
	}

	/*create a new 'sw_info' after a success 'fork',after the switch leave ,we nend to free the space*/
	/*
	sw_info *new_sw_info = (sw_info *)malloc(sizeof(sw_info));
	new_sw_info->cc_switch.cc_socket.cc_addr.sin_family = switch_addr.sin_family;
	new_sw_info->cc_switch.cc_socket.cc_addr.sin_port = switch_addr.sin_port;
	new_sw_info->cc_switch.cc_socket.cc_addr.sin_addr.s_addr = switch_addr.sin_addr.s_addr;
	*/
	cc_sw_info->cc_switch->cc_socket->fd = accept_fd;
	cc_sw_info->cc_switch->cc_socket->cc_addr = switch_addr;
	
	pid = fork();
	//cc_sw_info->cc_switch->pid = pid;
	//cc_sw_queue->head->next = new_sw_info;
	
	if(pid < 0)
	{
		//TODO: close the listen socket
		//printf("|ERR|fork failed\n");
		perror("create child process failed!");
		log_err_for_cc("create child process failed!");
		return CC_ERROR;
	}

	if(pid == 0)
	{
		//int child_pid = getpid();
		int child_tmp = tmp;
		//cc_switch_proc.cc_sock[child_tmp].pid = child_pid;
		close( cc_switch_table->listen_socket.fd );

		//sw_info *cc_sw_info = (sw_info *)malloc(sizeof(sw_info));
		//cc_sw_info->cc_switch.pid = getpid();
		//cc_sw_info->cc_switch
		if( accept_fd < CC_ACCEPT_FD)
		{
			dup2(accept_fd,CC_ACCEPT_FD);//avoid the fd is smaller than 3,0 is for standard input, 
										//1 is for standard output 2 is for standard error
			close(accept_fd);
			accept_fd = CC_ACCEPT_FD;
		}

		
		/*cc_send_hello(cc_socket);//after accept directly send heelo msg to switch?
		 *cc_send_feature_request
		 *cc_send_echo_request
		 */

		
		/*FUNC:pool_int():
		 * after fork,we create the threads in the child process,to handle thr 'read' and 'write'
		 */
		pool_init(CC_MAX_THREAD_NUM);
		while(1)
		{
			cc_secure_channel_read(cc_sw_info);
			cc_secure_channel_write(cc_sw_info);
		}
		return CC_SUCCESS;
	}else{
		/* this is parent*/
		cc_sw_info->cc_switch->pid = pid;
		close(accept_fd);
		return CC_SUCCESS;
	}

}

	
