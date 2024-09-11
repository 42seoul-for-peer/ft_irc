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
	std::string					_send_nick;
	std::string					_send_user;
	std::map< std::string, std::string > _msgs;
	// <clnt or chan name, msg>
	// std::string					_msg;

// MEMBER FUNCITON
 public:
// getter
	const std::string&								getCmd() const;
	const std::string&								getSender() const;
	// int												getReceiverCnt() const;
	const std::map< std::string, int >&	getReceiver() const;
	const std::map< std::string, std::string>& getMsgs() const;
	// getReceiver의 경우 const 키워드를 걸어야 하는 getter의 한계 때문에 execute 호출해서 메세지 받는 것으로 대신 하기


	// const std::string getMsg() const; // test 용으로 임시로 추가함

	// protocol message 내부에 receiver가 변경되는 경우가 있어 getProtoMsg() const가 적절한 형태인지 모르겠음
	// 변수로 저장하기 보단 매번 생성해서 보내는 형태가 비교적 적절할 것 같음
	// const std::string	getProtoMsg(const std::pair< std::string, int>& recv, const std::string& target) const;
	// <rpl_no, actual msg>

// setter etMsg(str& name, str& msg)
	void	setMsgs(const std::string& name, const std::string& msg); // msgs 변수에 새 메세지 연장/추가 하는 함수
// usable function
	void		parse(int clnt_fd, Server& serv);
	std::string	execute();
// irc message
	void	pass(Client& send_clnt, Server& serv);
	void	nick(Client& send_clnt, Server& serv);
	void	user(Client& send_clnt, Server& serv);
	void	join(Client& send_clnt, Server& serv);
	void	privmsg(Server& serv);
	void	mode(Server& serv);
	void	kick(Server& serv);
	void	part(Client& send_clnt, Server& serv);
	void	topic(Server& serv);
	void	invite(Server& serv);
	void	quit(Server& serv);
	// void	ping();
	void	unknownCommand(Server& serv);

 private:
	void	_kMode(bool flag, Server& serv, Channel* chan);
	void	_iMode(bool flag, Server& serv, Channel* chan);
	void	_lMode(bool flag, Server& serv, Channel* chan);
	void	_tMode(bool flag, Server& serv, Channel* chan);
	void	_oMode(bool flag, Server& serv, Channel* chan);

	bool	_valid_nick(std::string& new_nick) const;

	bool	_valid_user(std::string& new_user) const;

	const std::string	_genPrefix(int rpl_no) const;

	const std::string	_genRplMsg(int rpl_no) const;
	const std::string	_genErrMsg(int rpl_no) const;

	const std::string	_genMsg(int rpl_no) const;
	const std::string	_genMsg(int rpl_no, const std::string& param) const;
	const std::string	_genMsg(int rpl_no, const std::string& param1, const std::string& param2) const;

	const std::string	_genProtoMsg(int rpl_no, const std::string& prefix) const;
	const std::string	_genProtoMsg(int rpl_no, const std::string& prefix, const std::string& target1) const;
	const std::string	_genProtoMsg(int rpl_no, const std::string& prefix, const std::string& target1, const std::string& target2) const;

	std::vector<std::string> _parsebyComma(std::queue<std::string>& _args);
	const std::string _appendRemaining();
};

#endif
