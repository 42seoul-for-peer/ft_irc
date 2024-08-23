#include "irc.hpp"

/*
#include "Server.hpp"
int main(int argc, char *argv[])
{
	try {
		if (argc != 3)
			throw();
		Serv serv(argv[1], argv[2]);
		serv.loopConnection();
	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl();
		return (1);
	}
}
*/

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
    int                 servSock; // v
    int                 clntSock;
    int                 kq; // v
    /*
    try {
        Server serv(argv[1], argv[2]);
        serv.serverInit();
        serv.loopConnection();
    }
    catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return (1);
    }
    return (0);
    */
//
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
//

    while (1)
    {
// check new event occurence
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
//
		    if (currEvent->flags & EV_ERROR)
                errorSequence(servSock, currEvent, clients);
//
            else if (currEvent->filter == EVFILT_READ)
                readSequence(clntSock, servSock, currEvent, kqEvents, clients);
//
            else if (currEvent->filter == EVFILT_WRITE)
                writeSequence(currEvent, clients, kqEvents);
        }
    }
    return (0);
}