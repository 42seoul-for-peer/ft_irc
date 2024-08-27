#include <iostream>
#include "PRIVMSG.hpp"

// OCCF
// PRIVMSG::PRIVMSG() {
// 	// std::cout << "Default constructor called for PRIVMSG.\n";
// }

PRIVMSG::~PRIVMSG() {
	// std::cout << "Default destructor called for PRIVMSG.\n";
}

PRIVMSG::PRIVMSG(const PRIVMSG& copy)
: Command(copy) {
	// std::cout << "Copy constructor called for PRIVMSG.\n";
}

PRIVMSG& PRIVMSG::operator = (const PRIVMSG& copy) {
	// std::cout << "Copy assignment called for PRIVMSG.\n";
	if (this == &copy)
		return (*this);
	return (*this);
}

PRIVMSG::PRIVMSG(std::string cmd, std::string sender, std::vector<std::string> receiver)
: Command(cmd, sender, receiver) {
	// std::cout << "Parameterized constructor called for PRIVMSG.\n";
	// _proto_msg << ;                
}

// MEMBER FUNCTION

int PRIVMSG::execute() {

}

PRIVMSG* PRIVMSG::clone(std::stringstream input_cmd) {
	std::string cmd, sender;
	std::vector<std::string> receiver;

	input_cmd >> cmd >> sender;
	while (!input_cmd.eof()) {
		std::string individual_receiver;
		input_cmd >> individual_receiver;
		receiver.push_back(individual_receiver);
	}
	PRIVMSG* tmp = new PRIVMSG();

	tmp->_cmd = "PRIVMSG";
	tmp->_sender 
}