#include <sys/socket.h>
#include<arpa/inet.h>
#include<ctype.h>
#include<unistd.h>
#include<stdio.h>
#include<sys/types.h>
#include<strings.h>
#include<sys/wait.h>
#include"wrap.h"

#define SERV_IP "127.0.0.1"
#define SERV_PORT 6666
//"192.168.32.30"

void *wait_child(int signo)//子进程回收函数
{
	while (waitpid(0,NULL,WNOHANG) > 0);
	return ;
}

int main(void)
{

	pid_t pid;
	int sfd,cfd;
		
	struct sockaddr_in serv_addr , clie_addr;
		
        socklen_t clie_addr_len,serv_addr_len;
        memset(&serv_addr,0,sizeof(serv_addr));
        memset(&clie_addr,0,sizeof(clie_addr));

	char buf[BUFSIZ];
    char clie_IP[BUFSIZ];
	int n;
	int i;
        sfd =  Socket(AF_INET,SOCK_STREAM,0);
	 
	 serv_addr.sin_family = AF_INET;//ipv4
	 serv_addr.sin_port = htons(SERV_PORT);
	 serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	 //inet_pton(AF_INET,"192.168.32.30",&serv_addr.sin_addr.s_addr);
	 serv_addr_len = sizeof(serv_addr);
	 Bind(sfd, (struct sockaddr*)&serv_addr, serv_addr_len);
	 
	 Listen(sfd,128);
	 printf("wait for client connect");
	 while(1)//创建子进程  
	 {
		 clie_addr_len = sizeof(clie_addr);
		 cfd = Accept(sfd,(struct sockaddr*)&clie_addr,&clie_addr_len);
		 printf("cfd =  %d\n",cfd);
		 printf("cliet IP:%s,cliet port:%d\n",inet_ntop(AF_INET,&clie_addr.sin_addr.s_addr,clie_IP,sizeof(clie_IP)),ntohs(clie_addr.sin_port));
	
		 pid = fork();
		if (pid <0)
		{
			 perror("fork error");
			 exit(1);
		}
		else if(pid ==0)
		{
			 close(sfd);
			 break;
		}else{
			close(cfd);
			signal(SIGCHLD,wait_child);
		}
	}
	if(pid ==0)//子进程操作
	{
		while(1)
		{
			n = Read(cfd,buf,sizeof(buf));
			if(n == 0)
			{
				close(cfd);
				return 0;
			}else if(n == -1)
			{
				perror("read error");
				exit(1);
			}else			{

				for(i = 0;i < n;i++)
				{
					buf[i] = toupper(buf[i]);
				}	
					write(cfd,buf,n);
					write(STDOUT_FILENO,buf,n);
				
			}
		}
	}
	return 0;
} 







