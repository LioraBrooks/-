#include"wrap.h"

void sys_err(const char *str)
{
        perror(str);
        exit(1);
}

 int Socket(int domain, int type, int protocol)
{
	int fd=socket(domain,type,protocol);
        if(fd==-1)
        {
                sys_err("socket error");
        		return fd;
        }
	return fd;
}

int Bind(int sockfd, const struct sockaddr *addr,socklen_t addrlen)
{
	int bfd=0;
        bfd=bind(sockfd,addr,addrlen);
        if(bfd==-1)
        {
                sys_err("bind error");
	        	return bfd;
        }
	return 0;
}

int Listen(int sockfd, int backlog)
{
	int lfd=0;
        lfd=listen(sockfd,backlog);
        if(lfd==-1)
        {
                sys_err("listen error");
	        	return lfd;
        }
	return 0;
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	int afd=0;
        afd=accept(sockfd,addr,addrlen);
        if(afd==-1)
        {
                sys_err("accept error");
	        	return afd;
        }
	return afd;
}

int Connect(int sockfd, const struct sockaddr *addr,socklen_t addrlen)
{
	int res=connect(sockfd,addr,addrlen);
        if(res!=0)
        {
                sys_err("socket error");
	        	return res;
        }
	return 1;
}

