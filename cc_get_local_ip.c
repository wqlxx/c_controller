#include "cc_get_local_ip.h"

in_addr_t get_local_ip_main()
{
	in_addr_t cc_s_addr;
    /* ����IPv4��UDP�׽���fd */
    if ((get_local_ip_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("socket(AF_INET, SOCK_DGRAM, 0)");
        return -1;
    }
 
    /* ��ʼ��ifconf�ṹ */
    get_local_ip_ifc.ifc_len = sizeof(get_local_ip_buf);
    get_local_ip_ifc.ifc_buf = (caddr_t) get_local_ip_buf;
 
    /* ��ýӿ��б� */
    if (ioctl(get_local_ip_fd, SIOCGIFCONF, (char *) &get_local_ip_ifc) == -1)
    {
        perror("SIOCGIFCONF ioctl");
        return -1;
    }
 
    get_local_ip_if_len = get_local_ip_ifc.ifc_len / sizeof(struct ifreq); /* �ӿ����� */
    printf("�ӿ�����:%d/n/n", get_local_ip_if_len);
 
    while ((get_local_ip_if_len--) > 0) /* ����ÿ���ӿ� */
    {
        printf("�ӿڣ�%s/n", buf[get_local_ip_if_len].ifr_name); /* �ӿ����� */
 
        /* ��ýӿڱ�־ */
        if (!(ioctl(get_local_ip_fd, SIOCGIFFLAGS, (char *) &get_local_ip_buf[get_local_ip_if_len])))
        {
            /* �ӿ�״̬ */
            if (buf[get_local_ip_if_len].ifr_flags & IFF_UP)
            {
                printf("�ӿ�״̬: UP/n");
            }
            else
            {
                printf("�ӿ�״̬: DOWN/n");		
            }
        }
        else
       		return GET_LOCAL_IP_ERR;
 
        /* �������� */
        if (!(ioctl(get_local_ip_fd, SIOCGIFNETMASK, (char *) &get_local_ip_buf[get_local_ip_if_len])))
        {
            printf("��������:%s/n",
                    (char*)inet_ntoa(((struct sockaddr_in*) (&get_local_ip_buf[get_local_ip_if_len].ifr_addr))->sin_addr));
        }
        else
       		return GET_LOCAL_IP_ERR;

		
        /* IP��ַ */
        if (!(ioctl(get_local_ip_fd, SIOCGIFADDR, (char *) &get_local_ip_buf[get_local_ip_if_len])))
        {
            printf("IP��ַ:%s/n",
                    (char*)inet_ntoa(((struct sockaddr_in*) (&get_local_ip_buf[get_local_ip_if_len].ifr_addr))->sin_addr));
			cc_s_addr = (struct sockaddr_in*) (&get_local_ip_buf[get_local_ip_if_len].ifr_addr))->sin_addr.s_addr;
        }
        else
       		return GET_LOCAL_IP_ERR;
		
    }//�Cwhile end
 
    //�ر�socket
    close(get_local_ip_fd);
	return cc_s_addr
}

