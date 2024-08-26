#ifndef PRIVMSG_HPP
# define PRIVMSG_HPP

# include "Command.hpp"
# include "Protocol.hpp"

class PRIVMSG : public Command {
// OCCF
 public:
	PRIVMSG();
	PRIVMSG(const PRIVMSG& copy);
	PRIVMSG& operator = (const PRIVMSG& copy);
	~PRIVMSG();
// MEMBER VARIABLE
// MEMBER FUNCITON
 public:
	int execute(Protocol& protocol);
};

#endif
