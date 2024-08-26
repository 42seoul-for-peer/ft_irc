#include <iostream>
#include "Client.hpp"

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

Client::Client(int clientSock)
	: _sock_fd(clientSock)
{
	
}

void Client::recvBuff() {
	
}

Protocol&	Client::parse() {
	return new Protocol();
}
