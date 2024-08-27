#include <iostream>
#include "Command.hpp"

// OCCF
// Command::Command() {
// 	// std::cout << "Default constructor called for Command.\n";
// }

Command::~Command() {
	std::cout << "Default destructor called for Command.\n";
}

Command::Command(const Command& copy) {
	// std::cout << "Copy constructor called for Command.\n";
	_cmd = copy._cmd;
	// _sender = copy._sender;
	// _receiver = copy._receiver;
	// _proto_msg = copy._proto_msg;
}

// parameterized constructor
Command::Command(std::stringstream input_cmd) {
	// std::cout << "Parameterized constructor called for Command.\n";
	input_cmd >> _cmd;
}

// Command& Command::operator = (const Command& copy) {
// 	// std::cout << "Copy assignment called for Command.\n";
// 	if (this == &copy)
// 		return (*this);
// 	return (*this);
// }

// MEMBER FUNCTION
