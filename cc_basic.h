#ifndef CC_BASIC_H
#define CC_BASIC_H 1

#include "cc_init_of.h"
#include "cc_socket.h"
#include "cc_log.h"


#define CC_ERROR -1
#define CC_SUCCESS 0

/*used to temperaly to restrict the number \
*of linked switch,to make this project go easy
*/
#define CC_MAX_NUM_SWITCH 3

/*used in cc_thread_pool.c to \
*restrict the number of pthread
*/
#define CC_MAX_THREAD_NUM 3 

#define FUNC_NAME __FUNCTION__

#define CC_TIMEOUT_FOR_HELLO 5
#define CC_TIMEOUT_FOR_ECHO_REPLY 5
#define CC_RECV_BUFFER_SIZE (UINT16_MAX+sizeof( struct ofp_packet_in )-2) 
#define CC_MAX_SOCKET_BUFF 3*1024*1024
#define CC_MAX_PORT 52

enum sw_state{
	CC_CONNECT,
	CC_DISCONNECT,
	CC_WAIT_HELLO,
	CC_WAIT_ECHO_REPLY,
};
typedef enum sw_state sw_state;

struct cc_socket{
	struct sockaddr_in cc_addr;
	//uint16_t port;
	int fd;
};
typedef struct cc_socket cc_socket;

struct port_info{
	struct ofp_phy_port port;
	int valid;
};
typedef struct port_info port_info;

struct each_sw{
	pid_t pid;
	uint64_t dpid;//datapath_id come from switch_feature_request/reply
	cc_socket cc_socket;
	//each_sw *next;
	char* profile_path;
	uint8_t version;
	uint32_t n_buffers;
	uint8_t n_tables;
	uint32_t actions;
	uint32_t capabilities;
	//port_info* port_head;
	port_info sw_port[CC_MAX_PORT];
};
typedef struct each_sw each_sw;

struct switch_table {
	each_sw *head;
	struct cc_socket listen_socket;
};
typedef struct switch_table switch_table;
/**************************************************/

struct ofmsg_buf
{
    void* data;
    size_t length;
    void *user_data;
};
typedef struct ofmsg_buf ofmsg_buf;

typedef struct worker   
{   
    void *(*process) (void *arg);   
    void *arg;/*回调函数的参数,recv_ofmsg*/  
    struct worker *next;   
} CThread_worker;   
  
struct CThread_pool
{   
    pthread_mutex_t queue_lock;   
	pthread_cond_t queue_ready;   
    
    CThread_worker *queue_head;   
  
    int shutdown;   
    pthread_t *threadid;   
    int max_thread_num;   
    int cur_queue_size;   
}; 
typedef struct CThread_pool CThread_pool;

struct event_handler{
	event_handler_callback read_handler;
	event_handler_callback write_handler;
	void* read_buf;
	void* write_buf;
};
typedef struct event_handler event_handler;

struct sw_info
{
	each_sw cc_switch;
	pthreat_t cc_sw_thread[CC_MAX_THREAD_NUM];

	//uint64_t datapathid;
	CThread_pool cc_recv_thread_pool;
	CThread_pool cc_send_thread_pool;	

	sw_state state;

	message_queue *send_queue;
  	message_queue *recv_queue;

	event_handler eh;
	uint32_t xid;
	struct sw_info* next_sw;
}
typedef struct sw_info sw_info;



//struct cc_socket listen_socket;

//typedef struct sw_queue cc_sw_queue;



#endif

