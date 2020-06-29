#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>

void perr_exit(const char *s)//出错处理封装
{
	perror(s);
	exit(-1);
	
}

int Accept(int fd,struct sockaddr *sock,socklen_t *sock_len)
{
        int n;
again:
        if((n = accept(fd,sock,sock_len))<0)
        {
                if((errno == ECONNABORTED)||(errno == EINTR))//防止程序呗阻塞期间报错，
                        goto again;
                else
                        perr_exit("accept error");
        }
        return n;
}



int Bind(int fd,const struct sockaddr *sock,socklen_t sock_len)
{
        int n;
        if((n = bind(fd,sock,sock_len)) < 0)
                perr_exit("bind error");
        return n;
}


int Socket(int family,int type, int protocol )
{
	int n ;
	if((n = socket(family,type,protocol))<0)
		perr_exit("listen error");
	return n;
}


int Listen(int fd,int connect_number)
{
	int n ;
	if((n = listen(fd,connect_number))<0)
		perr_exit("listen error");
	return n;
}



int Connect(int fd,const struct sockaddr *sock,socklen_t sock_len)
{
	int n;
	n = connect(fd,sock,sock_len);
	printf("connect() return %d",n);
	if(n<0){
            perr_exit("connect error");
		}	
	return n;
}

ssize_t Read(int fd,void *ptr,size_t nbytes)//慢速进程调用，可能会造成堵塞，read 实际返回值为读到的字节数 
                                              
{
	ssize_t n;
again:
	 if((n=read(fd,ptr,nbytes)) <0){
		 if(errno == EINTR)
		  goto again;	
		 else
		 return -1;
		}
         return n;
}


ssize_t Write(int fd,void *ptr,size_t nbytes)
{
	ssize_t n;
again:
	 if((n = write(fd,ptr,nbytes)) <0){
		 if(errno == EINTR)
			 goto again;
		
		 else
		 return -1;
	}
     return n;
}


int Close(int fd)
{
	int n;
	if((n = close(fd))<0)
		perr_exit("close error");
     return n;
}

//参数三

ssize_t Readn(int fd,void *vptr,size_t n)
{
	size_t nleft;
	ssize_t nread;
	char *ptr;
	
	ptr = vptr;
	
	nleft = n;
	
	while(nleft >0){
		if((nread = read(fd,ptr,nleft)) < 0){
			if(errno == EINTR)
				nread == 0;
			else return -1;
		}
		else if(nread == 0)
			break;
		nleft -= nread;
		ptr += nread;
	}
	return n - nleft;
}

static ssize_t my_read(int fd,char *ptr)
{
	static int read_cnt;
	static char *read_ptr=NULL;
	static char read_buf[64];
	
	if(read_cnt <= 0){
		again:
		if((read_cnt = read(fd,read_buf,sizeof(read_buf))) <0 ){
			if(errno == EINTR)
				goto again;
			return -1;
		}else if(read_cnt == 0)
			return 0;
		read_ptr = read_buf;
	}
	read_cnt--;
	*ptr = *read_ptr++;
	return 1;
}

ssize_t Writen(int fd,void *vptr,size_t n)
{
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;
	
	ptr = vptr;

	
	nleft = n;
	
	while(nleft >0){
		if((nwritten = write(fd,ptr,nleft)) <= 0){
			if(nwritten <0 && errno == EINTR)
				nwritten == 0;
			else return -1;
		}

		nleft -= nwritten;
		ptr += nwritten;
	}
	return n;
}

//readline   fgets
//传出参数 vptr
ssize_t Readline(int fd,void *vptr,size_t maxlen)
{
	ssize_t n,rc;
	char c,*ptr;
	ptr = vptr;
	
	for(n = 1;n < maxlen;n++)
	{
		if((rc = my_read(fd,&c)) == 1)
		{
			*ptr++ = c;
	  	    if(c == '\n')
				break;
		}
			     
	     	else if(rc == 0){
				*ptr = 0;
			     return n-1;
			}
			else 
				return -1;
	}
	*ptr = 0;
	return n;
}





































