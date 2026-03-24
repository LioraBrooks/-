#ifndef _WRAP_H_
#define _WRAP_H_

#include<iostream>
#include<string.h>
#include<ctype.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<errno.h>
#include<unistd.h>
#include<pthread.h>
#include<sys/socket.h>
using namespace std;

void sys_err(const char *str);

int Socket(int domain, int type, int protocol);

int Bind(int sockfd, const struct sockaddr *addr,socklen_t addrlen);

int Listen(int sockfd, int backlog);

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

int Connect(int sockfd, const struct sockaddr *addr,socklen_t addrlen);

#endif
