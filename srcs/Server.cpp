#include <iostream>
#include <sstream>
#include <unistd.h>
#include "Server.hpp"

// OCCF
// Server::Server() {
	// std::cout << "Default constructor called for Server.\n";
// }

Server::~Server() {
	// std::cout << "Default destructor called for Server.\n";
}

// Server::Server(const Server& copy) {
	// std::cout << "Copy constructor called for Server.\n";
// }

// Server& Server::operator = (const Server& copy) {
// 	std::cout << "Copy assignment called for Server.\n";
// 	if (this == &copy)
// 		return (*this);
// 	return (*this);
// }

Server::Server(const std::string& port, const std::string& password)
: _port(std::atoi(port.c_str())), _password(password) {
	if (_port <= 0 || _port > 65535)
		throw PortOutOfRangeException();
}

// MEMBER FUNCTION
void Server::serverInit()
{
	_sock_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (_sock_fd == -1) {
		std::cerr << "server initiation failed (socket() error)" << std::endl;
		exit(1);
	}
		throw ServInitFuncException("socket");

	memset(&_addr, 0, sizeof(_addr));
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_addr.sin_addr.s_addr = htons(_port);

	if (bind(_sock_fd, (struct sockaddr*) &_addr, sizeof(_addr)) == -1)
		throw ServInitFuncException();

	if (listen(_sock_fd, 50) == -1)
		throw ServInitFuncException();

	_kq = kqueue();
	if (_kq == -1)
		throw ServInitFuncException();

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
					throw ServSockCloseException();
				else
					disconnectClnt(_event[i].ident);
			}
			else if (_event[i].filter == EVFILT_READ)
			{
				if (_event[i].ident == static_cast<uintptr_t>(_sock_fd)) {
					try {
						acceptClnt();
					}
					catch (ClntAcceptionFailException& e) {
						std::cerr << e.what() << std::endl;
					}
				}
				else if (_clients.find(_event[i].ident) != _clients.end()) {
					try {
						recvMsgFromClnt(_event[i].ident);
					} catch (ClntErrException& e) {
						std::cerr << e.what() << std::endl;
						disconnectClnt(_event[i].ident);
					}
				}
			}
		}
		if (!_commandQueue.empty())
			sendMsgToClnt(_commandQueue.first());
    }
}

void Server::acceptClnt()
{
	int clientSock;

	clientSock = accept(_sock_fd, NULL, NULL);
	if (clientSock == -1)
		throw ClntAcceptionFailException(); // 디버깅할 필요 없을 땐 return으로 처리하는 게 좋을 듯
	fcntl(clientSock, F_SETFL, O_NONBLOCK);
	changeEvents(_kq_events, clientSock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	_clients[clientSock] = new Client(clientSock);
}

void Server::recvMsgFromClnt(int clnt_fd)
{
	const int commandSize = _commandList.size();
	std::vector<char> rBuf(1024);
	std::string readString;
	std::string cmdToken;
	Protocol	*tmp;
	int			idx;

	int n = recv(clnt_fd, &rBuf[0], rBuf.capacity(), 0);
	if (n > 0) {
		rBuf.erase(rBuf.begin() + n, rBuf.end());
	}
	else {
		if (n < 0)
			throw ClntErrException();
		disconnectClnt(_event[i].ident);
	}
	readString = std::string(rBuf.begin(), rBuf.end());
	std::stringstream stream(readString);

	Command cmd(stream);
	cmd.parse(*this);
	cmd.execute();
	
	_commandQueue.push(cmd);
}

void Server::sendMsgToClnt(Command& cmd)
{
	std::string							sender = cmd.getSender();
	std::vector<std::string>			receiver = cmd.getReceiver();
	std::string							outBuf = receiver.getProtocol();
	std::vector<std::string>::iterator	receiver_it;
	std::map< int, Client* >::iterator	clnt_it;
	int									result;

	receiver_it = receiver.begin(); 
	while (receiver_it != receiver.end())
	{
		clnt_it = _clients.begin();
		while (clnt_it != _clients.end()) {
			if (*it == clnt_it->second().getUsername())
				break;
			clnt_it++;
		}
		result = send(clnt_it->first(), outBuf, outBuf.size(), 0);
		if (result < 0)
			disconnectClnt(clnt_it->first());
		it++;
	}
	_commandQueue.pop();
}

void	Server::disconnectClnt(int clnt_fd) {
	std::cout << "client disconnected: " << clnt_fd << std::endl;
	close(clnt_fd);
	delete _clients.find(clnt_fd)->second;
	_clients.erase(clnt_fd);
}

// EXCEPTION
const char*	Server::PortOutOfRangeException::what() const throw() {
	const char	*reason = "port number should be between 0 to 65535.";
	return (reason);
}

const char*	Server::ServInitFuncException::what() const throw() {
	std::stringstream	stream;
	char				*reason;

	stream << "server init error " << std::strerror(errno) << '.';
	stream >> reason;
	return (reason);
}

const char*	Server::ClntAcceptionFailException::what() const throw() {
	std::stringstream	stream;
	char				*reason;

	stream << "accept() error " << std::strerror(errno) << '.';
	stream >> reason;
	return (reason);
}

const char*	Server::ClntErrException::what() const throw() {
	std::stringstream	stream;
	char				*reason;

	stream << "client error " << std::strerror(errno) << '.';
	stream >> reason;
	return (reason);
}
