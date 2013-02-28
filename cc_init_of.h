#include "cc_socket.h"
#include "cc_of_handler.h"
#include "cc_switch_table.h"
#include "cc_basic_struct.h"

typedef void (*event_handler_callback)(int ,void* data);

extern static int cc_init_event_handler(int fd,event_handler_callback read_callback,void * read_data,
						event_handler_callback write_callbacl,void * write_data)
extern static int cc_init_of();
extern static int cc_init_app();
