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

#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <string.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/time.h>
typedef int SOCKET;
#define MAXBUFSIZE 1024
#define NLISTEN 1024
#define isvalidsock(s)	( ( s ) > 0 )
void set_address( char *hname, char *sname,struct sockaddr_in *sap, char *protocol )
{
	struct servent *sp;
	struct hostent *hp;
	char *endptr;
	short port;

	bzero( sap, sizeof( *sap ) );
	sap->sin_family = AF_INET;
	if ( hname != NULL )
	{
		if ( !inet_aton( hname, &sap->sin_addr ) )
		{
			hp = gethostbyname( hname );
			if ( hp == NULL )
				error( 1, 0, "unknown host: %s/n", hname );
			sap->sin_addr = *( struct in_addr * )hp->h_addr;
		}
	}
	else
		sap->sin_addr.s_addr = htonl( INADDR_ANY );
	port = strtol( sname, &endptr, 0 );
	if ( *endptr == '/0' )
		sap->sin_port = htons( port );
	else
	{
		sp = getservbyname( sname, protocol );
		if ( sp == NULL )
			error( 1, 0, "unknown service: %s/n", sname );
		sap->sin_port = sp->s_port;
	}
}

SOCKET tcp_server( char *hname, char *sname )
{
	struct sockaddr_in local;
	SOCKET s;
	const int on = 1;

	set_address( hname, sname, &local, "tcp" );
	s = socket( AF_INET, SOCK_STREAM, 0 );
	if ( !isvalidsock( s ) )
		error( 1, errno, "socket call failed" );

	if ( setsockopt( s, SOL_SOCKET, SO_REUSEADDR,
		( char * )&on, sizeof( on ) ) )
		error( 1, errno, "setsockopt failed" );

	if ( bind( s, ( struct sockaddr * ) &local,
		 sizeof( local ) ) )
		error( 1, errno, "bind failed" );

	if ( listen( s, NLISTEN ) )
		error( 1, errno, "listen failed" );

	return s;
}


int main(int argc,char**argv)
{
   int i,maxi,maxfd,listenfd,connfd,sockfd;
   int nready,client[FD_SETSIZE];
   fd_set rset,alllset,wset,wsetsave;
   char buf[MAXBUFSIZE];
   socklen_t clilen;
   struct sockaddr_in servaddr,cliaddr;
   int rval;
   struct timeval to;
   int n = 0;
   listenfd =  tcp_server(NULL,"5000");


   FD_ZERO(&alllset);
   FD_ZERO(&wsetsave);
   FD_SET(listenfd,&alllset);
   maxfd = listenfd;
   for(i = 0; i < FD_SETSIZE;i++)
	   client[i] = -1;
   maxi = -1;
   while(1)
   {
	FD_ZERO(&rset);
    rset = alllset;
	wset = wsetsave;
	printf("server waiting...,maxfd:%d/n",maxfd);
	nready = select(maxfd+1,&rset,&wset,NULL,NULL);
	if(FD_ISSET(listenfd,&rset))
	{
	   clilen = sizeof(cliaddr);
	   connfd = accept(listenfd,(struct sockaddr*)&cliaddr,&clilen);
	   for(i = 0; i< FD_SETSIZE; i++)
       {
		   if(client[i] < 0)
		   {
                client[i] = connfd;
				break;
		   }
	   }
	   if(i == FD_SETSIZE)
	   {
           printf("too many clients /n");
           exit(1);
	   }
       FD_SET(connfd,&alllset);
	   FD_SET(connfd,&wsetsave);
	   printf("addinng client on fd %d/n",connfd);
	   if(connfd > maxfd)
              maxfd = connfd;
	   if( i > maxi)
               maxi = i;
	   if(--nready <= 0)
                continue;
     
    }
    for(i = 0; i <= maxi; i++)
    {
        if((sockfd = client[i]) < 0 )
		  	continue;
	  	if(FD_ISSET(sockfd,&rset))
	  	{
          bzero(buf,100);
          if((n = recv(sockfd,buf,MAXBUFSIZE,0)) <= 0)
	      {
            close(sockfd);
		    FD_CLR(sockfd,&alllset);
		    client[i] = -1;
		    printf("recv error,client:%d exit/n",sockfd);
		  
	      }
	      else
	      {
            sleep(2);
            printf("serving client read on fd %d,recv datalen: %d,data:%s /n",sockfd,n,buf);
		  	write(sockfd,buf,n);
	      }
	      if(--nready <= 0 )
		  break;    

      	}else if(FD_ISSET(sockfd,&wset)){
          char buf1[100] = {""};
          sprintf(buf1,"Welcome the selectsvr .../n");
          if((n = send(sockfd,buf1,strlen(buf1),0)) <= 0)
     	  {
               close(sockfd);
		       FD_CLR(sockfd,&wsetsave);
		       printf("send error,client:%d exit/n",sockfd);
	      }else{
               FD_CLR(sockfd,&wsetsave);
               printf("serving client write on fd %d,data:%s/n",sockfd,buf1);
	      }
	      sleep(5);
	  }

    }
  } 

}


