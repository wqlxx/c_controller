#ifndef CC_GET_LOCAL_IP_H
#define cc_get_local_ip.h 1

#include <arpa/inet.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
 
#define MAXINTERFACES 16    /* 最大接口数 */
#define GET_LOCAL_IP_ERR -1

int get_local_ip_fd;         /* 套接字 */
int get_local_ip_if_len;     /* 接口数量 */
struct ifreq get_local_ip_buf[MAXINTERFACES];    /* ifreq结构数组 */
struct ifconf get_local_ip_ifc;                  /* ifconf结构 */

struct in_addr get_local_ip_main();

#endif
