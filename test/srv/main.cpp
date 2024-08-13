#include <iostream>
#include <sys/socket.h>
#include <poll.h>

#include <cstring> // for using memset
#include <cstdlib> // for using atoi
#include <unistd.h>
#include <arpa/inet.h>

#define BUF_SIZE 512

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cout << "usage: ./ircserv <port> <password>" << std::endl;
        return (1);
    }
    // what is difference betweent sockaddr and sockaddr_in?
    struct sockaddr_in srvAddr;
    struct sockaddr_in clntAddr;
    struct pollfd clients[512];
    socklen_t clntAddrSize;
    int srvSock;
    int clntSock;

    // create socket
    srvSock = socket(AF_INET, SOCK_STREAM, 0);
    if (srvSock == -1)
    {
        std::cout << "socket error" << std::endl;
        return (1);
    }

    // set socket option
    // what is htonl(), htons() ?
    memset(&srvAddr, 0, sizeof(srvAddr));
    srvAddr.sin_family = AF_INET;
    // has to be changed to argv[2]
    srvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    int port = std::atoi(argv[1]);
    std::cout << "port: " << port << std::endl;
    srvAddr.sin_port = htons(port);

    // bind socket
    if (bind(srvSock, (struct sockaddr*)&srvAddr, sizeof(srvAddr)))
    {
        std::cout << "bind error" << std::endl;
        return (1);
    }

    // wait conntection
    if (listen(srvSock, 5) == -1)
    {
        std::cout << "listen error" << std::endl;
        return (1);
    }

    // initialize pollfd array
    for (int i = 0; i < 512; i++)
        clients[i].fd = -1;

    clients[0].fd = srvSock;
    clients[0].events = POLLIN;

    int max_i = 0; // the highest index of pollfd registred
    int i;
    while (true)
    {
        if (poll(clients, max_i + 1, 5000) <= 0)
            continue;
        
        // accept connection
        if (clients[0].revents & POLLIN)
        {
            clntAddrSize = sizeof(clntAddr);
            clntSock = accept(srvSock, \
                (struct sockaddr*)&clntAddr, &clntAddrSize);
            for (i = 1; i < 512; i++)
            {
                if (clients[i].fd < 0)
                {
                    clients[i].fd = clntSock;
                    clients[i].events = POLLIN;
                    break ;
                }
            }
            if (i == 512)
            {
                std::cout << "out of maximum of clients" << std::endl;
                return (1);
            }
            if (i > max_i)
                max_i = i;
            std::cout << "(" << clntSock << ") connected" << std::endl;
            continue;
        }

        // operate client's request
        for (i = 1; i <= max_i; i++)
        {
            if (clients[i].fd < 0)
                continue;
            // detected where POLLIN event is occured in (i)
            if (clients[i].revents & POLLIN)
            {
                std::cout << "detected event from client (" << clients[i].fd << ")" << std::endl;
                int byteReceived;
                char buf[BUF_SIZE];
                memset(buf, 0, BUF_SIZE);

                // read socket where the event occured in
                byteReceived = recv(clients[i].fd, buf, BUF_SIZE, 0);
                std::cout << clients[i].fd << ": " << buf << std::endl << std::endl;
                if (byteReceived <= 0)
                {
                    close(clients[i].fd);
                    std::cout << clients[i].fd << \
                        " client disconnected" << std::endl;
                    if (i == max_i)
                        for (; clients[max_i].fd < 0; --max_i)
                            ;
                }
                else
                {
                    for (int j = 0; j <= max_i; j++)
                        send(clients[j].fd, buf, byteReceived, 0);
                }
            }
        }
    }
    return (0);
}