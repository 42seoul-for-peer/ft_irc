#include <iostream>
#include "PRIVMSG.hpp"

// OCCF
PRIVMSG::PRIVMSG() {
	std::cout << "Default constructor called for PRIVMSG.\n";
}

PRIVMSG::~PRIVMSG() {
	std::cout << "Default destructor called for PRIVMSG.\n";
}

PRIVMSG::PRIVMSG(const PRIVMSG& copy) {
	std::cout << "Copy constructor called for PRIVMSG.\n";
}

PRIVMSG& PRIVMSG::operator = (const PRIVMSG& copy) {
	std::cout << "Copy assignment called for PRIVMSG.\n";
	if (this == &copy)
		return (*this);
	return (*this);
}

// MEMBER FUNCTION

int PRIVMSG::execute(Protocol& protocol) {

}
