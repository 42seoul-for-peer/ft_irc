#include "irc.hpp"

bool compare(const std::vector<char>& rBuf, const std::string& str)
{
    const int rBufLength = rBuf.size();
    const int strLength = str.length();

    if (rBufLength != strLength)
        return (false);
    for (int i = 0; i < strLength; i++)
    {
        if (rBuf[i] != str[i])
            return (false);
    }
    return (true);
}

void bufferHandling(std::vector<char> rBuf, struct pollfd clients[512], int targetSock)
{
    std::string wBuf;

    if (compare(rBuf, "CAP LS 302"))
    {
        wBuf = ":irc.local 451 * JOIN :You have not registered.\n";
        send(clients[targetSock].fd, wBuf.c_str(), wBuf.length(), 0);
    }
    else
        std::cout << "not sendtargetSockng" << std::endl;
}