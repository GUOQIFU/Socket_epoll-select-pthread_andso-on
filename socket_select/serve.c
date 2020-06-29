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
#include"wrap.h"


#define MAXSIAE 10240
#define SERV_IP "127.0.0.1"
#define SERV_PORT 6666
//"192.168.32.30"

	
int main(void)
{
		int i,j,n,maxi;//maxi 定义为指向client[]  最后一个元素的下标
		int nready,client[FD_SETSIZE];//自定义的存放监听文件描述符的数组，默认1024  将描述符单独存放，避免以后遍历
		char buf[BUFSIZ],str[INET_ADDRSTRLEN];////INET_ADDRSTRLEN 16,存放IP
		char clie_IP[BUFSIZ];
		int listenfd,connectfd,socketfd,maxfd;
		socklen_t clie_addr_len,serv_addr_len;   
		/*集合*/fd_set rset,allset;//rset 读事件文件描述符的集合   allset用来暂时存储监听事件listenfd、
		
		struct sockaddr_in serv_addr , clie_addr;
		listenfd =  Socket(AF_INET,SOCK_STREAM,0);
		bzero(&serv_addr,sizeof(serv_addr));//地址结构初始化
		serv_addr.sin_family = AF_INET;//ipv4
		serv_addr.sin_port = htons(SERV_PORT);
		serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		//inet_pton(AF_INET,"192.168.32.30",&serv_addr.sin_addr.s_addr);
		
		serv_addr_len = sizeof(serv_addr);
		Bind(listenfd, (struct sockaddr*)&serv_addr, serv_addr_len);
		Listen(listenfd,128);
		
		printf("wait for client connect::\n");
		maxfd = listenfd;//初始的第一个lfd是最大的，赋给maxfd
		/*listenfd的值是文件描述符的序号，初始为3号描述符*/
		maxi = -1;
		for(i = 0; i < FD_SETSIZE;i++)
		{
			client[i] = -1;//将存放监听文件描述符的数组全置为-1，方便以后放入监听值
		}
		
	    FD_ZERO(&allset);//清空暂存rset事件缓存区
		FD_SET(listenfd,&allset);//构造文件描述符类型的集合,并将listefd添加进去
	 
	 
	 while(1) 
	 {
		 rset = allset;//每次循环时都重新设置select监控合集
		 nready = select(maxfd+1,&rset,NULL,NULL,NULL);//neady 是 select 函数返回的监听的描述符的个数
		                                               //rset集合中暂时存储了需要监听的文件描述符
		 /*参数1：最大描述符+1
		 参数2：读事件的集合
		 参数3：写事件的集合
		 参数4：异常事件
		 参数5：时间，NULL表示永久等待
		 */
		 if(nready < 0)
		 {
			 perr_exit("select error");
		 }
		 
		 if(FD_ISSET(listenfd,&rset))//是否有请的客户端请求，有的话将新生成的文件描述符添加到client数组
		 {
			clie_addr_len = sizeof(clie_addr);
			connectfd = Accept(listenfd,(struct sockaddr*)&clie_addr,&clie_addr_len);
			printf("recevied from %s at PORT %d ",
					inet_ntop(AF_INET,&clie_addr.sin_addr.s_addr,str,sizeof(str)),
					ntohs(clie_addr.sin_port));//打印客户端的信息
                        printf("connectfd =  %d\n",connectfd);

		 
			for(i = 0;i < FD_SETSIZE;i++)
			{
				if(client[i] < 0)//将客户端的文件描述符存放进client[]数组，从头开始存放
				{
					client[i] = connectfd;
					break;
				}
			}
			if(i == FD_SETSIZE)
			{
				fputs("too many clients\n",stderr);
				exit(1);
			}
			
			FD_SET(connectfd,&allset);//添加connectfd描述符
			if(connectfd > maxfd)
			{
				maxfd = connectfd ;//select 参数1需要：最大描述符+1
			}
		    if(i > maxi)
			{
				maxi = i;
			}
			if(--nready == 0)//
				continue;
		}
		
		/*在已经监听过的client数组中的fd 进行读写操作*/
		for(i = 0;i <= maxi;i++)//
		{
			if((socketfd = client[i]) < 0)
			{
				continue;
			}
			if(FD_ISSET(socketfd,&rset))//判断socketfd是否在rset集合中
			{
				if((n = Read(socketfd,buf,sizeof(buf))) == 0)
				{
					Close(socketfd);
					FD_CLR(socketfd,&allset);//解除allset对文件描述符的监控
					client[i] = -1;
				}else if(n > 0)//n 实际读到的字节数
				{
					for(j = 0;j < n;j++)
					{
						buf[j] = toupper(buf[j]);
					}
						sleep(10);
						Write(socketfd,buf,n);
						Write(STDOUT_FILENO,buf,n);
					if(-- nready == 0)
						break;
				}
			}
		}
	}
	Close(listenfd);
		return 0;
} 







