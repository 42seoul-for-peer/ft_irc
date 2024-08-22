#include <iostream>
#include <poll.h>

#include <ctime>
#include <cstring>
#include <cstdlib>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <algorithm>


int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cout << "usage: ./clint <ip> <port>" << std::endl;
        return (1);
    }

    struct sockaddr_in addrServ;

    std::memset(&addrServ, 0, sizeof(addrServ));
    addrServ.sin_family = AF_INET; // IPv4
    addrServ.sin_addr.s_addr = inet_addr(argv[1]); // ip address
    int port = std::atoi(argv[2]);
    addrServ.sin_port = htons(port);

    int sockClnt = socket(AF_INET, SOCK_STREAM, 0);
    if (sockClnt == -1)
    {
        std::cout << "socket error" << std::endl;
        return (1);
    }

    if (connect(sockClnt, (struct sockaddr*)&addrServ, \
        sizeof(addrServ)) == -1)
    {
        std::cout << "connect error" << std::endl;
        return (1);
    }
    std::string wBuf;
    std::string rBuf(512, 0);
    struct pollfd pollfds[1];
    pollfds[0].fd = sockClnt;
    pollfds[0].events = POLLIN;
    while (true)
    {
        std::cout << "\ntype here: ";
        std::getline(std::cin, wBuf);
        if (wBuf.length() > 511)
            break ;
        int wBytes = send(sockClnt, wBuf.c_str(), wBuf.length(), 0);
        std::cout << "send: " << wBytes << "bytes" << std::endl;
        if (wBytes == -1)
        {
            std::cout << "write error" << std::endl;
            break ;
        }
        std::fill(wBuf.begin(), wBuf.end(), 0);
        std::fill(rBuf.begin(), rBuf.end(), 0);
        // rBuf.clear();
        int rBytes = recv(sockClnt, &rBuf[0], 512, 0);
        if (rBytes == -1)
        {
            std::cout << "read error" << std::endl;
            break ;
        }
        else
            std::cout << "rBuf: " << rBuf << "(" << rBuf.length() << "bytes)" << std::endl;
    }
    close(sockClnt);
    return (0);
}