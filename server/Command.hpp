#ifndef COMMAND_HPP
# define COMMAND_HPP

# include "Protocol.hpp"

class Command {
 private:
	std::string cmd;
 public:
	virtual int	execute(Protocol& protocol) = 0;
	std::string getCmd();
	
};

#endif