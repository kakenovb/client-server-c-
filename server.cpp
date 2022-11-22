#include <arpa/inet.h>
#include <boost/log/trivial.hpp>
#include <cstdlib>
#include <iostream>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <thread> 
#include <unistd.h>
#include <vector>

#include "wrappers.h"

void StartCommunication(int client_fd, char *buf, size_t buffer_size, sockaddr_in client_addr) {
    while (true) {
        if (!Read(client_fd, buf, buffer_size)) break; 
        if (strcmp("STOP", buf) == 0) {
            BOOST_LOG_TRIVIAL(info) << "Client on [PORT=" << ntohs(client_addr.sin_port) << "] " << " stopped communication";  
            break;
        }
        BOOST_LOG_TRIVIAL(info) << "Client on [PORT=" << ntohs(client_addr.sin_port) << "]: "  << buf;  
        Write(client_fd, buf, strlen(buf) + 1);
    }
}

void StartLogging(int client_fd, char *buf, size_t buffer_size, sockaddr_in client_addr) {
    while (true) {
        if (!Read(client_fd, buf, buffer_size)) break; 
        if (strcmp("STOP", buf) == 0) {
            BOOST_LOG_TRIVIAL(info) << "[Violation of protocol] Client stopped communication";  
            break;
        }
        BOOST_LOG_TRIVIAL(info) << "[Violation of protocol] Client's message: " << buf;  
        Write(client_fd, "", 1);
    }
}

void HandleClient(int client_fd, sockaddr_in client_addr) {
    size_t buffer_size = 512; 
    char buf[buffer_size];
    Read(client_fd, buf, buffer_size); 
    if (strcmp("START", buf) == 0) {
        BOOST_LOG_TRIVIAL(info) << "Start communication with the client [PORT=" << ntohs(client_addr.sin_port) << "]";  
        Write(client_fd, "Start communication", 20);
        StartCommunication(client_fd, buf, buffer_size, client_addr);
    } else {
        BOOST_LOG_TRIVIAL(warning) << "Interaction protocol violation occurred with the client [PORT="
                                    << ntohs(client_addr.sin_port) << "] Client's message: " << buf;  
        Write(client_fd, "Interaction protocol violation occurred", 40);
        StartLogging(client_fd, buf, buffer_size, client_addr);
    } 
    BOOST_LOG_TRIVIAL(info) << "Server finished working with the client [PORT=" << ntohs(client_addr.sin_port) << "]";  
    close(client_fd);
}

void StartServer(unsigned short port, const char* server_ip) {
    size_t queue_len = 5; 
    BOOST_LOG_TRIVIAL(info) << "Starting server [PORT=" << port << " IP=" << server_ip << "]"; 
    /*create a listening socket*/
    int server_fd = Socket(AF_INET, SOCK_STREAM, 0);
    int socket_option = 1;
    setsockopt(server_fd, SOL_SOCKET , SO_REUSEADDR , &socket_option , sizeof(socket_option));
    BOOST_LOG_TRIVIAL(info) << "Listening socket created"; 
    /*bind created socket to the given ip and port*/
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip); 
    Bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    BOOST_LOG_TRIVIAL(info) << "Socket and address are binded"; 
    /*mark the socket as a listening socket that will be used 
    to accept incoming connection requests*/
    BOOST_LOG_TRIVIAL(info) << "Socket starts listening";
    Listen(server_fd, queue_len);
    std::vector<std::thread> clients;
    sockaddr_in client_addr;
    while (true) {
        socklen_t addr_len = sizeof(client_addr);
        /*wait for a connection and accept it*/
        int client_fd = Accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
        clients.emplace_back([client_fd, client_addr]() {
            HandleClient(client_fd, client_addr);
        });
        BOOST_LOG_TRIVIAL(info) << "Client " << ntohs(client_addr.sin_port) << " connected"; 
    }
    for (auto &&c: clients) {
        c.join();
    }
    BOOST_LOG_TRIVIAL(info) << "Server is shutting down";
    close(server_fd);
}

int main() {
    unsigned short port = 9876;
    const char* server_ip = "127.0.0.7"; 
    StartServer(port, server_ip);
    return EXIT_SUCCESS;
}
