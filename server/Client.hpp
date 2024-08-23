#ifndef USER_HPP
# define USER_HPP

# include <string>
# include <exception>
# include "Server.hpp"
# include "Channel.hpp"

class Client {
// OCCF
 public:
	Client();
	~Client();
 private:
	Client(const Client& copy);
	Client& operator = (const Client& copy);
// MEMBER VARIABLE
 private:
	int			_sock_fd;
	std::string	_username;
	std::string	_nickname;
	Channel*	_curr_channel;
// MEMBER FUNCITON
 public:
	
// EXCEPTION
};

#endif
