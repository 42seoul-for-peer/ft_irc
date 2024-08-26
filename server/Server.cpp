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
				else {// (_event[i].ident == static_cast<uintptr_t>(_client_fd))
					std::cout << "client disconnected: " << _event[i].ident << std::endl;
					close(_event[i].ident);
					delete _clients.find(_event[i].ident)->second;
					_clients.erase(_clients.find(_event[i].ident));
				}
			}
			else if (_event[i].filter == EVFILT_READ)
			{
				if (_event[i].ident == static_cast<uintptr_t>(_sock_fd))
					acceptClnt();
				else if (_clients.find(_event[i].ident) != _clients.end())
					recvMsgFromClnt(_clients.find(_event[i].ident)->second);
			}
		}
		if (!_protocols.empty())
			sendMsgToClnt();
    }
}

void Server::acceptClnt()
{
	int clientSock;

	clientSock = accept(_sock_fd, NULL, NULL);
	if (clientSock == -1)
		; // server error exception
	fcntl(clientSock, F_SETFL, O_NONBLOCK);
	changeEvents(_kq_events, clientSock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	_clients[clientSock] = new Client(clientSock);
}

void Server::recvMsgFromClnt(Client* client)
{
	client->recvBuff();
	_protocols.push(client->parse());
	//clnt의 recv함수를 호출하냐 -> 그 함수가 버퍼에다 채우고, 파싱하고, 필요한 요청을 처리를 하겠죠
	//recv -> clnt의 buffer를 수정하느냐
}

void Server::sendMsgToClnt()
{
	int idx;
	const int commandSize = _commandList.size();
	while (_protocols.size())
	{
		for (idx = 0; idx < commandSize; idx++) {
			if (_commandList[idx]->getCmd() == _protocols.front().getCmd())
				break ;
		}
		if (idx == commandSize)
			; // 명령어를 찾지 못함
		else
			_commandList[idx]->execute(_protocols.front());
		_protocols.pop();
	}
}

// EXCEPTION
const char*	Server::PortOutOfRangeException::what() const throw() {
	const char	*reason = "port number should be between 0 to 65535.";
	return (reason);
}

Server::ServInitFuncException::ServInitFuncException(const std::string& func_name) : _func_name(func_name) {}

const char*	Server::ServInitFuncException::what() const throw() {
	std::stringstream stream;
	char	*reason;

	stream << _func_name << " error " << std::strerror(errno) << '.';
	stream >> reason;
	return (reason);
}
