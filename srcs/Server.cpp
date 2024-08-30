#include <iostream>
#include <sstream>
#include <unistd.h>
#include "Server.hpp"

// OCCF
Server::~Server() { }

Server::Server(const std::string& port, const std::string& password)
: _port(std::atoi(port.c_str())), _password(password) {
	if (_port <= 0 || _port > 65535)
		throw std::runtime_error("port number should be between 0 to 65535.");
}

// MEMBER FUNCTION
void Server::serverInit()
{
	_sock_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (_sock_fd == -1)
		throw std::runtime_error("server init error: " + std::string(std::strerror(errno)));

	memset(&_addr, 0, sizeof(_addr));
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_addr.sin_addr.s_addr = htons(_port);

	if (bind(_sock_fd, (struct sockaddr*) &_addr, sizeof(_addr)) == -1)
		throw std::runtime_error("server init error: " + std::string(std::strerror(errno)) + '.');

	if (listen(_sock_fd, 50) == -1)
		throw std::runtime_error("server init error: " + std::string(std::strerror(errno)) + '.');

	_kq = kqueue();
	if (_kq == -1)
		throw std::runtime_error("server init error: " + std::string(std::strerror(errno)) + '.');

	changeEvents(EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
}

void Server::serverProcess()
{
	int newEvent;
	while (1) {
		newEvent = checkNewEvents();
		for (int i = 0; i < newEvent; i++) {
			if (_event[i].flags & EV_ERROR) {
				if (_event[i].ident == static_cast<uintptr_t>(_sock_fd))
					throw std::runtime_error("server socket close error: " + std::string(std::strerror(errno)) + '.');
				else
					disconnectClnt(_event[i].ident);
			} else if (_event[i].filter == EVFILT_READ) {
				if (_event[i].ident == static_cast<uintptr_t>(_sock_fd)) {
					try {
						acceptClnt();
					} catch (std::exception& e) {
						std::cerr << e.what() << std::endl;
					}
				} else if (_clients.find(_event[i].ident) != _clients.end()) {
					try {
						recvMsgFromClnt(_event[i].ident);
					} catch (std::exception& e) {
						std::cerr << e.what() << std::endl;
						disconnectClnt(_event[i].ident);
					}
				}
			}
		}
		if (!_commandQueue.empty())
			sendMsgToClnt(*_commandQueue.front());
    }
}

void Server::acceptClnt()
{
	int clientSock;

	clientSock = accept(_sock_fd, NULL, NULL);
	if (clientSock == -1)
		throw std::runtime_error("accept function error: " + std::string(std::strerror(errno)) + '.'); // 디버깅할 필요 없을 땐 return으로 처리하는 게 좋을 듯
	fcntl(clientSock, F_SETFL, O_NONBLOCK);
	changeEvents(EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	_clients[clientSock] = new Client(clientSock);
}

void Server::recvMsgFromClnt(int clnt_fd)
{
	std::vector<char> rBuf(1024);
	std::string readString;
	std::string cmdToken;

	int n = recv(clnt_fd, &rBuf[0], rBuf.capacity(), 0);
	if (n > 0) {
		rBuf.erase(rBuf.begin() + n, rBuf.end());
	} else {
		if (n < 0)
			throw std::runtime_error("client error: " + std::string(std::strerror(errno)) + '.');
		disconnectClnt(clnt_fd);
	}
	readString = std::string(rBuf.begin(), rBuf.end());
	std::stringstream stream(readString);

	Command cmd(stream);
	cmd.parse(clnt_fd, *this);
	cmd.execute();

	_commandQueue.push(&cmd);
}

void Server::sendMsgToClnt(Command& cmd)
{
	std::string							sender = cmd.getSender();
	std::vector<std::string>			receiver = cmd.getReceiver();
	std::string							outBuf = cmd.getProtoMsg();
	std::vector<std::string>::iterator	receiver_it;
	std::map< int, Client* >::iterator	clnt_it;
	int									result;

	receiver_it = receiver.begin();
	while (receiver_it != receiver.end()) {
		clnt_it = _clients.begin();
		while (clnt_it != _clients.end()) {
			if (*receiver_it == clnt_it->second->getUsername())
				break;
			clnt_it++;
		}
		result = send(clnt_it->first, outBuf.c_str(), outBuf.size(), 0);
		if (result < 0)
			disconnectClnt(clnt_it->first);
		receiver_it++;
	}
	_commandQueue.pop();
}

int		Server::checkNewEvents() {
	int	newEvent = kevent(_kq, &_kq_events[0], _kq_events.size(), _event, 8, NULL);
	while (newEvent <= 0) {
		if (newEvent < 0)
			throw std::runtime_error("server socket close error: " + std::string(std::strerror(errno)) + '.');
	}
	// 수정 필요 (kqevent가 event보다 크면 이벤트가 씹힐 우려)
	_kq_events.clear();
	return (newEvent);
}

void	Server::changeEvents(int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata) {
	struct kevent tmpEvent;

    EV_SET(&tmpEvent, _sock_fd, filter, flags, fflags, data, udata);
    _kq_events.push_back(tmpEvent);
}

void	Server::disconnectClnt(int clnt_fd) {
	std::cout << "client disconnected: " << clnt_fd << std::endl;
	close(clnt_fd);
	delete _clients.find(clnt_fd)->second;
	_clients.erase(clnt_fd);
}

void	Server::addNewClnt(int clnt_fd, Client* clnt) {
	_clients.insert(std::make_pair(clnt_fd, clnt));
}

// getter
int	Server::getPort() const {
	return (_port);
}

const std::string&	Server::getPassword() const {
	return (_password);
}

const std::map< int, Client* >&	Server::getClients() const {
	return (_clients);
}

const std::map< std::string, Channel* >&	Server::getChannels() const {
	return (_channels);
}
