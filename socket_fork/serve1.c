#include <sys/socket.h>
#include<arpa/inet.h>
#include<ctype.h>
#include<unistd.h>
#include<stdio.h>
#include<sys/types.h>
#include"wrap.h"

#define SERV_IP "127.0.0.1"
#define SERV_PORT 6666


int main(void)
{

	int sfd,cfd;
		
	struct sockaddr_in serv_addr , clie_addr;
		
    socklen_t clie_addr_len,serv_addr_len;
        memset(&serv_addr,0,sizeof(serv_addr));
        memset(&clie_addr,0,sizeof(clie_addr));

	char buf[BUFSIZ];
        char clie_IP[BUFSIZ];
	int n;
	int i;
     sfd =  socket(AF_INET,SOCK_STREAM,0);
	 
	 serv_addr.sin_family = AF_INET;//ipv4
	 serv_addr.sin_port = htons(SERV_PORT);
	 serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	 
	 serv_addr_len = sizeof(serv_addr);
	 bind(sfd, (struct sockaddr*)&serv_addr, serv_addr_len);
	 
	 listen(sfd,5);
	 printf("wait for client connect");
	 
	 clie_addr_len = sizeof(clie_addr);
	 
	 cfd = accept(sfd,(struct sockaddr*)&clie_addr,&clie_addr_len);
	 printf("cfd =  %d\n",cfd);
	 printf("cliet IP:%s,cliet port:%d\n",inet_ntop(AF_INET,&clie_addr.sin_addr.s_addr,clie_IP,sizeof(clie_IP)),ntohs(clie_addr.sin_port));
	 while(1)
	 {
	 n = read(cfd,buf,sizeof(buf));
	  for( i = 0;i<n;i++)
		  buf[i] = toupper(buf[i]);
	 
	 write(cfd,buf,n);
	 }
	 close(sfd);
	 close(cfd);
	 
	return 0;
}







