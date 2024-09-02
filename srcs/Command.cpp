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
	// _sender = serv.clnt_list.find(clnt_fd); << pass 때문에 각 명령어에서 처리해야됨
	Client&	send_clnt = *(serv.getClients().find(clnt_fd)->second);
	if (_cmd == "PASS")
		pass(send_clnt, serv);
	else if (_cmd == "NICK")
		nick(send_clnt, serv);
	else if (_cmd == "USER")
		user(send_clnt, serv);
	else if (_cmd == "JOIN")
		join(send_clnt, serv);
	else if (_cmd == "PRIVMSG")
		join(send_clnt, serv);
	else
		unknownCommand(send_clnt, serv);
}

std::string	Command::execute() {
	return (std::string("empty"));
}