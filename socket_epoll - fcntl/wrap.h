#ifndef _WARP_H_
#define _WARP_H_

void perr_exit(const char *s);
int Socket(int family,int type, int protocol );
int Bind(int fd,const struct sockaddr *sock,socklen_t sock_len);
int Listen(int fd,int connect_number);
int Accept(int fd,struct sockaddr *sock,socklen_t *sockt_len);
int Connect(int fd,const struct sockaddr *sock,socklen_t sock_len);
ssize_t Read(int fd,void *ptr,size_t nbytes);
ssize_t Write(int fd,void *ptr,size_t nbytes);
int Close(int fd);
/*ssize_t Readn(int fd,const void *vptr,size_t n);
static ssize_t my_read(int fd,char *ptr);
ssize_t Writen(int fd,const void *vptr,size_t n);
ssize_t Readline(int fd,void *vptr,size_t maxlen);
*/
#endif
