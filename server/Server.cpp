#include <iostream>
#include <sstream>
#include "Server.hpp"

// OCCF
Server::Server() {
	// std::cout << "Default constructor called for Server.\n";
}

Server::~Server() {
	// std::cout << "Default destructor called for Server.\n";
}

Server::Server(const Server& copy) {
	// std::cout << "Copy constructor called for Server.\n";
}

Server& Server::operator = (const Server& copy) {
	// std::cout << "Copy assignment called for Server.\n";
	// if (this == &copy)
	// 	return (*this);
	// return (*this);
}

// Server::Server(const int& port, const std::string& password)
// : _port(port), _password(password)
Server::Server(const std::string& port, const std::string& password)
: _port(std::atoi(port.c_str())), _password(password) {
	if (_port <= 0 || _port > 65535)
		throw PortOutOfRangeException();
}

// MEMBER FUNCTION
void Server::serverInit()
{
	_sock_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (_sock_fd == -1)
		throw ServInitFuncException("socket");

	memset(&_serv_addr, 0, sizeof(_serv_addr));
	_serv_addr.sin_family = AF_INET;
	_serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_serv_addr.sin_addr.s_addr = htons(_port);

	if (bind(_sock_fd, (struct sockaddr*) &_serv_addr, sizeof(_serv_addr)) == -1)
		throw ServInitFuncException("bind");
	
	if (listen(_sock_fd, 50) == -1)
		throw ServInitFuncException("listen");
	
	_kq = kqueue();
	if (_kq == -1)
		throw ServInitFuncException("kqueue");
	
	changeEvents(_kq_events, _sock_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
}

void Server::serverProcess()
{
	int newEvent;
	while (1)
    {
		newEvent = checkNewEvents();
		for (int i = 0; i < newEvent; i++)
		{
			if (_event[i].flags & EV_ERROR)
			{
				if (_event[i].ident == static_cast<uintptr_t>(_sock_fd))
					; //throw(server error)
				else // (_event[i].ident == static_cast<uintptr_t>(_client_fd))
					; // discconect client
			}
			else if (_event[i].filter == EVFILT_READ)
			{
				if (_event[i].ident == static_cast<uintptr_t>(_sock_fd))
					acceptClnt();
				else if (clients.find(_event[i].ident) != clients.end())
					recvMsgFromClnt(/* 해당 클라이언트 인수로 넣기 */);
			}
			else if (_event[i].filter == EVFILT_WRITE)
				sendMsgToClnt();
		}
		// catch client socket error
    }
}

// EXCEPTION
const char*	Server::PortOutOfRangeException::what() const throw() {
	const char	*reason = "port number should be between 0 to 65535.";
	return (reason);
}

const char*	Server::ServInitFuncException::what(const std::string& func_name) const throw() {
	std::stringstream stream;
	char	*reason;

	stream << func_name << " error " << std::strerror(errno) << '.';
	stream >> reason;
	return (reason);
}
