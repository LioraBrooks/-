C++实现简易多进程、多线程并发服务器，并用select/poll/epoll函数进行了多路I/O转接，服务器功能为小写字母转大写。server端的代码有雷同，在此仅记录学习过程。

编译时，需要分别将对应的server.cpp、client.cpp和wrap.cpp同时编译，生成可执行文件。
例如：g++ process_server.cpp wrap.cpp -o server -Wall

客户端的实现相同，测试时也可以直接用nc 127.0.0.1 PORT连接上服务器。

（注：thread_server.cpp、thread_client.cpp编译时，需要加上 -pthread参数）
