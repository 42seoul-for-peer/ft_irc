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

const std::map< std::string, int >& Command::getReceiver() const {
	return (_receiver);
}


const std::string	Command::getProtoMsg(std::string& recv_name) const {
	std::map< std::string, int>::const_iterator it = _receiver.find(recv_name);
	// it == end()인 경우 처리 해야하나?
	std::string reply = "";
	if (it->second > 399) {
		reply += std::to_string(it->second);
		reply += " " + _sender + " " + recv_name + " :"; // 마지막에 error message 들어가야 함
	}
	else {
		reply += _cmd;
	}
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
	else if (_cmd != "")
		unknownCommand(send_clnt, serv);
}

std::string	Command::execute() {
	return (std::string("empty"));
}