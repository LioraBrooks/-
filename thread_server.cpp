#include"wrap.h"

#define SERV_PORT 7777

struct s_info
{//定义一个结构体，将地址与文件描述符捆绑
    struct sockaddr_in clit_addr;
    int cfd;
};

void *tfn(void *arg)
{
    //子线程
    char buf[BUFSIZ];
    int ret;

    struct s_info *cli_info=(struct s_info *)arg;
    char str[INET_ADDRSTRLEN];      //#define INET_ADDRSTRLEN 16

    while(1)
    {
         ret=read(cli_info->cfd,buf,sizeof(buf));
         if(ret==0)
         {
             cout<<"the client "<<cli_info->cfd<<"closed."<<endl;
             break;
         }
    

         //打印客户端信息（IP/PORT）
         cout<<"received from "<<inet_ntop(AF_INET,&(*cli_info).clit_addr.sin_addr,str,sizeof(str))
             <<" at port "<<ntohs((*cli_info).clit_addr.sin_port)<<endl;

         for(int i=0;i<ret;i++)
         {
             buf[i]=toupper(buf[i]);
         }

         write(cli_info->cfd,buf,ret);
         write(STDOUT_FILENO,buf,ret);
    }

    close(cli_info->cfd);

    return NULL;
}

int main()
{
    pthread_t tid;
    struct sockaddr_in serv_addr = {};
    struct sockaddr_in clit_addr;

    socklen_t clit_addr_len;

    struct s_info cinfo[256];       //创建结构体数组

    int res,lfd,cfd;
    int i=0;

    lfd=Socket(AF_INET,SOCK_STREAM,0);

    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(SERV_PORT);
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);

    //设置端口复用，跳过2MSL
    int opt=1;
    res=setsockopt(lfd,SOL_SOCKET,SO_REUSEADDR,(void *)&opt,sizeof(opt));

    Bind(lfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr));

    Listen(lfd,128);

    clit_addr_len=sizeof(clit_addr);

    cout<<"Accepting client connect..."<<endl;

    while(1)
    {
        cfd=Accept(lfd,(struct sockaddr *)&clit_addr,&clit_addr_len);
        
        cinfo[i].clit_addr=clit_addr;
        cinfo[i].cfd=cfd;

        res=pthread_create(&tid,NULL,tfn,(void *)&cinfo[i]);
        if(res!=0)
        {
            sys_err("pthread_create error");
        }

        res=pthread_detach(tid);
        if(res!=0)
        {
            sys_err("pthread_detach error");
        }

        i++;

    }

    return 0;
}
