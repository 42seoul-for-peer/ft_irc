#include "irc.hpp"

void errorSequence(int servSock, struct kevent *currEvent, \
                    std::map< int, std::vector<char> >& clients)
{
    std::cout << RED << "error sequence" << RESET << std::endl;
    if (currEvent->ident == static_cast<uintptr_t>(servSock))
        error_msg("server socket error");
    else
    {
        std::cerr << "client socket error" << std::endl;
        disconnectClient(currEvent->ident, clients);
    }
}

void readSequence(int& clntSock, int servSock, struct kevent *currEvent, \
                    std::vector<struct kevent>& kqEvents, std::map< int, std::vector<char> >& clients)
{
    std::vector<char> rBuf(BUF_SIZE);

    std::cout << YELLOW << "read sequence" << RESET << std::endl;
    if (currEvent->ident == static_cast<uintptr_t>(servSock))
    {
        std::cout << "client accepting "<< currEvent << std::endl;
        if ((clntSock = accept(servSock, NULL, NULL)) == -1)
            error_msg("accept error");
        std::cout << "accept new client: " << clntSock << std::endl;
        fcntl(clntSock, F_SETFL, O_NONBLOCK);
        changeEvents(kqEvents, clntSock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
        clients[clntSock] = std::vector<char>(1,'\0');
    }
    else if (clients.find(currEvent->ident) != clients.end())
    {
        int n = recv(currEvent->ident, &rBuf[0], rBuf.capacity(), 0);
        std::cout << "read from client " << clntSock << std::endl;
        if (n <= 0)
        {
            if (n < 0)
                std::cout << "client read error" << std::endl;
            disconnectClient(currEvent->ident, clients);
        }
        else
        {
            rBuf.erase(rBuf.begin() + n, rBuf.end());
            clients[currEvent->ident] = rBuf;
            std::cout << "(" << currEvent->ident << "): '" << clients[currEvent->ident] << "'(" << n << "bytes)" << std::endl;
            changeEvents(kqEvents, currEvent->ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
        }
    }
}

void writeSequence(struct kevent *currEvent, \
                    std::map< int, std::vector<char> >& clients, std::vector<struct kevent>& kqEvents)
{
    std::cout << YELLOW << "write sequence" << RESET << std::endl;

    std::map<int, std::vector<char> >::iterator it = clients.find(currEvent->ident);
    if (it != clients.end())
    {
        if (clients[currEvent->ident][0] != '\0')
        {
            std::string wBuf_string(clients[currEvent->ident].begin(), clients[currEvent->ident].end());
            if (send(currEvent->ident, wBuf_string.c_str(), wBuf_string.size(), 0) == -1)
            {
                std::cerr << "client write error" << std::endl;
                disconnectClient(currEvent->ident, clients);
            }
            else
            {
                std::cout << "send " << wBuf_string.size() << "bytes" << std::endl;
                clients[currEvent->ident].clear();
            }
            changeEvents(kqEvents, currEvent->ident, EVFILT_WRITE, EV_CLEAR | EV_DISABLE, 0, 0, NULL);
        }
    }
}