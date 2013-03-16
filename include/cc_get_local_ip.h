/*
 * cc_of_msg_action functions.
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
