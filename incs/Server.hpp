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
	// Server& operator = (const Server& copy);
// MEMBER VARIABLE
 private:
	int			_port;
	std::string	_password;
	std::string	_serv_name;

	int					_sock_fd;
	int					_kq;
	struct sockaddr_in	_addr;

	std::vector<struct kevent>		_kq_events;
	std::map< int, std::string >	_readString; // key: fd, value: data from key fd
	struct kevent       			_event[8];

	std::map< int, Client* >			_clients; // key:fd, value: Client
	std::map< std::string, Channel* >	_channels; // key: channel name, value: Channel
	std::queue< Command* > 				_commandQueue;

// MEMBER FUNCITON
 public:
	void	serverInit();
	void	serverProcess();

 private:
	// serverProcess();
	int		_checkNewEvents();
	void	_acceptClnt();
	void	_recvMsgFromClnt(int clnt_fd); //버퍼를 받아와서 cmd객체 생성, cmd parse 함수, 검사해서 괜찮으면 exec
	void	_sendMsgToClnt(Command& cmd); //writable 한지 보고 전송
	void	_sendMsgModule(const std::string& recv, const std::string& msg);

	void	_changeEvents(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
	void	_disconnectClnt(int clnt_fd);

 public:
	// clnt controller
	void	addNewClnt(int clnt_fd, Client* clnt);
	void	deleteClnt(int clnt_fd);
	// chnl controller
	void	addNewChnl(Channel* chnl);
	void	deleteChnl(Channel* chnl);

	// getter
	int											getPort() const;
	int											getClient(std::string nickname) const;
	Client*										getClient(int clnt_fd) const;
	const std::map< int, Client* >&				getClients() const;
	const std::string&							getPassword() const;
	const std::map< std::string, Channel* >&	getChannels() const;
	const std::string&							getServName() const;
};

#endif
