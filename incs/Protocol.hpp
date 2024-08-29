#ifndef PROTOCOL_HPP
# define PROTOCOL_HPP

# include "Client.hpp"
# include "Channel.hpp"
# include "command/Command.hpp"

class Client;
class Channel;
// class Command;

class Protocol {
// OCCF
 public:
	Protocol(const Command* cmd);
	~Protocol();
 private:
	Protocol(const Protocol* copy);
	Protocol* operator = (const Protocol* copy);
	Protocol();
// MEMBER VARIABLE
 private:
	Command* _cmd;
	std::pair<int, Client*> _sender;
	std::vector<Channel*> _channel;
	std::vector< std::pair< int, Client*> > _receivers;

// MEMBER FUNCITON
 public:
	const std::string&	getCmd() const;
};

#endif
