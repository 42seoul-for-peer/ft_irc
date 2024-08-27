#ifndef PRIVMSG_HPP
# define PRIVMSG_HPP

# include <sstream>
# include <string>

# include "Command.hpp"
# include "../Protocol.hpp"

class PRIVMSG : public Command {
// OCCF
 public:
	~PRIVMSG();
	PRIVMSG(std::string cmd, std::string sender, std::vector<std::string> receiver);
 private:
	PRIVMSG();
	PRIVMSG(const PRIVMSG& copy);
	PRIVMSG& operator = (const PRIVMSG& copy);
// MEMBER VARIABLE
// MEMBER FUNCITON
 public:
	int 		execute();
	void		getProtocolMsg();
	PRIVMSG*	clone(std::stringstream input_cmd);
};

#endif
