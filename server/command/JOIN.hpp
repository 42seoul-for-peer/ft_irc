#ifndef JOIN_HPP
# define JOIN_HPP

# include <sstream>

# include "Command.hpp"
# include "../Protocol.hpp"

class JOIN : public Command {
// OCCF
 public:
	JOIN();
	~JOIN();
 private:
	JOIN(const JOIN& copy);
	JOIN& operator = (const JOIN& copy);
// MEMBER VARIABLE
// MEMBER FUNCITON
 public:
	int		execute();
	void	getProtocolMsg();
	JOIN*	clone(std::stringstream input_cmd);
};

#endif
