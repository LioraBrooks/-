#include"wrap.h"
#define SERV_PORT 5555
#include<sys/epoll.h>

int main()
{
    int lfd,cfd,res,i,tempfd;                                    //listen fd,connect fd
    int epfd;                                   //用于监听红黑树的根节点
    struct epoll_event ep[1024];               //tep用于设置单个fd属性；ep是epoll_wait()传出的满足监听事件的数组
    struct epoll_event tep;


    struct sockaddr_in serv_addr={};
    struct sockaddr_in clit_addr;
    socklen_t clit_addr_len=sizeof(clit_addr);

    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(SERV_PORT);
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);

    lfd=Socket(AF_INET,SOCK_STREAM,0);

    tep.events=EPOLLIN;                         //初始化
    tep.data.fd=lfd;

    Bind(lfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr));

    Listen(lfd,128);

    epfd=epoll_create(1024);                    //创建用于监听的红黑树
    
    res=epoll_ctl(epfd,EPOLL_CTL_ADD,lfd,&tep);      //将lfd加到监听树上
    if(res<0)
    {
        sys_err("epoll_ctl lfd error");
    }

    int n;

    char buf[BUFSIZ];

    while(1)
    {
        res=epoll_wait(epfd,ep,1024,-1);            //阻塞监听
        if(res<0)
        {
            sys_err("epoll_wait error");
        }

        for(i=0;i<res;i++)
        {
            if(!(ep[i].events & EPOLLIN))
            {//如果不是读事件，继续循环
                continue;
            }

            tempfd=ep[i].data.fd;
            if(tempfd==lfd)
            {//lfd满足读事件，有新的客户端发起连接请求
                cfd=Accept(lfd,(struct sockaddr *)&clit_addr,&clit_addr_len);

                tep.events=EPOLLIN;
                tep.data.fd=cfd;

                res=epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&tep);         //加入红黑树
                if(res<0)
                {
                    sys_err("epoll_ctl cfd error");
                }
            }
            else
            {
                //cfd满足读事件，有客户端写数据过来
                n=read(tempfd,buf,sizeof(buf));
                if(n==0)
                {
                    close(tempfd);
                    res=epoll_ctl(epfd,EPOLL_CTL_DEL,tempfd,NULL);

                }
                else if(n>0)
                {
                    for(int j=0;j<n;j++)
                    {
                        buf[j]=toupper(buf[j]);
                    }
                    write(tempfd,buf,n);
                    write(STDOUT_FILENO,buf,n);

                }
                else if(n<0)
                {
                    sys_err("read<0 error");
                    epoll_ctl(epfd,EPOLL_CTL_DEL,tempfd,NULL);          //出错，摘除节点
                    close(tempfd);
                }

            }
        }
    }

    close(lfd);
    close(epfd);
    return 0;
}
