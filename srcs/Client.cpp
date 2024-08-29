#include <iostream>
#include <sstream>
#include "Client.hpp"

// OCCF
Client::Client() {
	std::cout << "Default constructor called for Client.\n";
}

Client::~Client() {
	std::cout << "Default destructor called for Client.\n";
}

Client::Client(const Client& copy) {
	std::cout << "Copy constructor called for Client.\n";
}

Client& Client::operator = (const Client& copy) {
	std::cout << "Copy assignment called for Client.\n";
	if (this == &copy)
		return (*this);
	return (*this);
}

// MEMBER FUNCTION

Client::Client(int clientSock)
	: _sock_fd(clientSock)
{
	
}
/// 도움.....1!?!?!?!?
// 여기 왔을 때 buffer는 이미 비어있다는 전제.
void Client::recvToBuff() {
	std::vector<char> rBuf(1024);
	int n = recv(_sock_fd, &rBuf[0], rBuf.capacity(), 0);
	if (n > 0) {
		rBuf.erase(rBuf.begin() + n, rBuf.end());
		_buffer = std::string(rBuf.begin(), rBuf.end());
	}
	else {
		if (n < 0)
			throw ReadErrorException();
		// disconnectClient();
	}
}

// parse에서 _buffer가 clear되어 더이상 사용하지 않음을 확실시해야함 - seungjun
Protocol*	Client::parse() {
	// std::stringstream	stream(_buffer);
	// Command*			command;
	// std::string		cmd;

	// /*buffer를 가지고 이것저것*/
	// std::getline(stream, cmd, ' ');
	// if (cmd.compare("JOIN") == 0)
	// 	command = new JOIN(stream.str());
	// else if (cmd.compare("PRIVMSG") == 0)
	// 	command = new PRIVMSG(stream.str());
	// else if (cmd.compare("KICK") == 0)
	// 	command = new KICK(stream.str());
	// _buffer.clear();s
	return (nullptr);
}

// EXCEPTION
const char* Client::ReadErrorException::what() const throw() {
	std::stringstream	stream;
	char				*reason;

	stream << "recv error " << std::strerror(errno) << '.';
	stream >> reason;
	return (reason);
}