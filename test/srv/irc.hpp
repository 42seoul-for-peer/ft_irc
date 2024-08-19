#ifndef IRC_HPP
# define IRC_HPP

# include <iostream>
# include <sys/socket.h>
# include <poll.h>

# include <cstring> // for using memset
# include <cstdlib> // for using atoi
# include <unistd.h>
# include <arpa/inet.h>

# include <vector> // for read/write buffer
# include <algorithm> // for implementing buffer printing

# define BUF_SIZE 512

int loopConnection(struct pollfd clients[512], struct sockaddr_in clntAddr, int srvSock);
void bufferHandling(std::vector<char> rBuf, struct pollfd clients[512], int targetSock);
bool compare(const std::vector<char>& rBuf, const std::string& str);

std::ostream& operator <<(std::ostream& out, const std::vector<char>& buf);

#endif