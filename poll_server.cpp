#include"wrap.h"
#include<poll.h>
#define SERV_PORT 8000

int main()
{
    int lfd,cfd,res,i;
    int nready,maxi,tepfd;
    struct sockaddr_in serv_addr={},clit_addr;
    socklen_t clit_addr_len=sizeof(clit_addr);
    char buf[BUFSIZ];

    struct pollfd pfd[1024];                                      //用于监听的文件描述符数组

    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(SERV_PORT);
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);

    lfd=Socket(AF_INET,SOCK_STREAM,0);

    int opt=1;
    res=setsockopt(lfd,SOL_SOCKET,SO_REUSEPORT,&opt,sizeof(opt));
    if(res<0)
    {
        sys_err("setsockopt error");
    }

    Bind(lfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
    Listen(lfd,128);
    
    pfd[0].fd=lfd;                                              //监听lfd
    pfd[0].events=POLLIN;                                      //lfd监听的是读事件
    pfd[0].revents=0;

    for(i=1;i<1024;i++)
    {
        pfd[i].fd=-1;                                           //用-1初始化数组中剩余的文件描述符
        pfd[i].events=0;
        pfd[i].revents=0;
    }

    maxi=0;                                                     //pfd[]数组中有效元素的最大下标

    int n;

    while(1)
    {
        nready=poll(pfd,maxi+1,-1);                             //阻塞监听是否有客户端连接请求
        if(nready<0)
        {
            sys_err("poll error");
        }

        if(pfd[0].revents & POLLIN)                              //listenfd有读事件就绪
        {                                       
            cfd=Accept(lfd,(struct sockaddr *)&clit_addr,&clit_addr_len);       //接收客户端请求
            
            for(i=1;i<1024;i++)
            {
                if(pfd[i].fd<0)
                {
                    pfd[i].fd=cfd;                              //寻找pfd[]数组中的空位，将cfd放入
                    pfd[i].events=POLLIN;                           //设置cfd，监听读事件
                    pfd[i].revents=0;

                    break;
                }
            }
                if(i==1024)                                     //达到最大客户端数
                {
                    sys_err("too many clients");
                }

                if(maxi<i)                                      
                {
                    maxi=i;                                     //更新最大下标
                }

                if(--nready==0)
                {
                    continue;                                   //没有更多就绪事件时，返回poll继续阻塞
                }
        }

        for(i=1;i<=maxi;i++)                                    //若前面的if条件没满足，说明没有listenfd符合条件，继续处理pfd[],看看哪个connectfd满足
        {
            if((tepfd=pfd[i].fd)<0)
            {
                continue;
            }
            if(pfd[i].revents & POLLIN)
            {
                if((n=read(tepfd,buf,sizeof(buf)))<0)
                {
                    if(errno==ECONNRESET)
                    {
                        //收到RST标志
                        cout<<"client "<<i<<" aborted connection"<<endl;
                        close(tepfd);
                        pfd[i].fd=-1;
                        pfd[i].events=0;
                        pfd[i].revents=0;
                    }
                    else
                    {
                        sys_err("read error");
                    }
                }
                else if(n==0)
                {
                    //说明客户端先关闭连接
                    cout<<"client "<<i<<" closed connection"<<endl;
                    close(tepfd);
                    pfd[i].fd=-1;
                    pfd[i].events=0;
                    pfd[i].revents=0;
                    
                }
                else
                {
                    for(int j=0;j<n;j++)
                    {
                        buf[j]=toupper(buf[j]);
                    }
                    write(tepfd,buf,n);
                    write(STDOUT_FILENO,buf,n);
                }
                if(--nready==0)
                {//如果这是最后一个就绪事件，跳出循环
                    break;
                }
            }
        }

    }
    close(lfd);

    return 0;

}
