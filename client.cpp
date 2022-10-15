#include "wrappers.h"
#include <iostream>
#include <string.h>
#include <unistd.h> 
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <boost/log/trivial.hpp>

void StartClient(int const port, char const *server_ip) {
    size_t buffer_size = 512;
    /*create a socket with ipv4 and tcp*/
    BOOST_LOG_TRIVIAL(info) << "Starting client"; 
    int fd = Socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip); 
    Inet_pton(AF_INET, server_ip, &server_addr.sin_addr);
    BOOST_LOG_TRIVIAL(info) << "Socket created"; 
    /*connect to the server*/
    BOOST_LOG_TRIVIAL(info) << "Connecting to the server [PORT=" << port << " IP=" << server_ip << "]"; 
    Connect(fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    BOOST_LOG_TRIVIAL(info) << "Connected to the server"; 
    /*start communication*/
    char buf[buffer_size];
    /*client send START message to start protocol interaction. 
    Then send and get 2 messages with echo-server. Then end 
    interaction with STOP message*/
    Write(fd, "START", 6);
    Read(fd, buf, buffer_size); 
    BOOST_LOG_TRIVIAL(info) << "Server's response: " << buf; 
    do {
        /*get line from istream*/
        std::cout << "> "; 
        std::cin.getline(buf, buffer_size); 
        /*send message to the server*/
        Write(fd, buf, strlen(buf) + 1);
        if (strcmp("STOP", buf) == 0) break;
        /*get response from the server*/
        if (!Read(fd, buf, buffer_size)) break; 
        BOOST_LOG_TRIVIAL(info) << "Server's response: " << buf; 
    } while (true);
    /* interactive communication is implemented in another
    version of server-client*/
    close(fd);
    BOOST_LOG_TRIVIAL(info) << "Client is shutting down";
}

int main(int argc, char *argv[]) {
    int port = 9876;
    if (argc < 2) {
        BOOST_LOG_TRIVIAL(warning) << "The name of the environment variable storing the port is not given";
	} else {
        char *sport = getenv(argv[1]);
        if (sport == NULL) {
            BOOST_LOG_TRIVIAL(error) << "not found the environment variable " << argv[1];
            return EXIT_FAILURE;
        }
        port = atoi(sport);
    }
    char const *server_ip = "127.0.0.7"; 
    StartClient(port, server_ip);
    return EXIT_SUCCESS;
}