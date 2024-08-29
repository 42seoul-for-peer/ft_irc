#include <iostream>
#include "Command.hpp"

// OCCF
Command::Command() {
	// std::cout << "Default constructor called for Command.\n";
}

Command::~Command() {
	// std::cout << "Default destructor called for Command.\n";
}

Command::Command(const Command& copy) {
	// std::cout << "Copy constructor called for Command.\n";
}

// parameterized constructor
Command::Command(std::stringstream& input_cmd) {
	// std::cout << "Parameterized constructor called for Command.\n";
	std::string tmp;

	input_cmd >> _cmd;
	while (input_cmd >> tmp) {
		_args.push(tmp);
	}
	_rpl_no = 0;
}

Command& Command::operator = (const Command& copy) {
	// std::cout << "Copy assignment called for Command.\n";
	if (this == &copy)
		return (*this);
	return (*this);
}

// MEMBER FUNCTION
// getter
const std::string&	Command::getCmd() const {
	return (_cmd);
}

const std::string&	Command::getSender() const {
	return (_sender);
}

const std::queue< std::string >& Command::getReceiver() const {
	return (_receiver);
}

int	Command::getReceiverCnt() const {
	return (_receiver_cnt);
}

const std::string&	Command::getProtoMsg() const {
	return (_proto_msg);
}

int	Command::getReplyNumber() const {
	return (_rpl_no);
}

// setter

// usable function
void	Command::parse(int clnt_fd, Server& serv) {
	// _sender = serv.clnt_list.find(clnt_fd); << pass 때문에 각 명령어에서 처리해야됨
	if (_cmd == "PASS")
		pass(clnt_fd, serv);
	else
		unknownCommand(serv);
}

void	Command::execute() {
}