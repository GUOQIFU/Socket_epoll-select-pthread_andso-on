#include <sys/socket.h>
#include<arpa/inet.h>
#include<ctype.h>
#include<unistd.h>
#include<stdio.h>
#include<sys/types.h>
#include<strings.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<errno.h>
#include<sys/epoll.h>
#include<time.h>
#include<sys/stropts.h>
#include"wrap.h"


#define MAXSIZE 10
#define SERV_IP "127.0.0.1"
#define SERV_PORT 6666
//"192.168.32.30"
#define OPEN_MAX 5000
	
int main(void)
{
		int i,j,n;
		ssize_t nready,efd,res;
		char buf[MAXSIZE],str[INET_ADDRSTRLEN];////INET_ADDRSTRLEN 16,存放IP
		int listenfd,connectfd,socketfd;
		socklen_t clie_addr_len,serv_addr_len;   
		
		struct epoll_event tep,ep[OPEN_MAX];//定义epoll_event结构体  
		/*
		struct epoll_event
			{
			  uint32_t events;  
			  epoll_data_t data;   
			} ;
		*/
		
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
		
		efd = epoll_create(OPEN_MAX);//返回值 -1 失败 
		/*创造一颗红黑树，efd为根节点，epoll通过红黑树存储结构来存储 监听文件描述符的集合
		红黑树 每个节点 添加用 epoll_ctl函数，添加lfd(是一个结构体，里面包含了tep 结构体--事件类型)
		*/
		if(efd == -1)
		{
			perr_exit("epoll_create error");
		}
		
		tep.events = EPOLLIN | EPOLLET;
		tep.data.fd = listenfd;
		res = epoll_ctl(efd,EPOLL_CTL_ADD,listenfd,&tep);//把 tep 结构体加载到efd 红黑树节点上
		/*
		   第一个参数是epoll_create()的返回值，
           第二个参数表示动作，用三个宏来表示：
           EPOLL_CTL_ADD：       注册新的fd到epfd中；
           EPOLL_CTL_MOD：      修改已经注册的fd的监听事件；
           EPOLL_CTL_DEL：        从epfd中删除一个fd；
         第三个参数是需要监听的fd，
          第四个参数是告诉内核需要监听什么事件，structepoll_event： 
		*/
		if(res == -1)
		{
			perr_exit("epoll_ctl error");
		}
		
		while(1)	
		{
			nready = epoll_wait(efd,ep,OPEN_MAX,-1);
			/*参数1：epoll_create()的返回值
			  参数2：ep 是 epoll_event 类型数组
			  参数3：类型数组的容量
			  参数4；time参数 -1 阻塞监听 
			  返回整型值。
			*/
			if(nready == -1)
			{
				perr_exit("epoll_wait error");
			}
			
			for(i = 0;i < nready ;i++)
			{
				if(!(ep[i].events & EPOLLIN))//如果非读事件，继续循环
					          continue;
				
				if(ep[i].data.fd == listenfd)//监听是有否新的请求
				{
					clie_addr_len = sizeof(clie_addr);
					connectfd = Accept(listenfd,(struct sockaddr*)&clie_addr,&clie_addr_len);
			
					printf("recevied from %s at PORT %d ",
					inet_ntop(AF_INET,&clie_addr.sin_addr.s_addr,str,sizeof(str)),
					ntohs(clie_addr.sin_port));//打印客户端的信息
					printf("connectfd =  %d   client = %d \n",connectfd,++num);
                 
				 
				    flag = fcntl(connectfd,F_GETFL);//修改connfd为非阻塞读
					flag |= O_NONBLOCK;
					fcntl(connectfd,F_SETFL,flag);
				 
					tep.events = EPOLLIN;
					tep.data.fd = connectfd;
				 
					res = epoll_ctl(efd,EPOLL_CTL_ADD,connectfd,&tep);
				 
					if(res == -1)
						perr_exit("epoll_ctl error");
				 
				}
				else//读事件的发生
				{
					 socketfd = ep[i].data.fd;
					 while(n = Read(socketfd,buf,MAXSIZE/2) > 0)
					 {
							if(n == 0)//读到0，表示客户端关闭连接 ？？？？？？？
						 {
							  res = epoll_ctl(efd,EPOLL_CTL_DEL,socketfd,NULL);//将 文件从红黑树上删除
							  if(res == -1)
							  {
								  perr_exit("epoll_ctl error");
							  }
							  Close(socketfd);
							  printf("client[%d] closed connection\n",socketfd);
						 }
						 else if(n < 0)
						 {
							 perror("error read n < 0 ");
							 res = epoll_ctl(efd,EPOLL_CTL_DEL,socketfd,NULL);
							 Close(socketfd);
						 }
						 else
						 {
							 Write(STDOUT_FILENO,buf,n);
						 } 
					 }
					 
					 
				}
			}
		}
	Close(listenfd);
	Close(efd);
		return 0;		
}
	 
	 
	 
	
