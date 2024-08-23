#include "irc.hpp"

std::ostream& operator << (std::ostream& out, const std::vector<char>& buf)
{
    int bufLength = buf.size();

    for (int i = 0; i < bufLength; i++)
        out << buf[i];
    out << std::endl;
    return (out);
}

void    changeEvents(std::vector<struct kevent>& kqEvents, uintptr_t ident, int16_t filter, 
            uint16_t flags, uint32_t fflags, intptr_t data, void *udata) {
    struct kevent tmpEvent;

    EV_SET(&tmpEvent, ident, filter, flags, fflags, data, udata);
    kqEvents.push_back(tmpEvent);
}
void	disconnectClient(int client_fd, std::map<int, std::vector<char> >& clients) {
	std::cout << "client disconnected: " << client_fd << std::endl;
	close(client_fd);
	clients.erase(client_fd);
}

void error_msg(std::string msg) {
	std::cerr << BOLD <<msg << RESET <<std::endl;
	exit(1);
}