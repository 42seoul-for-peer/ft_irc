#ifndef JOIN_HPP
# define JOIN_HPP

# include "Command.hpp"
# include "Protocol.hpp"

class JOIN : public Command {
// OCCF
 public:
	JOIN();
	JOIN(const JOIN& copy);
	JOIN& operator = (const JOIN& copy);
	~JOIN();
// MEMBER VARIABLE
// MEMBER FUNCITON
 public:
	int execute(Protocol& protocol);
};

#endif
