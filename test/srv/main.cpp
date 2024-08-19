#include "irc.hpp"

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
    int srvSock;

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

    if (!loopConnection(clients, clntAddr, srvSock))
        return (1);
    return (0);
}