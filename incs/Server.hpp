#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <sys/event.h>	// kqueue
# include <arpa/inet.h>
# include <fcntl.h> //fcntl

# include <vector>
# include <map>
# include <queue>

# include "Channel.hpp"
# include "Command.hpp"

class Client;

class Channel;

class Command;

class Server {
// OCCF
 public:
	Server(const std::string& port, const std::string& password);
	~Server();
 private:
	Server();
	Server(const Server& copy);
	Server& operator = (const Server& copy);
// MEMBER VARIABLE
 private:
	int			_port;
	std::string	_password;
	std::string	_serv_name;

	struct sockaddr_in	_addr;
	int					_sock_fd;
	int					_kq;

	std::vector<struct kevent>	_kq_events;
	struct kevent       		_event[8];

	std::map< int, Client* >			_clients; // <fd : buffer>
	std::map< std::string, Channel* >	_channels;
	std::queue< Command* > 				_commandQueue;

// MEMBER FUNCITON
 public:
	void	serverInit(); //sock fd 초기화, kq 생성하기
	void	serverProcess();
	// void	serverClose(); // 어디서 호출할지 고민해보기

	// under serverProcess();
	int		checkNewEvents();
	void	acceptClnt();
	void	recvMsgFromClnt(int clnt_fd); //버퍼를 받아와서 cmd객체 생성, cmd parse 함수, 검사해서 괜찮으면 exec
	void	sendMsgToClnt(Command& cmd); //writable 한지 보고 전송

	void	changeEvents(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
	void	disconnectClnt(int clnt_fd);

	// clnt, chnl controller
	void	addNewClnt(int clnt_fd, Client* clnt);
	void	deleteClnt(int clnt_fd);
	void	addNewChnl(Channel* chnl);

	// getter
	int											getPort() const;
	const std::string&							getPassword() const;
	const std::map< int, Client* >&				getClients() const;
	Client*										getClient(int clnt_fd) const;
	int											getClient(std::string nickname) const;
	const std::map< std::string, Channel* >&	getChannels() const;
	const std::string&							getServName() const;
};

#endif
