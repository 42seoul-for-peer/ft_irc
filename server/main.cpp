#include "irc.hpp"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cout << "usage: ./irsserv [port] [password]" << std::endl;
        return (1);
    }
    std::vector<struct kevent> kqEvents;
    std::map< int, std::vector<char> > clients;
    struct kevent       event[8];
    struct kevent       *currEvent;
    struct sockaddr_in  srvAddr;
    int                 newEvent;
    int                 servSock;
    int                 clntSock;
    int                 kq;

    if ((servSock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        error_msg("socket error");
        
    memset(&srvAddr, 0, sizeof(srvAddr));
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int port = std::atoi(argv[1]);
    if (port == 0)
        error_msg("port error");
    srvAddr.sin_port = htons(port);

    if (bind(servSock, (struct sockaddr*)&srvAddr, sizeof(srvAddr)) == -1)
        error_msg("bind error");
    
    if (listen(servSock, 10) == -1)
		error_msg("listen error");
    fcntl(servSock, F_SETFL, O_NONBLOCK);

    if ((kq = kqueue()) == -1)
        error_msg("kqueue error");

	changeEvents(kqEvents, servSock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	std::cout << GREEN << "echo server started" << RESET << std::endl;

    while (1)
    {
        std::cout << GREEN << "\nlistening..."  << RESET << std::endl;
        newEvent = 0;
        while ((newEvent = kevent(kq, &kqEvents[0], kqEvents.size(), event, 8, NULL)) <= 0)
        {
            if (newEvent < 0)
                error_msg("kevent error");
        }
        kqEvents.clear();
        std::cout << "newEvent: " << newEvent << std::endl;
        for (int i = 0; i < newEvent; i++)
        {
            currEvent = &event[i];
            if (currEvent->flags & EV_ERROR)
                errorSequence(servSock, currEvent, clients);
            else if (currEvent->filter == EVFILT_READ)
                readSequence(clntSock, servSock, currEvent, kqEvents, clients);
            else if (currEvent->filter == EVFILT_WRITE)
                writeSequence(clntSock, currEvent, clients, kqEvents);
        }
    }
    return (0);
}