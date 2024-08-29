#ifndef COMMAND_HPP
# define COMMAND_HPP

# include <string>
# include <vector>
# include <sstream>

# include "Server.hpp"
# include "Client.hpp"
# include "Channel.hpp"

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

	std::vector< std::string >	_args;
	std::string					_sender;
	int							_receiver_cnt;
	std::vector< std::string >	_receiver;
	std::string					_proto_msg;

	int							_rep_no;

// MEMBER FUNCITON
 public:
// getter
	const std::string&					getCmd() const;
	const std::string&					getSender() const;
	int									getReceiverCnt() const;
	const std::vector< std::string >&	getReceiver() const;
	// protocol message 내부에 receiver가 변경되는 경우가 있어 getProtoMsg() const가 적절한 형태인지 모르겠음
	// 변수로 저장하기 보단 매번 생성해서 보내는 형태가 비교적 적절할 것 같음
	const std::string&					getProtoMsg() const;

	int									getReplyNumber() const;
// setter
// usable function
	void	parse(int clnt_fd, Server& serv);
	void	execute();
// irc message
	// void	pass();
	// void	nick();
	// void	user();
	// void	join();
	// void	privmsg();
	// void	kick();
	// void	invite();
	// void	topic();
	// void	mode();
	// void	part();
	void	connect(Server& serv);
	// void	quit();
	// void	ping();
	void	unknownCommand(Server& serv);
};

#endif
