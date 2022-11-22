#pragma once
#include <sys/socket.h>


int Socket(int domain, int type, int protocol); 

void Bind(int sockfd, struct sockaddr* addr, socklen_t addrlen);

void Listen(int sockfd, int backlog); 

int Accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen); 

int Connect(int sockfd, struct sockaddr* addr, socklen_t addrlen); 

void Inet_pton(int af, const char* src, void *dst); 

int Read(int fd, void* buf, size_t count);

void Write(int fd, const void* buf, size_t count);
