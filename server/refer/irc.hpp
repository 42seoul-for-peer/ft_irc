#ifndef IRC_HPP
# define IRC_HPP

# include <iostream>
# include <unistd.h>      // to use close
# include <fcntl.h>

# include <sys/types.h>
# include <sys/socket.h>
# include <sys/event.h>  // to use kqueue
# include <arpa/inet.h>
# include <cstdlib>      // to use atoi
# include <cstring>      // to use memset
# include <algorithm>
# include <vector>
# include <map>

# define BUF_SIZE 512
# define RED     "\033[1;31m"
# define GREEN   "\033[1;32m"
# define YELLOW  "\033[1;33m"
# define BOLD    "\033[1m"
# define RESET   "\033[0m"

void errorSequence(int servSock, struct kevent *currEvent, std::map< int, std::vector<char> >& clients);
void readSequence(int& clntSock, int servSock, struct kevent *currEvent, \
                    std::vector<struct kevent>& kqEvents, std::map< int, std::vector<char> >& clients);
void writeSequence(struct kevent *currEvent, \
                    std::map< int, std::vector<char> >& clients, std::vector<struct kevent>& kqEvents);

std::ostream&   operator << (std::ostream& out, const std::vector<char>& buf);
void            changeEvents(std::vector<struct kevent>& kqEvents, uintptr_t ident, int16_t filter, 
                                uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
void	        disconnectClient(int client_fd, std::map<int, std::vector<char> >& clients);
void            error_msg(std::string msg);
#endif