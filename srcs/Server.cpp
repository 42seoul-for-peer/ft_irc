#include <iostream>
#include <sstream>
#include <unistd.h>
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
	_commandList.push_back(new PRIVMSG);
	_commandList.push_back(new JOIN);
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
					throw ServSockCloseException();
				else {
					std::cout << "client disconnected: " << _event[i].ident << std::endl;
					close(_event[i].ident);
					delete _clients.find(_event[i].ident)->second;
					_clients.erase(_clients.find(_event[i].ident));
				}
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
				else if (_clients.find(_event[i].ident) != _clients.end())
					recvMsgFromClnt(_event[i].ident);
			}
		}
		if (!_commandQueue.empty())
			sendMsgToClnt();
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
			; // exception
		// disconect
	}
	readString = std::string(rBuf.begin(), rBuf.end());
	std::stringstream stream(readString);
	std::getline(stream, cmdToken, ' ');
	for (idx = 0; idx < commandSize; idx++) {
		if (_commandList[idx]->getCmd() == cmdToken)
			break ;
	}
	if (idx == commandSize)
		std::cout << "Unknown command" << std::endl; // 명령어를 찾지 못함
	else
		_commandQueue.push(_commandList[idx]->clone(stream));
}
/*
	_clients[clnt_fd]->recvToBuff();
	tmp = _clients[clnt_fd]->parse();
	if (tmp)
		_commandQueue.push(tmp);


	Command* tmp;
	_clients[clnt_fd]->recvToBuff();
	tmp = _clients[clnt_fd]->parse(); new JOIN / new PRIVMSG
	std::stringstream	stream(_buffer);
	std::string		cmd;
*/
void Server::sendMsgToClnt()
{
	int idx;
	const int commandSize = _commandList.size();
	while (_commandQueue.size())
	{
		_commandQueue.front()->execute();
		_commandQueue.pop();
	}
}

// EXCEPTION
const char*	Server::PortOutOfRangeException::what() const throw() {
	const char	*reason = "port number should be between 0 to 65535.";
	return (reason);
}

Server::ServInitFuncException::ServInitFuncException(const std::string& func_name) : _func_name(func_name) {}

const char*	Server::ServInitFuncException::what() const throw() {
	std::stringstream	stream;
	char				*reason;

	stream << _func_name << " error " << std::strerror(errno) << '.';
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