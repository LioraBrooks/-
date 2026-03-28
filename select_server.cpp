#include "wrap.h"

#define SERV_PORT 8888

int main()
{
    int listenfd,connfd;                        //connect fd
    int i,res,maxfd=0;
    struct sockaddr_in serv_addr= {},clit_addr;
    socklen_t clit_addr_len=sizeof(clit_addr);
    char buf[BUFSIZ];
    fd_set rset,allset;                         //定义监听读集合rset、备份集合allset

    int clit[FD_SETSIZE],maxi,nready;                       //自定义数组clit，防止轮询1024个文件描述符.FD_SETSIZE默认为1024,

    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(SERV_PORT);
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);

    listenfd=Socket(AF_INET,SOCK_STREAM,0);

    int opt=1;
    res=setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,(void *)&opt,sizeof(opt));
    if(res!=0)
    {
        sys_err("setsockopt error");
    }

    Bind(listenfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr));

    Listen(listenfd,128);

    FD_ZERO(&allset);                       //将总的监听集合全部置零

    FD_SET(listenfd,&allset);               //将用于监听的文件描述符添加至总的监听集合

    maxfd=listenfd;                         //最大文件描述符

    maxi=-1;                                //用作clit[]的下标，初始值指向0个元素之前下标位置
    for(i=0;i<FD_SETSIZE;i++)
    {
        clit[i]=-1;                         //用-1初始化clit[]
    }

    while(1)
    {
        rset=allset;                        //用rset给allset备份
        nready=select(maxfd+1,&rset,NULL,NULL,NULL);       //使用select监听
        if(nready<0)
        {
            sys_err("select error");
        }

        if(FD_ISSET(listenfd,&rset))         //listenfd满足 读事件
        {
            connfd=Accept(listenfd,(struct sockaddr *)&clit_addr,&clit_addr_len);       //不阻塞建立连接

            for(i=0;i<FD_SETSIZE;i++)
            {
                if(clit[i]<0)                //找clit[]中没有被使用过的位置           
                {
                    clit[i]=connfd;
                    break;
                }
            }

            if(i==FD_SETSIZE)               //达到监听上限
            {
                cout<<"too many clients"<<endl;
                exit(1);
            }

            FD_SET(connfd,&allset);          //将新产生的fd加入到监听集合中，监听数据的读事件

            if(maxfd<connfd)                
            {
                maxfd=connfd;                //更新最大文件描述符
            }

            if(i>maxi)
            {
                maxi=i;                     //保证maxi总是clit[]的最后一个下标
            }

            if(res==1)
            {
                                             //说明select只返回一个，并且是listenfd，因此后续无需执行
                continue;
            }

        }

            //处理满足读事件的fd
            for(i=0;i<=maxi;i++)
            {
                int sockfd;
                if((sockfd=clit[i])<0)
                {
                    continue;
                }

                if(FD_ISSET(sockfd,&rset))               //找到满足读事件的fd
                {
                    //从客户端读
                    int ret=read(sockfd,buf,sizeof(buf));
                    if(ret==0)
                    {
                        close(sockfd);                   //客户端结束通信
                        FD_CLR(sockfd,&allset);          //将关闭的fd从总集合中移除
                        clit[i]=-1;
                    }
                    else if(ret<0)
                    {
                        sys_err("read error");
                    }

                    for(int j=0;j<ret;j++)
                    {
                        //进行小写转大写
                        buf[j]=toupper(buf[j]);
                    }

                    //写回给客户端
                    write(sockfd,buf,ret);

                    //显示至屏幕
                    write(STDOUT_FILENO,buf,ret);

                    if(nready==1)
                    {
                        continue;                   //只监听到1个fd，并且是listenfd，跳出for，但还在while中
                    }

                }

            }

        }

        close(listenfd);

        return 0;

    }

