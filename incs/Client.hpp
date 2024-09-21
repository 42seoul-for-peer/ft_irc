#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <vector>
# include <map>
# include "Channel.hpp"

class Channel;

class Client {
// OCCF
 public:
	Client(int clientSock);
	~Client();
 private:
	Client();
	Client(const Client& copy);
	Client& operator = (const Client& copy);

// MEMBER VARIABLE
 private:
	int 		_sock_fd;
	int			_pass_validity; // input 없음: -1, 틀림: 0, 맞음: 1
	bool		_registered; // 등록 완료 시 true
	bool		_connected; // 연결 유지: true, 연결 해제해야함: false
	std::string	_addr;
	std::string _username;
	std::string _nickname;
	std::vector< std::string > _curr_channel;

// MEMBER FUNCTION
 public:
	// GETTER
	int 				getSockFd(void) const;
	int					getPassValidity(void) const;
	bool				getConnected(void) const;
	bool				getRegistered(void) const;
	const std::string& 	getAddr(void) const;
	const std::string& 	getUsername(void) const;
	const std::string& 	getNickname(void) const;
	const std::vector< std::string >& getCurrChannel(void) const;

	// SETTER
	void	setAddr(std::string str);
	void	setPassValidity(int flag);
	void	setConnected(bool flag);
	void	setRegistered();
	void	setUsername(std::string& str);
	void	setNickname(std::string& str);

	// HANDLING CHANNEL
	void	joinChannel(const Channel& channel);
	void	leaveChannel(const Channel& channel);
};

#endif