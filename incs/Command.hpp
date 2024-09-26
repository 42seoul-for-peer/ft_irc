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
	Command();
	Command(const Command& copy);
	Command& operator = (const Command& copy);
// MEMBER VARIABLE
 protected:
	std::string					_cmd;
	std::queue< std::string >	_args;

	std::string					_send_nick;
	std::string					_send_user;
	std::string					_send_addr;
	std::map< std::string, std::string > _msgs; // key: clnt/chan name, value: msg

// MEMBER FUNCITON
 public:
// getter
	const std::string&		getCmd() const;
	const std::map< std::string, std::string>& getMsgs() const;

// setter
	void	setMsgs(const std::string& name, const std::string& msg); // msgs 변수에 새 메세지 연장/추가 하는 함수

// usable function
	void		parse(int clnt_fd, Server& serv);
	std::string	execute();

// irc commands
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
	void	unknownCommand();

 private:
	std::string	_makeModeMsg(std::queue< std::pair< int, std::string > > token_queue);
	void		_promptMode(bool is_joined, Channel* chan);
	bool		_iMode(bool flag, Channel* chan);
	bool		_kMode(bool flag, Channel* chan);
	bool		_lMode(bool flag, Channel* chan);
	bool		_tMode(bool flag, Channel* chan);
	bool		_oMode(bool flag, Channel* chan);

	bool	_valid_nick(std::string& new_nick) const;

	bool	_valid_user(std::string& new_user) const;

// msg generators
	const std::string	_genPrefix(int rpl_no) const;

	const std::string	_genRplMsg(int rpl_no) const;
	const std::string	_genErrMsg(int rpl_no) const;

	const std::string	_genMsg(int rpl_no) const;
	const std::string	_genMsg(int rpl_no, const std::string& param) const;
	const std::string	_genMsg(int rpl_no, const std::string& param1, const std::string& param2) const;

	// util
	std::vector<std::string>	_parsebyComma();
	const std::string			_appendRemaining();
};

#endif
