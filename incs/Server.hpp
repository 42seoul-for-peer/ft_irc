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

# include "Command.hpp"

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
	std::queue< Command* > 				_commandQueue;

// MEMBER FUNCITON
 public:
	void	serverInit(); //sock fd 초기화, kq 생성하기
	void	serverProcess();
	void	serverClose(); // 어디서 호출할지 고민해보기

	// under serverProcess();
	int		checkNewEvents();
	void	acceptClnt();
	void	recvMsgFromClnt(int clnt_fd); //버퍼를 받아와서 cmd객체 생성, cmd parse 함수, 검사해서 괜찮으면 exec
	void	sendMsgToClnt(); //writable 한지 보고 전송


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

	class ClntAcceptionFailException : public std::exception {
	 public:
	 	virtual const char* what() const throw();
	};
};

#endif
