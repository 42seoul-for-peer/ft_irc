#include <iostream>
#include <algorithm>
#include "../incs/Client.hpp"

// OCCF
Client::Client() {
	std::cout << "Default constructor called for Client.\n";
}

Client::~Client() {
	std::cout << "Default destructor called for Client.\n";
}

Client::Client(const Client& copy) {
	std::cout << "Copy constructor called for Client.\n";
}

Client& Client::operator = (const Client& copy) {
	std::cout << "Copy assignment called for Client.\n";
	if (this == &copy)
		return (*this);
	return (*this);
}

// MEMBER FUNCTION
Client::Client(int clientSock, std::string username, std::string nickname)
    : _sock_fd(clientSock), _username(username), _nickname(nickname)
{

}

const int& Client::getSockFd(void) const
{
	return (_sock_fd);
}

const std::string& Client::getUsername(void) const
{
    return (_username);
}

const std::string& Client::getNickname(void) const
{
    return (_nickname);
}

void Client::setUsername(std::string& str)
{
    _username = str;
}

void Client::setNickname(std::string& str)
{
    _nickname = str;
}

void Client::joinChannel(Channel& channel)
{
	_curr_channel.push_back(channel);
}

void Client::leaveChannel(const Channel& channel)
{
	_curr_channel.erase(std::find(_curr_channel.begin(), _curr_channel.end(), channel));
}