#ifndef PROTOCOL_HPP
# define PROTOCOL_HPP

# include "Client.hpp"
# include "Channel.hpp"

class Protocol {
// OCCF
 public:
	Protocol(const std::string& cmd);
	~Protocol();
 private:
	Protocol(const Protocol& copy);
	Protocol& operator = (const Protocol& copy);
	Protocol();
// MEMBER VARIABLE
 private:
	std::string	_cmd;
	std::pair< int, Client& > _sender;
	std::vector< std::pair< int, Client& > > _receivers;
	std::vector< Channel& > _channel;

// MEMBER FUNCITON
 public:
	const std::string&	getCmd() const;
};

#endif
