#include <iostream>
#include <algorithm>
#include "../incs/Client.hpp"

// OCCF
Client::~Client() {
	// std::cout << "Default destructor called for Client.\n";
}

// MEMBER FUNCTION
Client::Client(int clientSock)
: _sock_fd(clientSock), _is_registered(false), _connected(false),
_username("*"), _nickname("*") {}

int   Client::getSockFd(void) const {
	return (_sock_fd);
}

bool	Client::getIsRegistered(void) const {
    return (_is_registered);
}

bool	Client::getConnected(void) const {
    return (_connected);
}

const std::vector< std::string >&   Client::getCurrChannel(void) const {
    return (_curr_channel);
}

const std::string&	Client::getUsername(void) const {
    return (_username);
}

const std::string&	Client::getNickname(void) const {
    return (_nickname);
}

void	Client::setIsRegistered(void) {
    _is_registered = true;
}

void    Client::setConnected(bool flag) {
    _connected = flag;
}

void	Client::setUsername(std::string& str) {
    _username = str;
}

void	Client::setNickname(std::string& str) {
    _nickname = str;
}

void	Client::joinChannel(const Channel& channel) {
	_curr_channel.push_back(channel.getTitle());
}

void	Client::leaveChannel(const Channel& channel) {
	_curr_channel.erase(std::find(_curr_channel.begin(), _curr_channel.end(), channel.getTitle()));
}