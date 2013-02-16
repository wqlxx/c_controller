#ifndef CC_SOCKET_H
#define CC_SOKERT_H 1

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "cc_get_local_ip.h"
#include "cc_basic.h"

#define CC_CONN_ERR -1
#define CC_CONN_SUCCESS 0
#define CC_LISTEN_PORT 6633
#define CC_LENGTH_OF_LISTEN_QUEUE 20
//#define CC_TRY_CONN_TIMES 5
#define CC_ACCEPT_FD 3
#define CC_CHILD 1
#define CC_PARENT 0

extern int
cc_conn_init(struct cc_socket* cc_socket);

extern int 
cc_conn_accept(struct cc_socket* cc_socket);

extern int 
cc_close_socket(struct cc_socket* cc_socket);

extern int
cc_set_socket_nonblocking(struct cc_socket * cc_socket)(struct cc_socket* cc_socket);

extern int 
cc_set_recvbuf(struct cc_socket* cc_socket,size_t size);

extern int 
cc_set_socket_fd(void);

extern int 
cc_server_conn_create(struct cc_socket* cc_socket);

#endif