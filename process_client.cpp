#include"wrap.h"
#define SERV_PORT 6666 


int main()
{
	struct sockaddr_in serv_addr;

	serv_addr.sin_family=AF_INET;
	serv_addr.sin_port=htons(SERV_PORT);
	inet_pton(AF_INET,"127.0.0.1",&serv_addr.sin_addr.s_addr);

	int cfd=Socket(AF_INET,SOCK_STREAM,0);

	int res=Connect(cfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr));

//	int counter=5;
	char buf[BUFSIZ];

/*	while(--counter)
	{
		write(cfd,"hello",5);
		res=read(cfd,buf,sizeof(buf));
		write(STDOUT_FILENO,buf,res);
		sleep(1);//1秒发送一次
	}
*/
    while(1)
    {
        //从键盘读取内容
        int n=read(STDIN_FILENO,buf,sizeof(buf));
        if(n<=0)
        {
            break;
        }
        //与服务器通信
        write(cfd,buf,n);
        //接受服务器回显
        res=read(cfd,buf,sizeof(buf));
        if(res==0)
        {
            break;
        }

        //打印至屏幕
        write(STDOUT_FILENO,buf,res);
    }
	close(cfd);
	return 0;


}

