#include<ctype.h>
#include<unistd.h>
#include<stdio.h>
#include<strings.h>
#include<sys/wait.h>
#include<sys/epoll.h>
#include<time.h>
#define MAXSIZELINE 10

int main(void)
{
	int pfd[2];//创建管道准备
	int efd,i;
	pid_t pid;
	char buf[MAXSIZELINE];
	char ch = '1';
	pipe(pfd);//创建管道
	
	pid = fork();
	if(pid == 0)//子进程 写
	{
		close(pfd[0]);//关闭 读
		while(1)
		{
			for(i = 0; i < MAXSIZELINE / 2;i++)
				buf[i] = ch;
			buf[i-1] = '\n';ch++;
			for(i; i < MAXSIZELINE;i++)
				buf[i] = ch;
			buf[i-1] = '\n';ch++;
			
			write(pfd[1],buf,MAXSIZELINE);//
			sleep(3);//每次休眠3秒再次写入
		}
			close(pfd[1]);
	}
	else if(pid > 0)//父进程 读
	{
       	close(pfd[1]);
		struct epoll_event event;
		struct epoll_event resevent[MAXSIZELINE];
		int res , len;
		efd = epoll_create(MAXSIZELINE);
		//event.events = EPOLLIN | EPOLLET;//ET 边沿触发模式
		event.events = EPOLLIN;          //默认 LT触发模式 
		event.data.fd = pfd[0];
		epoll_ctl(efd,EPOLL_CTL_ADD,pfd[0],&event);
		
		while(1)
		{
			res = epoll_wait(efd,resevent,MAXSIZELINE,-1);
			printf("res = %d",res);
                        printf("\n");
			if(resevent[0].data.fd == pfd[0])
			{
				len = read(pfd[0],buf,MAXSIZELINE/2);//测试触发模式，只读一半的数据，留一半在缓冲区内
				write(STDOUT_FILENO,buf,len);
			}
		}
		
	}
}




















