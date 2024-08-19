#include "irc.hpp"

std::ostream& operator <<(std::ostream& out, const std::vector<char>& buf)
{
    int bufLength = buf.size();

    for (int i = 0; i < bufLength; i++)
        out << buf[i];
    out << std::endl;
    return (out);
}

int loopConnection(struct pollfd clients[512], struct sockaddr_in clntAddr, int srvSock)
{
    std::vector<char> rBuf(BUF_SIZE);
    std::vector<char> wBuf(BUF_SIZE);
    socklen_t clntAddrSize;
    int byteReceived;
    int numClients = 0;
    int clntSock;
    int idx;

    while (true)
    {
        if (poll(clients, numClients + 1, 5000) <= 0)
            continue;
        
        // accept connection
        if (clients[0].revents & POLLIN)
        {
            clntAddrSize = sizeof(clntAddr);
            clntSock = accept(srvSock, \
                (struct sockaddr*)&clntAddr, &clntAddrSize);
            for (idx = 1; idx < 512; idx++)
            {
                if (clients[idx].fd < 0)
                {
                    clients[idx].fd = clntSock;
                    clients[idx].events = POLLIN;
                    break ;
                }
            }
            if (idx == 512)
            {
                std::cout << "out of maximum of clients" << std::endl;
                return (1);
            }
            if (idx > numClients)
                numClients = idx;

            continue;
        }
        // operate client's request
        for (int i = 1; i <= numClients; i++)
        {
            if (clients[i].fd < 0)
                continue;
            // detected where POLLIN event is occured in (i)
            if (clients[i].revents & POLLIN)
            {
                rBuf.erase(rBuf.begin(), rBuf.end());
                std::cout << "detected event from client (" << clients[i].fd << ")" << std::endl;
                std::cout << rBuf.size() << "," << rBuf.capacity() << std::endl;
                //rBuf.clear();

                // read socket where the event occured in
                byteReceived = recv(clients[i].fd, &rBuf[0], BUF_SIZE, 0);
                std::cout << "client(" << clients[i].fd << ", " << byteReceived << "): " << rBuf << std::endl;
                std::cout << "size: " << rBuf.size() << std::endl;
                if (byteReceived <= 0)
                {
                    close(clients[i].fd);
                    std::cout << clients[i].fd << \
                        " client disconnected" << std::endl;
                    if (i == numClients)
                        for (; clients[numClients].fd < 0; --numClients)
                            ;
                }
                else
                {
                    rBuf.erase(rBuf.begin() + byteReceived - 2, rBuf.end());
                    bufferHandling(rBuf, clients, i);
                }
            }
        }
    }
}