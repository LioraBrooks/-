#include<signal.h>
#include"wrap.h"
#include<sys/wait.h>
#define SERV_PORT 6666

void catch_child(int signo)
{
    while( (waitpid(0,NULL,WNOHANG)) >0);

    return;
}

int main()
{
    int lfd,cfd,res;
    struct sockaddr_in serv_addr = {};    //定义并将地址结构清0
    struct sockaddr_in clit_addr;
    socklen_t clit_addr_len;
    pid_t pid;
    char buf[BUFSIZ];

    //将地址结构清零
    // memset(&serv_addr,0,sizeof(serv_addr));

    //或者：bzero是非标准函数，一般不使用
    //bzero(&serv_addr,sizeof(serv_addr));      //#include<strings.h>

    //初始化
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(SERV_PORT);
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);

    //创建套接字
    lfd=Socket(AF_INET,SOCK_STREAM,0);

    //设置端口复用
    int opt=1;
    res=setsockopt(lfd,SOL_SOCKET,SO_REUSEPORT,(void *)&opt,sizeof(opt));
    if(res!=0)
    {
        sys_err("setsockopt error");
    }

    //绑定地址结构（IP+PORT）
    Bind(lfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr));

    //设置与服务器的连接上限
    Listen(lfd,128);

    clit_addr_len=sizeof(clit_addr);
    while(1)
    {
        //阻塞等待客户端建立连接
        cfd=Accept(lfd,(struct sockaddr *)&clit_addr,&clit_addr_len);

        pid=fork();         //创建子进程
        if(pid<0)
        {
            sys_err("fork error");
        }
        else if(pid==0)
        {
            close(lfd);         //关闭用于建立连接的套接字

             for(;;)
             {
                 //子进程处理客户端通信
                 res=read(cfd,buf,sizeof(buf));
                 if(res==-1)
                 {
                     sys_err("read error");
                 }
                 if(res==0)
                 {
                     //读至文件末尾关闭用于和客户端通信的套接字
                     close(cfd);
                     exit(0);//子进程退出
                 }
                 for(int i=0;i<res;i++)
                 {
                     //小写转大写
                     buf[i]=toupper(buf[i]);
                 }
                //回写给客户端
                 write(cfd,buf,res);
                // 打印到屏幕
                 write(STDOUT_FILENO,buf,res);
             }

        }
        else
        {
            //父进程注册回收函数
            close(cfd);
            struct sigaction act;

            act.sa_handler=catch_child;
            sigemptyset(&act.sa_mask);
            act.sa_flags=0;

            res= sigaction(SIGCHLD,&act,NULL);
            if(res!=0)
            {
                sys_err("sigaction error");
            }
            continue;
        }

    }
    return 0;
}
