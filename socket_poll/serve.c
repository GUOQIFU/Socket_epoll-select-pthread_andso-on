#include <sys/socket.h>
#include<arpa/inet.h>
#include<ctype.h>
#include<unistd.h>
#include<stdio.h>
#include<sys/types.h>
#include<strings.h>
#include<sys/wait.h>
#include<pthread.h>
#include<fcntl.h>
#include<errno.h>
#include<poll.h>
#include<time.h>
#include<sys/stropts.h>
#include"wrap.h"


#define MAXSIAE 10240
#define SERV_IP "127.0.0.1"
#define SERV_PORT 6666
//"192.168.32.30"
#define OPEN_MAX 1024
	
int main(void)
{
		int i,j,n,maxi;
		int nready;
		char buf[BUFSIZ],str[INET_ADDRSTRLEN];////INET_ADDRSTRLEN 16,存放IP
		int listenfd,connectfd,socketfd;
		socklen_t clie_addr_len,serv_addr_len;   
		
		struct pollfd client[OPEN_MAX];//定义 poll 数组
		struct sockaddr_in serv_addr , clie_addr;
		
		listenfd =  Socket(AF_INET,SOCK_STREAM,0);
		int opt = 1 ;
		setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));//端口复用函数，，，
		
		
		bzero(&serv_addr,sizeof(serv_addr));//地址结构初始化
		serv_addr.sin_family = AF_INET;//ipv4
		serv_addr.sin_port = htons(SERV_PORT);
		serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		//inet_pton(AF_INET,"192.168.32.30",&serv_addr.sin_addr.s_addr);
		
		serv_addr_len = sizeof(serv_addr);
		Bind(listenfd, (struct sockaddr*)&serv_addr, serv_addr_len);
		Listen(listenfd,128);
		printf("wait for client connect::\n");
		
		client[0].fd = listenfd;//需要监听的第一个文件描述符 存入client[0];
		client[0].events = POLLIN;//listenfd 读事件监听  POLLIN 读事件  POLLOUT 写事件  POLLERR 异常事件
		
		for(i = 1; i < OPEN_MAX;i++)
		{
			client[i].fd = -1;//将存放监听文件描述符的数组全置为-1，方便以后放入监听值
		}
		
	    maxi = 0;	
		for( ; ; )
		{
			nready = poll(client,maxi+1,0);//阻塞监听 是否有客户端连接请求，返回请求的fd个数
			/*参数1：struct poll结构体数组
			  参数2：最大监听文件描述符+1
			  参数3：时间
			*/
			if(client[0].revents & POLLIN)//return 读事件
			{
                                printf("guoqi4\n");
				clie_addr_len = sizeof(clie_addr);
				connectfd = Accept(listenfd,(struct sockaddr*)&clie_addr,&clie_addr_len);
				printf("guoqi5\n");
				printf("recevied from %s at PORT %d ",
					inet_ntop(AF_INET,&clie_addr.sin_addr.s_addr,str,sizeof(str)),
					ntohs(clie_addr.sin_port));//打印客户端的信息
                                 printf("connectfd =  %d\n",connectfd);
		
				for(i = 0;i < OPEN_MAX;i++)
				{
					if(client[i].fd < 0)//将客户端的文件描述符存放进client[]数组，从头开始存放
					{
						client[i].fd = connectfd;
						break;
					}
				}
				if(i == OPEN_MAX)
				{
					perr_exit("too many clients\n");
				}
				
				client[i].events = POLLIN;
				if(i > maxi)
				{
					maxi = i;
				}
				
				if(--nready <= 0)
					continue;
			}
		
			/*在已经监听过的client数组中的fd 进行读写操作*/
			for(i = 0;i <= maxi;i++)//
			{
				if((socketfd = client[i].fd) < 0)
				{
					continue;
				}
				if(client[i].revents&POLLIN)//POLLIN 读事件  POLLOUT 写事件  POLLERR 异常事件
				{
					if((n = Read(socketfd,buf,sizeof(buf))) < 0)
					{
						if(errno == ECONNRESET)//收到RST标志
						{
							printf("client[%d] aborted connection\n",i);
							Close(socketfd);
							client[i].fd = -1;//将监听数组该目标置为-1
						}else{
							perr_exit("read error");
						}
					}
					else if(n == 0)//n 实际读到的字节数
					{
						printf("client[%d] closed connection\n",i);
						Close(socketfd);
						client[i].fd = -1;//将监听数组该目标置为-1
						
					}
					else 
					{
						for(j = 0;j < n;j++)
						{
							buf[j] = toupper(buf[j]);
						}
							sleep(2);
							Write(socketfd,buf,n);
							Write(STDOUT_FILENO,buf,n);
					}
				  if(-- nready <= 0)
                                     break;
				}
			}
		}
	Close(listenfd);
		return 0;		
}
	 
	 
	 
	
