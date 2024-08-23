#include <iostream>
#include "Channel.hpp"

// OCCF
Channel::Channel() {
	std::cout << "Default constructor called for Channel.\n";
}

Channel::~Channel() {
	std::cout << "Default destructor called for Channel.\n";
}

Channel::Channel(const Channel& copy) {
	std::cout << "Copy constructor called for Channel.\n";
}

Channel& Channel::operator = (const Channel& copy) {
	std::cout << "Copy assignment called for Channel.\n";
	if (this == &copy)
		return (*this);
	return (*this);
}

// MEMBER FUNCTION
