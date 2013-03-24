/*
 * cc_socket functions.
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

#include "cc_socket.h"


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
cc_set_socket_nonblocking(cc_socket* cc_socket_)
{
    int ret;
    if ((ret = fcntl(cc_socket_->fd, F_GETFL, NULL)) < 0) {
        return CC_ERROR;
    }
    if ((ret = fcntl((cc_socket_->fd, F_SETFL, flags | O_NONBLOCK)) < 0) {
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
		return 	CC_ERROR;
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
    	return CC_ERROR;
  	}

	//ret = setsockopt(cc_socket->fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof( flag ));
	ret = cc_set_socket_nodelay(cc_socket->fd);
	if ( ret < 0 ) 
	{
    	printf("|ERR|socket create failed\n");
    	return CC_ERROR;
  	}

	ret = cc_set_socket_nonblocking(cc_socket->fd);
	if ( ret < 0 ) 
	{
    	printf("|ERR|socket create failed\n");
    	return CC_ERROR;
  	}

	ret = bind(cc_socket->fd,(struct sockaddr*)&cc_socket->cc_addr,sizeof(struct sockaddr_in));
	if(ret < 0)
	{
    	printf("|ERR|socket create failed\n");
		close( cc_switch_proc.listen_sock.fd );
    	return CC_ERROR;
  	}

	if(listen(cc_socket->fd,CC_LENGTH_OF_LISTEN_QUEUE))
	{
    	printf("|ERR|socket create failed\n");
		close( cc_switch_proc.listen_sock.fd );
    	return CC_ERROR;
  	}

	return cc_socket->fd;
}


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

	
    cc_set_socket_nonblocking(fd);
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));    

    memset(sin.sin_zero, 0, sizeof sin.sin_zero);
    if (connect(fd, (struct sockaddr *)&sin, sizeof(struct sockaddr)) == -1) {
        perror("connect");
        return -1;
    }

    return fd;
}


int 
cc_close_socket(struct cc_socket *cc_socket)
{
	log_info_for_cc("close listen fd!");
	close(cc_socket->listen_fd);
	free(cc_socket);
	return CC_SUCCESS;
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
cc_init_listenfd(cc_socket* cc_socket)
{
	int ret;

	cc_socket = (cc_socket*)malloc(sizeof(cc_socket));
	
	ret = cc_conn_init(cc_socket);
	if( ret < 0 )
		return CC_ERROR;
	else
		return CC_SUCCESS;
}


static int
cc_init_sw_info(sw_info* cc_sw_info)
{

	cc_sw_info = (sw_info*)malloc(sizeof(cc_sw_info));
	cc_sw_info->xid = 0;
	cc_sw_info->recv_queue = create_message_queue();
	cc_sw_info->send_queue = create_message_queue();
	pool_init(cc_sw_info->cc_recv_thread_pool, CC_MAX_THREAD_NUM);
	cc_sw_info->state = CC_CONNECT;
	cc_init_xid_table(cc_sw_info);
	cc_sw_info->xid_latest = 0;
	cc_sw_info->config_flags = CC_MAX_THREAD_NUM;
	cc_sw_info->miss_send_len = OFP_DEFAULT_MISS_SEND_LEN;
	
	return CC_SUCCESS;
}


static int
cc_finalize_sw_info(cc_sw_info)
{
	delete_message_queue(cc_sw_info->recv_queue);
	delete_message_queue(cc_sw_info->send_queue);
	pool_destroy(cc_sw_info->cc_recv_thread_pool);

	return CC_SUCCESS;
}


static int 
cc_conn_accept(cc_socket* cc_socket_ ,sw_info* cc_sw_info)
{
	struct sockaddr_in switch_addr;
	socklen_t addr_len;
	int pid;
	int accept_fd;
	int ret;

	addr_len = sizeof(struct sockaddr_in);
	accept_fd = accept(cc_socket_->fd,(struct sockaddr*)&switch_addr,sizeof(switch_addr));
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
	
	pid = fork();
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
		int child_tmp = tmp;
		sw_info* cc_sw_info;
		cc_init_sw_info(cc_sw_info);

		cc_sw_info->cc_switch->pid = getpid();
		/*here we can add a function to build
		 *a file to restore the cc_sw_info with 
		 *a special name, such as "sw_$pid.txt".
		 *then main loop can throught search these files
		 *to make a list which can be used to build a virtual network manager
	     */
		cc_sw_info->cc_switch->cc_socket->fd = accept_fd;
		cc_sw_info->cc_switch->cc_socket->cc_addr = switch_addr;
		close( cc_switch_table->listen_socket.fd );

		if( accept_fd < CC_ACCEPT_FD)
		{
			dup2(accept_fd,CC_ACCEPT_FD);//avoid the fd is smaller than 3,0 is for standard input, 
										//1 is for standard output 2 is for standard error
			close(accept_fd);
			accept_fd = CC_ACCEPT_FD;
		}

		struct timeval timeout;
		fd_set writefds;
		fd_set readfds;
		pool_init(CC_MAX_THREAD_NUM);
		while(1)
		{
			FD_ZERO(&readfds);
			FD_ZERO(&writefds);
			FD_SET(accept_fd,&readfds);
			FD_SET(accept_fd,&writefds);
			timeout.tv_sec = CC_CONN_TIMEOUT_SEC;
			timeout.tv_usec = CC_CONN_TIMEOUT_USEC;
			ret = select(fd,readfds,writefds,NULL,timeout);
			if( ret == -1 )
			{
				if( errno == EINTR )				
					continue;
				else
					return CC_ERROR;
			}else if( ret == 0 ){
				continue;
			}else{
				if(FD_ISSET(accept_fd,&readfds))
					cc_of_handler_recv_event(cc_sw_info);
					//cc_recv_from_secure_channel(cc_sw_info);
				if(FD_ISSET(accept_fd,&writefds))
					cc_of_handler_send_event(cc_sw_info);
					//cc_flush_to_secure_channel(cc_sw_info);
			}
		}
		cc_finalize_sw_info(cc_sw_info);
		return CC_SUCCESS;
		/*may be we should throw a signal to parent to delete the
		*the record of this switch 
		*/
	}else{
		/* this is parent*/
		//cc_sw_info->cc_switch->pid = pid;
		close(accept_fd);
		return CC_SUCCESS;
	}

}

	
