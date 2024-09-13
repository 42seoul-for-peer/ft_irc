#include <iostream>
#include <sstream>
#include <iomanip>
#include <unistd.h>
#include "Server.hpp"

// OCCF
Server::~Server() {} // 추후 확인 필요

Server::Server(const std::string& port, const std::string& password)
: _port(std::atoi(port.c_str())), _password(password), _serv_name("irc.local") {
	if (_port <= 0 || _port > 65535)
		throw std::runtime_error("port number should be between 0 to 65535.");
}

// MEMBER FUNCTION
void Server::serverInit()
{
	_sock_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (_sock_fd == -1)
		throw std::runtime_error("socket error: " + std::string(std::strerror(errno)));

	memset(&_addr, 0, sizeof(_addr));
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_addr.sin_port = htons(_port);

	if (bind(_sock_fd, (struct sockaddr*) &_addr, sizeof(_addr)) == -1)
		throw std::runtime_error("bind error: " + std::string(std::strerror(errno)) + '.');

	if (listen(_sock_fd, 50) == -1)
		throw std::runtime_error("listen error: " + std::string(std::strerror(errno)) + '.');

	_kq = kqueue();
	if (_kq == -1)
		throw std::runtime_error("kqueue error: " + std::string(std::strerror(errno)) + '.');

	changeEvents(_sock_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	std::cout << "server activated with port " << ntohs(_addr.sin_port) << std::endl;
}

void Server::serverProcess()
{
	int newEvent;
	while (1) {
		// // std::cout << "while start" << std::endl;
		newEvent = checkNewEvents();
		// // std::cout << "event count: " << newEvent << std::endl;
		for (int i = 0; i < newEvent; i++) {
			if (_event[i].flags & EV_EOF) {
				disconnectClnt(_event[i].ident);
			} else if (_event[i].flags & EV_ERROR) {
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
					// // std::cout << "read event check" << std::endl;
					try {
						recvMsgFromClnt(_event[i].ident);
					} catch (std::exception& e) {
						std::cerr << e.what() << std::endl;
						disconnectClnt(_event[i].ident);
					}
				}
			}
			// // std::cout << "for end" << std::endl;
		}
		while (!_commandQueue.empty())
			sendMsgToClnt(*_commandQueue.front());
    }
	// std::cout << "out of while" << std::endl;
}

void Server::acceptClnt()
{
	int clientSock;

	clientSock = accept(_sock_fd, NULL, NULL);
	if (clientSock == -1)
		throw std::runtime_error("accept function error: " + std::string(std::strerror(errno)) + '.'); // 디버깅할 필요 없을 땐 return으로 처리하는 게 좋을 듯
	fcntl(clientSock, F_SETFL, O_NONBLOCK);
	changeEvents(clientSock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	_clients[clientSock] = new Client(clientSock);
	std::cout << "Accepted Client: " << clientSock << std::endl;
}

void Server::recvMsgFromClnt(int clnt_fd)
{
	std::vector<char> rBuf(1024);
	std::string cmdToken;

	std::map< int, std::string >::iterator	it = _readString.find(clnt_fd);
	if (it == _readString.end()) {
		_readString.insert(std::make_pair(clnt_fd, ""));
		it = _readString.find(clnt_fd);
	}

	// std::cout << "recv activated" << std::endl;
	int n = recv(clnt_fd, &rBuf[0], rBuf.capacity(), 0);
	std::cout << "recv num: " << n << std::endl;
	if (n > 0) {
		it->second += std::string(rBuf.begin(), rBuf.begin() + n);
		if (it->second.find('\n') == std::string::npos && it->second.find('\r') == std::string::npos )
			return ;
	} else if (n < 0) {
		throw std::runtime_error("client error: " + std::string(std::strerror(errno)) + '.');
	} else {
		if (it->second == "") {
			std::cout << "\tRead nothing from client." << std::endl;
			disconnectClnt(clnt_fd);
		}
		return ;
	}

	std::cout << "Received msg: \n\t" << it->second << std::endl;
	std::size_t carriage = it->second.find('\r');
	while (carriage != std::string::npos) {
		it->second.erase(carriage, 1);
		carriage = it->second.find('\r');
	}

	std::stringstream stream(it->second);
	it->second = "";
	changeEvents(clnt_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);

	std::vector<std::string> tokens;
    std::string token;
    while (std::getline(stream, token, '\n')) {
        tokens.push_back(token);
    }

	std::vector<std::string>::iterator token_it = tokens.begin();
	while (token_it != tokens.end()) {
		std::stringstream tmpstream(*token_it);

		Command* cmd = new Command(tmpstream);
		cmd->parse(clnt_fd, *this);
		cmd->execute();

		_commandQueue.push(cmd);
		token_it++;
	}
}

void Server::sendMsgToClnt(Command& cmd)
{
	const std::map< std::string, std::string >	msgs = cmd.getMsgs();

	std::map< std::string, std::string >::const_iterator	msgs_it;
	std::map< int, Client* >::iterator				clnt_it;

	msgs_it = msgs.begin();
	while (msgs_it != msgs.end()) { //전체 리시버 돌면서 하나씩 보내기

		std::vector< std::string >	channel_receiver;
		std::vector< std::string >::iterator	channel_receiver_it;
		std::vector< std::pair< bool, Client* > >::const_iterator	channel_member_it;

		if (msgs_it->first[0] == '#') {  //채널일경우
			std::map< std::string, Channel* >::iterator it = _channels.find(msgs_it->first);
			// if (it != _channels.end())
			channel_member_it = it->second->getClients().begin();
			while (channel_member_it != it->second->getClients().end()) {
				std::string nickname = channel_member_it->second->getNickname();
				channel_receiver.push_back(nickname);
				channel_member_it++;
			}

			channel_receiver_it = channel_receiver.begin();
			while (channel_receiver_it != channel_receiver.end()) {
				sendMsgModule(*channel_receiver_it, msgs_it->second);
				channel_receiver_it++;
			}
		} else { //일반 유저일 경우
			sendMsgModule(msgs_it->first, msgs_it->second);
		}
		msgs_it++;
	}
	delete _commandQueue.front();
	_commandQueue.pop();
}

void	Server::sendMsgModule(const std::string& recv, const std::string& msg) {
	int			dest_fd;
	int			result;

	dest_fd = getClient(recv);
	if (dest_fd != 0) {
		result = send(dest_fd, msg.c_str(), msg.size(), 0);
		if (result < 0) {
			std::cout << __func__ << ": SEND err. disconnect." << std::endl;
			disconnectClnt(dest_fd);
		} else {
			std::cout << ">> Send msg to " << recv << ":\n\t" << msg << "\n\n";
		}
	}

	Client* clnt = getClient(dest_fd);
	if (!clnt->getConnected())
		disconnectClnt(dest_fd);
}

std::string Server::genPrefix(const std::string& sender, int rpl) {
	std::string outBuf = ":";
	std::stringstream ss;
	ss << std::setw(3) << std::setfill('0') << rpl;

	if (rpl != 0) {
		outBuf += _serv_name + " " + ss.str() + " " + sender + " ";
		return outBuf;
	} else
		outBuf += sender + "!" + getClient(getClient(sender))->getUsername() + "@localhost"+ " ";
	return outBuf;
}

int		Server::checkNewEvents() {
	int newEvent = 0;
	// std::cout << "newEvent while start" << std::endl;
	while (newEvent <= 0) {
		newEvent = kevent(_kq, &_kq_events[0], _kq_events.size(), _event, 8, NULL);
		if (newEvent < 0)
			throw std::runtime_error("server socket close error: " + std::string(std::strerror(errno)) + '.');
	}
	// std::cout << "newEvent while end" << std::endl;
	// 수정 필요 (kqevent가 event보다 크면 이벤트가 씹힐 우려)
	_kq_events.clear();
	return (newEvent);
}

void	Server::changeEvents(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata) {
	struct kevent tmpEvent;

    EV_SET(&tmpEvent, ident, filter, flags, fflags, data, udata);
    _kq_events.push_back(tmpEvent);
}

void	Server::disconnectClnt(int clnt_fd) {
	std::cout << "call disconnection" << std::endl;
	if (clnt_fd > 0) {
		std::cout << "client disconnected: " << clnt_fd << std::endl;
		close(clnt_fd);
		// delete _clients.find(clnt_fd)->second;
		_clients.erase(clnt_fd);
		_readString.erase(clnt_fd);
	}
}

void	Server::addNewClnt(int clnt_fd, Client* clnt) {
	_clients.insert(std::make_pair(clnt_fd, clnt));
}

void	Server::deleteClnt(int clnt_fd) {
	delete _clients[clnt_fd];
	_clients.erase(clnt_fd);
}

void	Server::addNewChnl(Channel* chnl) {
	_channels.insert(std::make_pair(chnl->getTitle(), chnl));
}

void	Server::deleteChnl(Channel* chnl) {
	std::string chnl_name = chnl->getTitle();
	delete chnl;
	_channels.erase(_channels.find(chnl_name));
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

Client* Server::getClient(int clnt_fd) const {
	std::map< int, Client* >::const_iterator it = _clients.find(clnt_fd);
	if (it == _clients.end())
		return (NULL);
	return (it->second);
}

int Server::getClient(std::string nickname) const {
	std::map< int, Client* >::const_iterator it = _clients.begin();
	while (it != _clients.end()) {
		if (it->second->getNickname() == nickname)
			break;
		it++;
	}
	if (it == _clients.end())
		return (0);
	return (it->first);
}

const std::map< std::string, Channel* >&	Server::getChannels() const {
	return (_channels);
}

const std::string& Server::getServName() const {
	return (_serv_name);
}
