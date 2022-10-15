#include "wrappers.h"
#include <sys/socket.h>
#include <cstdlib>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <malloc.h>
#include <arpa/inet.h>
#include <boost/log/trivial.hpp>

int Socket(int domain, int type, int protocol) {
    int res = socket(domain, type, protocol);
    if (res == -1) {
        BOOST_LOG_TRIVIAL(error) << "Socket creation failed";
        exit(EXIT_FAILURE);
    }
    return res;
}

void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    int res = bind(sockfd, addr, addrlen);
    if (res == -1) {
        BOOST_LOG_TRIVIAL(error) << "Socket binding failed";
        exit(EXIT_FAILURE);
    }
}

void Listen(int sockfd, int backlog) {
    int res = listen(sockfd, backlog);
    if (res == -1) {
        BOOST_LOG_TRIVIAL(error) << "Socket listening failed";
        exit(EXIT_FAILURE);
    }
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    int res = accept(sockfd, addr, addrlen);
    if (res == -1) {
        BOOST_LOG_TRIVIAL(error) << "Accepting failed";
        exit(EXIT_FAILURE);
    }
    return res;
}

int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    int res = connect(sockfd, addr, addrlen);
    if (res == -1) {
        BOOST_LOG_TRIVIAL(error) << "Connection failed";
        exit(EXIT_FAILURE);
    }
    return res;
}

void Inet_pton(int af, const char *src, void *dst) {
    int res = inet_pton(af, src, dst);
    if (res == -1) {
        BOOST_LOG_TRIVIAL(error) << src << " does not contain a character "
                                "string representing a valid network address "
                                "in the specified address family";
        exit(EXIT_FAILURE);
    }    
    if (res == 0) {
        BOOST_LOG_TRIVIAL(error) << "Inet_pton failed";
        exit(EXIT_FAILURE);
    }
}

int Read(int fd, void *buf, size_t count) {
    /*attempts to read up to count bytes from file 
    descriptor fd into the buffer starting at buf*/
    memset(buf, 0, count);
    ssize_t res = read(fd, buf, count);
    if (res == -1) {
        BOOST_LOG_TRIVIAL(error) << "Reading failed";
        exit(EXIT_FAILURE);
    } 
    if (res == 0) {
        BOOST_LOG_TRIVIAL(warning) << "Disconnection";
        return 0;
    }
    return 1;
}

void Write(int fd, const void *buf, size_t count) {
    ssize_t res = write(fd, buf, count + 1);
    if (res == -1) {
        BOOST_LOG_TRIVIAL(error) << "Writing failed";
        exit(EXIT_FAILURE);
    }
}
