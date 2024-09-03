#include <iostream>
#include "Command.hpp"

// OCCF
Command::~Command() {
	// std::cout << "Default destructor called for Command.\n";
}

// parameterized constructor
Command::Command(std::stringstream& input_cmd) {
	// std::cout << "Parameterized constructor called for Command.\n";
	std::string tmp;

	input_cmd >> _cmd;
	while (input_cmd >> tmp) {
		_args.push(tmp);
	}
}

// MEMBER FUNCTION
// getter
const std::string&	Command::getCmd() const {
	return (_cmd);
}

const std::string&	Command::getSender() const {
	return (_sender);
}


const std::string	Command::getProtoMsg(std::string& recv_name, std::string& serv_name) const {
	std::string	reply(_sender);
	reply += "!" + serv_name + " " + _cmd + " " + recv_name + ":" + _msg;
	return (reply);
}

// setter

// usable function
void	Command::parse(int clnt_fd, Server& serv) {
	const Client*	send_clnt = serv.getClient(clnt_fd);
	// 해당 fd가 목록에 없는 경우
	if (!send_clnt)
		return ;
	if (_cmd == "PASS")
		pass(*send_clnt, serv);
	else if (_cmd == "NICK")
		nick(*send_clnt, serv);
	else if (_cmd == "USER")
		user(*send_clnt, serv);
	else if (_cmd == "JOIN")
		join(*send_clnt, serv);
	else if (_cmd == "PRIVMSG")
		join(*send_clnt, serv);
	else
		unknownCommand(*send_clnt, serv);
}

std::string	Command::execute() {
	return (std::string("empty"));
}