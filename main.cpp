#include <iostream>

#include <sys/socket.h>
#include <sys/event.h>  // to use kqueue
#include <arpa/inet.h>
#include <cstdlib>      // to use atoi
#include <cstring>      // to use memset
#include <vector>

void error_msg(std::string msg) {
	std::cerr << "\033[1;31m" <<msg << "\033[0m" <<std::endl;
	exit(1);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cout << "usage: ./irsserv [port] [password]" << std::endl;
        return (1);
    }
    std::vector<struct kqevent> kqEvents;
    struct kevent       event;
    struct sockaddr_in  srvAddr;
    struct sockaddr_in  clntAddr;
    int                 newEvent;
    int                 eventFd;
    int                 srvSock;
    int                 kq;

    if ((srvSock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        error_msg("socket error");
        
    memset(&srvAddr, 0, sizeof(srvAddr));
    srvAddr.sin_family = AF_INET;
    // INADDR_ANY = 0.0.0.0 = localhost = 127.0.0.1
    srvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int port = std::atoi(argv[1]);
    if (port == 0)
        error_msg("port error");
    srvAddr.sin_port = htons(port);

    if (bind(srvSock, (struct sockaddr*)&srvAddr, sizeof(srvAddr)))
        error_msg("bind error");
    
    if (listen(srvSock, 10) == -1)
		error_msg("listen error");

    // kqueue turn
    kq = kqueue();
    if (kq < 0)
        error_msg("kqueue error");
    EV_SET(&event, srvSock, EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(kq, &event, 1, NULL, 0, NULL) == -1)
        error_msg("kevent error");
    while (1)
    {
        newEvent = kevent(kq, NULL, 0, &event, 1, NULL);
        if (newEvent == -1)
            error_msg("kevent error");
        for (int i = 0; i < newEvent; i++)
        {
            eventFd = kqEvents[i].ident;
            // disconnected
            if (kqEvents[i].flags & EV_EOF)
            {
                std::cout << "Client " << eventFd << "has disconnected" << std::endl;
                close(event_fd);
            }
            // new connection
            else if (event_fd == srvSock)
        }

    }
}






