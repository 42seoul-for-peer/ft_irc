#include <iostream>
#include "JOIN.hpp"

// OCCF
JOIN::JOIN() {
	std::cout << "Default constructor called for JOIN.\n";
}

JOIN::~JOIN() {
	std::cout << "Default destructor called for JOIN.\n";
}

JOIN::JOIN(const JOIN& copy) {
	std::cout << "Copy constructor called for JOIN.\n";
}

JOIN& JOIN::operator = (const JOIN& copy) {
	std::cout << "Copy assignment called for JOIN.\n";
	if (this == &copy)
		return (*this);
	return (*this);
}

// MEMBER FUNCTION

int	JOIN::execute() {

}

void JOIN::getProtocolMsg() {

}

JOIN* JOIN::clone(std::stringstream input_cmd) {
	JOIN* tmp = new JOIN();
	tmp->_cmd = "JOIN";
	
}