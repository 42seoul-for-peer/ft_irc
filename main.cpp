#include <iostream>
#include <unistd.h>      // to use close
#include <fcntl.h>

#include <sys/types.h>
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
#define YELLOW  "\033[1;33m"
#define BOLD    "\033[1m"
#define RESET   "\033[0m"

std::ostream& operator << (std::ostream& out, const std::vector<char>& buf)
{
    int bufLength = buf.size();

    for (int i = 0; i < bufLength; i++)
        out << buf[i];
    out << std::endl;
    return (out);
}

void error_msg(std::string msg) {
	std::cerr << BOLD <<msg << RESET <<std::endl;
	exit(1);
}

// void addEvents(std::vector<struct kevent>& kqEvents, uintptr_t servSock, int16_t filter)
// {
//     struct kevent tmpEvent;
//     EV_SET(&tmpEvent, servSock, filter, EV_ADD | EV_ENABLE, 0, 0, NULL);
//     kqEvents.push_back(tmpEvent);
// }

void    changeEvents(std::vector<struct kevent>& kqEvents, uintptr_t ident, int16_t filter, 
            uint16_t flags, uint32_t fflags, intptr_t data, void *udata) {
    struct kevent tmpEvent;

    EV_SET(&tmpEvent, ident, filter, flags, fflags, data, udata);
    kqEvents.push_back(tmpEvent);
}

void	disconnectClient(int client_fd, std::map<int, std::string >& clients) {
	std::cout << "client disconnected: " << client_fd << std::endl;
	close(client_fd);
	clients.erase(client_fd);
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
    std::map< int, std::string > clients;
    struct kevent       event[8];
    struct kevent       *currEvent;
    struct sockaddr_in  srvAddr;
    // int                 byteReceived;
    int                 newEvent;
    int                 servSock;
    int                 clntSock;
    int                 kq;

    if ((servSock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
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
    // fcntl(servSock, F_SETFL, O_NONBLOCK);

    // kqueue turn
    if ((kq = kqueue()) == -1)
        error_msg("kqueue error");

    // addEvents(kqEvents, servSock, EVFILT_READ);
	changeEvents(kqEvents, servSock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	std::cout << GREEN << "echo server started" << RESET << std::endl;

    while (1)
    {
        std::cout << GREEN << "\nlistening..."  << RESET << std::endl;
        newEvent = kevent(kq, &kqEvents[0], kqEvents.size(), event, 8, NULL);
        if (newEvent == -1)
            error_msg("kevent error");
        kqEvents.clear();
        
        std::cout << "newEvent: " << newEvent << std::endl;
        for (int i = 0; i < newEvent; i++)
        {
			std::cout << "current idx: " << i << std::endl;
            currEvent = &event[i];

			// check error event return
            if (currEvent->flags & EV_ERROR)
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
            else if (currEvent->filter == EVFILT_READ)
            {
                std::cout << YELLOW << "read sequence" << RESET << std::endl;
                // new client connection
                if (currEvent->ident == static_cast<uintptr_t>(servSock))
                {
                    std::cout << "client accepting "<< currEvent << std::endl;
                    if ((clntSock = accept(servSock, NULL, NULL)) == -1)
                        error_msg("accept error");
                    std::cout << "accept new client: " << clntSock << std::endl;
                    fcntl(clntSock, F_SETFL, O_NONBLOCK);
					changeEvents(kqEvents, clntSock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
					changeEvents(kqEvents, clntSock, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
                    clients[clntSock] = "";
                }
                //
                else if (clients.find(currEvent->ident) != clients.end())
                {
					// read from client
					char	buf[1024];
					int n = read(currEvent->ident, buf, sizeof(buf));
                    std::cout << "read from client " << clntSock << std::endl;
                    // int byteReceived = recv(currEvent->ident, &rBuf[0], rBuf.capacity(), 0);
                    if (n <= 0)
                    {
                        if (n < 0)
                            std::cout << "client read error" << std::endl;
                        disconnectClient(currEvent->ident, clients);
                    }
                    else
                    {
						buf[n] = '\0';
                        clients[currEvent->ident] += buf;
                        std::cout << "received data from " << currEvent->ident << ": " << clients[currEvent->ident] << std::endl;
                    }
                }
            }
            else if (currEvent->filter == EVFILT_WRITE)
            {
				// send data to client
                std::cout << YELLOW << "write sequence" << RESET << std::endl;
				std::map<int, std::string >::iterator it = clients.find(currEvent->ident);
                if (it != clients.end())
                {
                    if (clients[currEvent->ident] != "")
                    {
						int n;
						n = write(currEvent->ident, clients[currEvent->ident].c_str(),
									clients[currEvent->ident].size());
                        // std::string wBuf_string(clients[currEvent->ident].begin(), clients[currEvent->ident].end());
                        // if (send(currEvent->ident, wBuf_string.c_str(), wBuf_string.size(), 0)
                        if (n == -1)
                        {
                            std::cerr << "client write error" << std::endl;
                            disconnectClient(currEvent->ident, clients);
                        }
                        else
                            clients[currEvent->ident].clear();
                    }
                }
            }
			// currEvent = nullptr;
        }
    }
}