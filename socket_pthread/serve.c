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

/*自定义一个结构体，将sockaddr_in与cfd捆绑*/
struct s_info{
		struct sockaddr_in clie_addr;
		int connectfd; 
};


void *do_work(void *arg)//子线程处理函数
{

		int n,i;
		char buf[MAXSIAE];
		char str[INET_ADDRSTRLEN];//INET_ADDRSTRLEN 16
		
		struct s_info *ts = (struct s_info*)arg;
			
		while(1)
		{
				n = Read(ts->connectfd,buf,MAXSIAE);//读客户端
				if(n == 0)
				{
					printf("the cline %d closed!",ts->connectfd);
					break;
				}
				printf("recevied from %s at PORT %d ",inet_ntop(AF_INET,&(*ts).clie_addr.sin_addr,str,sizeof(str)),ntohs((*ts).clie_addr.sin_port));//打印客户端的信息
			
				for(i = 0;i < n;i++)
					{
						buf[i] = toupper(buf[i]);
					}	
				write(ts->connectfd,buf,n);
				write(STDOUT_FILENO,buf,n);
		}
		
		Close(ts->connectfd);
		return (void*)0;
}
	
int main(void)
{
		
		struct sockaddr_in serv_addr , clie_addr;
		int sfd,cfd;
		
		socklen_t clie_addr_len,serv_addr_len;
		   
		char clie_IP[BUFSIZ];
		int i = 0;
		pthread_t tid;//创建线程
		struct s_info ts[256];//最大线程数创建结构体数组
		
		 sfd =  Socket(AF_INET,SOCK_STREAM,0);
		 bzero(&serv_addr,sizeof(serv_addr));//地址结构初始化
		 serv_addr.sin_family = AF_INET;//ipv4
		 serv_addr.sin_port = htons(SERV_PORT);
		 serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		 //inet_pton(AF_INET,"192.168.32.30",&serv_addr.sin_addr.s_addr);
		 serv_addr_len = sizeof(serv_addr);
		 
		 Bind(sfd, (struct sockaddr*)&serv_addr, serv_addr_len);
//		 printf("wait for client connect:::");
 
		 Listen(sfd,128);

		 printf("wait for client connect::\n");
	 
	 while(1) 
	 {
		 clie_addr_len = sizeof(clie_addr);
		 cfd = Accept(sfd,(struct sockaddr*)&clie_addr,&clie_addr_len);//阻塞监听
		 printf("cfd =  %d\n",cfd);
		
		ts[i].clie_addr = clie_addr;//取出接收到的客户端addr结构体
		ts[i].connectfd = cfd;//文件描述符
		//传递到pthead_create 子线程执行
		
	    //线程管理，当线程达到最大数时候进行处理
		pthread_create(&tid,NULL,do_work,(void*)&ts[i]);
		pthread_detach(tid);//子线程分离，防止僵尸线程产生
		i++;
	}
	
	return 0;
} 







