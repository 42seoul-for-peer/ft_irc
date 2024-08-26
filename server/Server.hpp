#ifndef SERVER_HPP
# define SERVER_HPP

# include <exception>
# include <string>
# include <sys/event.h>	// kqueue
# include <arpa/inet.h>
# include <fcntl.h> //fcntl

# include <vector>
# include <map>
# include <queue>

# include "Channel.hpp"
# include "Protocol.hpp"
# include "Command.hpp"
# include "command/JOIN.hpp"
# include "command/PRIVMSG.hpp"
// # include "command/CommandList.hpp" 이 헤더파일 안에 전부 include 되어 있음 (plan B)

class Client;
class Protocol;

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

	struct sockaddr_in	_addr;
	int					_sock_fd;
	int					_kq;

    std::map< int, Client* >	_clients; // <fd : buffer>
	std::vector<struct kevent>	_kq_events;
    struct kevent       		_event[8];

	std::map< std::string, Channel* >	_channels;

	std::queue< Protocol& > 			_protocols;

	std::vector< Command* >				_commandList;

// MEMBER FUNCITON
 public:
	void	serverInit(); //sock fd 초기화, kq 생성하기
	void	serverProcess();
	void	serverClose(); // 어디서 호출할지 고민해보기

	// under serverProcess();
	int		checkNewEvents();
	void	acceptClnt();
	void	recvMsgFromClnt(Client* client);	// parse message
	void	sendMsgToClnt();	// protocol 호출

	// cmd parser
	// protocol sender

	// cmd process
	// void processCMD(client, cmd);
	// client.cmd();
	// client 멤버 변수로 channel에 있는지 ㅇㄹ아야 하지 않을까요?

	// EVFILT_WRITE 조작 안해서 빠질 예정인 함수
	void	changeEvents(std::vector<struct kevent>& kqEvents, uintptr_t ident, int16_t filter, 
            	uint16_t flags, uint32_t fflags, intptr_t data, void *udata);



// EXCEPTION
 public:
	// 실행 인자로 받은 port 번호 오류
	class PortOutOfRangeException : public std::exception {
	 public:
	 	virtual const char* what() const throw();
	};

	// socket() 오류
	class ServInitFuncException : public std::exception {
	 public:
		ServInitFuncException(const std::string& func_name);
		virtual const char* what() const throw();

	 private:
		std::string _func_name;
	};

	class ServSockCloseException : public std::exception {
	 public:
	 	virtual const char* what() const throw();
	};
};

#endif
