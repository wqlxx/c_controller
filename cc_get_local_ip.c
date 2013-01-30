#include "cc_get_local_ip.h"

in_addr_t get_local_ip_main()
{
	in_addr_t cc_s_addr;
    /* 建立IPv4的UDP套接字fd */
    if ((get_local_ip_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("socket(AF_INET, SOCK_DGRAM, 0)");
        return -1;
    }
 
    /* 初始化ifconf结构 */
    get_local_ip_ifc.ifc_len = sizeof(get_local_ip_buf);
    get_local_ip_ifc.ifc_buf = (caddr_t) get_local_ip_buf;
 
    /* 获得接口列表 */
    if (ioctl(get_local_ip_fd, SIOCGIFCONF, (char *) &get_local_ip_ifc) == -1)
    {
        perror("SIOCGIFCONF ioctl");
        return -1;
    }
 
    get_local_ip_if_len = get_local_ip_ifc.ifc_len / sizeof(struct ifreq); /* 接口数量 */
    printf("接口数量:%d/n/n", get_local_ip_if_len);
 
    while ((get_local_ip_if_len--) > 0) /* 遍历每个接口 */
    {
        printf("接口：%s/n", buf[get_local_ip_if_len].ifr_name); /* 接口名称 */
 
        /* 获得接口标志 */
        if (!(ioctl(get_local_ip_fd, SIOCGIFFLAGS, (char *) &get_local_ip_buf[get_local_ip_if_len])))
        {
            /* 接口状态 */
            if (buf[get_local_ip_if_len].ifr_flags & IFF_UP)
            {
                printf("接口状态: UP/n");
            }
            else
            {
                printf("接口状态: DOWN/n");		
            }
        }
        else
       		return GET_LOCAL_IP_ERR;
 
        /* 子网掩码 */
        if (!(ioctl(get_local_ip_fd, SIOCGIFNETMASK, (char *) &get_local_ip_buf[get_local_ip_if_len])))
        {
            printf("子网掩码:%s/n",
                    (char*)inet_ntoa(((struct sockaddr_in*) (&get_local_ip_buf[get_local_ip_if_len].ifr_addr))->sin_addr));
        }
        else
       		return GET_LOCAL_IP_ERR;

		
        /* IP地址 */
        if (!(ioctl(get_local_ip_fd, SIOCGIFADDR, (char *) &get_local_ip_buf[get_local_ip_if_len])))
        {
            printf("IP地址:%s/n",
                    (char*)inet_ntoa(((struct sockaddr_in*) (&get_local_ip_buf[get_local_ip_if_len].ifr_addr))->sin_addr));
			cc_s_addr = (struct sockaddr_in*) (&get_local_ip_buf[get_local_ip_if_len].ifr_addr))->sin_addr.s_addr;
        }
        else
       		return GET_LOCAL_IP_ERR;
		
    }//–while end
 
    //关闭socket
    close(get_local_ip_fd);
	return cc_s_addr
}

