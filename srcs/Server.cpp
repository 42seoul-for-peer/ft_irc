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
void Server::serverInit() {
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

	_changeEvents(_sock_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	std::cout << "server activated with port " << ntohs(_addr.sin_port) << std::endl;
}

void Server::serverProcess() {
	int new_event;
	while (1) {
		// system("leaks -q ircserv");
		// // std::cout << "while start" << std::endl;
		new_event = _checkNewEvents();
		// // std::cout << "event count: " << new_event << std::endl;
		for (int i = 0; i < new_event; i++) {
			if (_event[i].flags & EV_EOF) {
				std::cout << "EOF detected" << std::endl;
				_disconnectClnt(_event[i].ident);
			}
			else if (_event[i].flags & EV_ERROR) {
				if (_event[i].ident == static_cast<uintptr_t>(_sock_fd))
					throw std::runtime_error("server socket close error: " + std::string(std::strerror(errno)) + '.');
				else
					_disconnectClnt(_event[i].ident);
			}
			else if (_event[i].filter == EVFILT_READ) {
				if (_event[i].ident == static_cast<uintptr_t>(_sock_fd)) {
					try {
						_acceptClnt();
					}
					catch (std::exception& e) {
						std::cerr << e.what() << std::endl;
					}
				}
				else if (_clients.find(_event[i].ident) != _clients.end()) {
					// // std::cout << "read event check" << std::endl;
					try {
						_recvMsgFromClnt(_event[i].ident);
					}
					catch (std::exception& e) {
						std::cerr << e.what() << std::endl;
						_disconnectClnt(_event[i].ident);
					}
				}
			}
			// // std::cout << "for end" << std::endl;
			while (!_command_queue.empty())
				_sendMsgToClnt(_event[i].ident, *_command_queue.front());
		}
    }
	// std::cout << "out of while" << std::endl;
}

void Server::_acceptClnt() {
	int					client_sock;
	struct sockaddr_in	clnt_addr;
	socklen_t			clnt_addr_len = sizeof(clnt_addr);

	client_sock = accept(_sock_fd, (struct sockaddr*)&clnt_addr, &clnt_addr_len);
	if (client_sock == -1)
		throw std::runtime_error("accept function error: " + std::string(std::strerror(errno)) + '.'); // 디버깅할 필요 없을 땐 return으로 처리하는 게 좋을 듯
	fcntl(client_sock, F_SETFL, O_NONBLOCK);
	_changeEvents(client_sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	_clients[client_sock] = new Client(client_sock);
	_clients[client_sock]->setAddr(inet_ntoa(clnt_addr.sin_addr));
	std::cout << "Accepted Client: " << client_sock << ", " << _clients[client_sock]->getAddr() << std::endl;
}

void Server::_recvMsgFromClnt(int clnt_fd) {
	std::vector<char> rBuf(1024);

	std::map< int, std::string >::iterator	it = _read_string.find(clnt_fd);
	if (it == _read_string.end()) {
		_read_string.insert(std::make_pair(clnt_fd, ""));
		it = _read_string.find(clnt_fd);
	}

	// std::cout << "recv activated" << std::endl;
	int n = recv(clnt_fd, &rBuf[0], rBuf.capacity(), 0);
	if (n > 0) {
		it->second += std::string(rBuf.begin(), rBuf.begin() + n);
		if (it->second.find('\n') == std::string::npos && it->second.find('\r') == std::string::npos)
			return ;
	}
	else if (n < 0) {
		throw std::runtime_error("client error: " + std::string(std::strerror(errno)) + '.');
	}
	else {
		if (it->second == "") {
			std::cout << "\tRead nothing from client." << std::endl;
			_disconnectClnt(clnt_fd);
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
	_changeEvents(clnt_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);

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

		_command_queue.push(cmd);
		token_it++;
	}
}

void Server::_sendMsgToClnt(int clnt_fd, Command& cmd)
{
	const std::map< std::string, std::string >	msgs = cmd.getMsgs();

	std::map< std::string, std::string >::const_iterator	msgs_it;
	std::map< int, Client* >::iterator						clnt_it;

	msgs_it = msgs.begin();
	while (msgs_it != msgs.end()) { //전체 리시버 돌면서 하나씩 보내기

		std::vector< std::string >	channel_receiver;
		std::vector< std::string >::iterator	channel_receiver_it;
		std::vector< std::pair< bool, Client* > >::const_iterator	channel_member_it;

		Client* clnt = getClient(clnt_fd);
		if (msgs_it->first[0] == '#') {  //채널일경우
			std::map< std::string, Channel* >::iterator it = _channels.find(msgs_it->first);
			if (it != _channels.end()) {
				channel_member_it = it->second->getClients().begin();
				while (channel_member_it != it->second->getClients().end()) {
					std::string nickname = channel_member_it->second->getNickname();
					channel_receiver.push_back(nickname);
					channel_member_it++;
				}

				channel_receiver_it = channel_receiver.begin();
				while (channel_receiver_it != channel_receiver.end()) {
					_sendMsgModule(getClient(*channel_receiver_it), msgs_it->second);
					channel_receiver_it++;
				}
			}
			else
				std::cout << "send target channel not found" << std::endl;
		}
		else if ((cmd.getCmd() == "USER" || cmd.getCmd() == "NICK") && !(clnt->getConnected())) {
			_sendMsgModule(clnt_fd, msgs_it->second);
		}
		else //일반 유저일 경우
			_sendMsgModule(getClient(msgs_it->first), msgs_it->second);
		msgs_it++;
	}
	delete _command_queue.front();
	_command_queue.pop();
}

void	Server::_sendMsgModule(int dest_fd, const std::string& msg) {
	int			result;

	if (dest_fd != 0) {
		result = send(dest_fd, msg.c_str(), msg.size(), 0);
		if (result < 0) {
			std::cout << __func__ << ": SEND err. disconnect." << std::endl;
			_disconnectClnt(dest_fd);
		}
		else {
			std::cout << ">> Send msg to fd(" << dest_fd << ") :\n\t" << msg << "\n\n";
		}

	Client* clnt = getClient(dest_fd);
	if (!clnt->getConnected())
		_disconnectClnt(dest_fd);
	}
}

int		Server::_checkNewEvents() {
	int new_event = 0;
	// std::cout << "new_event while start" << std::endl;
	while (new_event <= 0) {
		new_event = kevent(_kq, &_kq_events[0], _kq_events.size(), _event, 8, NULL);
		if (new_event < 0)
			throw std::runtime_error("server socket close error: " + std::string(std::strerror(errno)) + '.');
	}
	// std::cout << "new_event while end" << std::endl;
	// 수정 필요 (kqevent가 event보다 크면 이벤트가 씹힐 우려)
	_kq_events.clear();
	return (new_event);
}

void	Server::_changeEvents(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata) {
	struct kevent tmp_event;

    EV_SET(&tmp_event, ident, filter, flags, fflags, data, udata);
    _kq_events.push_back(tmp_event);
}

//끊겼든 끊었든 호출됨.
void	Server::_disconnectClnt(int clnt_fd) {
	if (clnt_fd <= 0)
		return ;

	if (_clients.find(clnt_fd) != _clients.end()) {
		Client* disconnected_clnt = _clients.find(clnt_fd)->second;

		//quit 같은걸로 이미 잘 삭제된 경우가 아니라면
		//channel도 정리해줘야함
		if (!(disconnected_clnt->getCurrChannel().empty())) {
			std::vector<std::string> clnt_chans = disconnected_clnt->getCurrChannel();
			std::vector<std::string>::iterator it = clnt_chans.begin();
			std::map<std::string, Channel*>::iterator chan;

			while (it != clnt_chans.end()) {
				chan = _channels.find(*it);
				if (chan != _channels.end()) {
					Channel* channel = chan->second;
					channel->deleteClient(*disconnected_clnt);
					disconnected_clnt->leaveChannel(*channel);
					std::string nick = disconnected_clnt->getNickname();
					channel->rmInvitedClients(nick);
					if (channel->getClients().size() == 0)
						deleteChnl(channel);
				}
				++it;
			}
		}
	}

	if (close(clnt_fd) >= 0) {
		std::cout << "client disconnected: " << clnt_fd << std::endl;
		delete _clients.find(clnt_fd)->second;
	}
	_clients.erase(clnt_fd);
	_read_string.erase(clnt_fd);
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

void	Server::updateInvitedList(std::string& old_nick, std::string& new_nick) {
	std::map< std::string, Channel* > channels = getChannels();
	std::map< std::string, Channel* >::iterator chans_it = channels.begin();
	while (chans_it != channels.end()) {
		std::vector< std::string > invited_list = chans_it->second->getInvitedClients();
		std::vector< std::string >::iterator target = std::find(invited_list.begin(), invited_list.end(), old_nick);
		if (target != invited_list.end()) {
			chans_it->second->rmInvitedClients(old_nick);
			chans_it->second->addInvitedClient(new_nick);
		}
		chans_it++;
	}
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
