#include <iostream>
#include <unistd.h>      // to use close

#include <sys/socket.h>
#include <sys/event.h>  // to use kqueue
#include <arpa/inet.h>
#include <cstdlib>      // to use atoi
#include <cstring>      // to use memset
#include <vector>
#include <map>

#define BUF_SIZE 512
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define RESET   "\033[0m"

std::ostream& operator <<(std::ostream& out, const std::vector<char>& buf)
{
    int bufLength = buf.size();

    for (int i = 0; i < bufLength; i++)
        out << buf[i];
    out << std::endl;
    return (out);
}

void error_msg(std::string msg) {
	std::cerr << "\033[1;31m" <<msg << "\033[0m" <<std::endl;
	exit(1);
}

void addEvents(std::vector<struct kevent>& kqEvents, uintptr_t servSock, int16_t filter)
{
    struct kevent tmpEvent;
    EV_SET(&tmpEvent, servSock, filter, EV_ADD | EV_ENABLE, 0, 0, NULL);
    kqEvents.push_back(tmpEvent);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cout << "usage: ./irsserv [port] [password]" << std::endl;
        return (1);
    }
    std::vector<struct kevent> kqEvents;
    std::vector<char>   rBuf(BUF_SIZE);
    std::map< int, std::vector<char> > clients;
    struct kevent       event;
    struct kevent       currentEvent;
    struct sockaddr_in  srvAddr;
    int                 byteReceived;
    int                 newEvent;
    int                 servSock;
    int                 clntSock;
    int                 kq;

    if ((servSock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        error_msg("socket error");
        
    memset(&srvAddr, 0, sizeof(srvAddr));
    srvAddr.sin_family = AF_INET;
    // INADDR_ANY = 0.0.0.0 = localhost = 127.0.0.1
    srvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int port = std::atoi(argv[1]);
    if (port == 0)
        error_msg("port error");
    srvAddr.sin_port = htons(port);

    if (bind(servSock, (struct sockaddr*)&srvAddr, sizeof(srvAddr)))
        error_msg("bind error");
    
    if (listen(servSock, 10) == -1)
		error_msg("listen error");

    // kqueue turn
    if ((kq = kqueue()) < 0)
        error_msg("kqueue error");
    addEvents(kqEvents, servSock, EVFILT_READ);

    while (1)
    {
        std::cout << "listening..." << std::endl;
        newEvent = kevent(kq, &kqEvents[0], kqEvents.size(), &event, 1, NULL);
        if (newEvent == -1)
            error_msg("kevent error");
        std::cout << "newEvent: " << newEvent << std::endl;
        for (int i = 0; i < newEvent; i++)
        {
            currentEvent = kqEvents[i];
            if (currentEvent.flags & EV_ERROR)
            {
                std::cout << "error sequence" << std::endl;
                if (currentEvent.ident == static_cast<uintptr_t>(servSock))
                    error_msg("server socket error");
                else
                {
                    std::cout << "client socket error" << std::endl;
                    close(currentEvent.ident);
                    clients.erase(clntSock);
                }
            }
            else if (currentEvent.filter == EVFILT_READ)
            {
                std::cout << GREEN "\nread sequence" RESET << std::endl;
                // new client connection
                if (currentEvent.ident == static_cast<uintptr_t>(servSock))
                {
                    if ((clntSock = accept(servSock, NULL, NULL)) == -1)
                        error_msg("accept error");
                    std::cout << "accept new client: " << clntSock << std::endl;
                    addEvents(kqEvents, clntSock, EVFILT_READ);
                    addEvents(kqEvents, clntSock, EVFILT_WRITE);
                    clients[clntSock] = std::vector<char>(0);
                }
                //
                else if (clients.find(currentEvent.ident) != clients.end())
                {
                    std::cout << "clients finding" << std::endl;
                    byteReceived = recv(currentEvent.ident, &rBuf[0], rBuf.capacity(), 0);
                    if (byteReceived <= 0)
                    {
                        if (byteReceived < 0)
                            std::cout << "client read error" << std::endl;
                        close(currentEvent.ident);
                        clients.erase(clntSock);
                    }
                    else
                    {
                        clients[currentEvent.ident] = rBuf;
                        std::cout << "received data from" << currentEvent.ident << ": " << clients[currentEvent.ident] << std::endl;
                    }
                }
            }
            else if (currentEvent.filter == EVFILT_WRITE)
            {
                std::cout << GREEN "\nwrite sequence" RESET << std::endl;
                if (clients.find(currentEvent.ident) != clients.find(currentEvent.ident))
                {
                    if (clients[currentEvent.ident][0] != 0)
                    {
                        
                        std::string wBuf_string(clients[currentEvent.ident].begin(), clients[currentEvent.ident].end());
                        if (send(currentEvent.ident, wBuf_string.c_str(), wBuf_string.size(), 0) == -1)
                        {
                            std::cout << "client write error" << std::endl;
                            close(currentEvent.ident);
                            clients.erase(clntSock);
                        }
                        else
                            clients[currentEvent.ident].clear();
                    }
                }
            }
        }
    }
}