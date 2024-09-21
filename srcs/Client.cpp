#include <iostream>
#include <algorithm>
#include "../incs/Client.hpp"

// OCCF
Client::~Client()	{
	// std::cout << "Default destructor called for Client.\n";
}

// MEMBER FUNCTION
Client::Client(int clientSock)
: _sock_fd(clientSock), _pass_validity(-1), _registered(false), _connected(true),
_username("*"), _nickname("*") {}

const std::string&	Client::getAddr(void) const {
	return (_addr);
}

int	Client::getSockFd(void) const {
	return (_sock_fd);
}

int	Client::getPassValidity(void) const {
	return (_pass_validity);
}

bool	Client::getConnected(void) const {
	return (_connected);
}

bool	Client::getRegistered(void) const {
	return (_registered);
}

const std::vector< std::string >&	Client::getCurrChannel(void) const {
	return (_curr_channel);
}

const std::string&	Client::getUsername(void) const {
	return (_username);
}

const std::string&	Client::getNickname(void) const {
	return (_nickname);
}

void	Client::setAddr(std::string str) {
	_addr = str;
}

void	Client::setPassValidity(int flag) {
	_pass_validity = flag;
}

void	Client::setConnected(bool flag) {
	_connected = flag;
}

void	Client::setRegistered(void) {
	_registered = true;
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