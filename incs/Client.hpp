#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <vector>
# include <map>
# include "Channel.hpp"

class Channel;

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
	int 		_sock_fd;
	bool		_is_registered;
	std::string _username;
	std::string _nickname;
	std::vector< std::string > _curr_channel;

// MEMBER FUNCTION
 public:
	// GETTER
	const int& 			getSockFd(void) const;
	const bool&			getIsRegistered(void) const;
	const std::string& 	getUsername(void) const;
	const std::string& 	getNickname(void) const;
	const std::vector< std::string > getCurrChannel(void) const;

	// SETTER
	void setUsername(std::string& str);
	void setNickname(std::string& str);
	void setRegistered(void);

	// HANDLING CHANNEL
	void joinChannel(const Channel& channel);
	void leaveChannel(const Channel& channel);

// EXCEPTION

};

#endif