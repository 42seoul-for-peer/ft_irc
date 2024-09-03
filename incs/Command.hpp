#ifndef COMMAND_HPP
# define COMMAND_HPP

# include <string>
# include <sstream>
# include <queue>
# include <map>

# include "Server.hpp"
# include "Client.hpp"
# include "Channel.hpp"
# include "replyNumber.hpp"

class Server;
class Client;
class Channel;

class Command {
// OCCF
 public:
	~Command();
// parameterized constructor
	Command(std::stringstream& input_cmd);
 private:
	Command(const Command& copy);
	Command& operator = (const Command& copy);
	Command();
// MEMBER VARIABLE
 protected:
	std::string					_cmd;

	std::queue< std::string >	_args;

	std::string					_sender;
	std::map< std::string, int >	_receiver;
	// <recv_name: rpl_no> // 오류가 발생한 recv_name도 일단 그대로 넣어둔다
	// proto_msg 만들 때 rpl_no 검사할 것임
	std::string					_msg;

// MEMBER FUNCITON
 public:
// getter
	const std::string&								getCmd() const;
	const std::string&								getSender() const;
	// int												getReceiverCnt() const;
	const std::map< std::string, int >&	getReceiver() const;
	// getReceiver의 경우 const 키워드를 걸어야 하는 getter의 한계 때문에 execute 호출해서 메세지 받는 것으로 대신 하기


	const std::string getMsg() const; // test 용으로 임시로 추가함

	// protocol message 내부에 receiver가 변경되는 경우가 있어 getProtoMsg() const가 적절한 형태인지 모르겠음
	// 변수로 저장하기 보단 매번 생성해서 보내는 형태가 비교적 적절할 것 같음
	const std::string	getProtoMsg(std::string& recv_name) const;
	// <rpl_no, actual msg>

// setter
// usable function
	void		parse(int clnt_fd, Server& serv);
	std::string	execute();
// irc message
	void	pass(Client& send_clnt, Server& serv);
	void	nick(Client& send_clnt, Server& serv);
	void	user(Client& send_clnt, Server& serv);
	void	join(Client& send_clnt, Server& serv);
	void	privmsg(Client& send_clnt, Server& serv);
	// void	kick();
	// void	invite();
	// void	topic();
	// void	mode();
	// void	part();
	// void	quit();
	// void	ping();
	void	unknownCommand(Client& send_clnt, Server& serv);
 private:
	std::string _genErrMsg(int err_no);
};

#endif
