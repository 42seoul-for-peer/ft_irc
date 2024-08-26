#ifndef USER_HPP
# define USER_HPP

# include <string>
# include <exception>
# include "Server.hpp"
# include "Channel.hpp"
# include "Protocol.hpp"

class Client {
// OCCF
 public:
	Client(int clientSock);
	~Client();
 private:
 	Client();
	Client(const Client& copy);
	Client& operator = (const Client& copy);
// MEMBER VARIABLE
 private:
	std::string	_buffer;
	int			_sock_fd;
	std::string	_username;
	std::string	_nickname;
	Channel*	_curr_channel;
// MEMBER FUNCITON
 public:
	void		recvBuff();
	Protocol&	parse();
// EXCEPTION
};

#endif
