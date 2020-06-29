#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<ctype.h>
#include<arpa/inet.h>
#include"wrap.h"
#include<unistd.h>
#define SERV_IP "127.0.0.1"
#define SERV_PORT 6666
#define MAXSIZELINE 10
int main(void)
{
	int cfd;

    struct sockaddr_in serv_addr;

	cfd = Socket(AF_INET, SOCK_STREAM, 0);
        memset(&serv_addr,0,sizeof(serv_addr)); 
	serv_addr.sin_family = AF_INET;								//ipv4
	serv_addr.sin_port = htons(SERV_PORT);		//host to net  unsigned short   服务器端口号
//	serv_addr.sin_addr.s_addr = htonl(SERV_IP);		//服务器地址;
	inet_pton(AF_INET,SERV_IP,&serv_addr.sin_addr.s_addr);
	
	//bind();客户端不用绑定，在connect中会绑定
	Connect(cfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));

	char buf[MAXSIZELINE];
	char ch = 'a';
	int i ;
	
	while(1)
	{
		
	for(i = 0; i < MAXSIZELINE/2; i++)
		buf[i] = ch;
	ch++;
	for(i; i < MAXSIZELINE; i++)
		buf[i] = ch;
	ch++;
	
	Write(cfd,buf,sizeof(buf));
	
	int n = Read(cfd,buf,sizeof(buf));
	
    Write(STDOUT_FILENO,buf,n);
	sleep(3);
	}
	
	close(cfd);
}
