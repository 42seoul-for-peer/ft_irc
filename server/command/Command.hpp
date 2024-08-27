#ifndef COMMAND_HPP
# define COMMAND_HPP

# include <string>
# include <vector>
# include <sstream>

class Command {
// OCCF
 public:
	Command(const Command& copy);
	virtual ~Command();
// parameterized constructor
	Command(std::stringstream input_cmd);
	Command(std::string cmd, std::string sender, std::vector<std::string> receiver);
	Command(std::string cmd, std::string sender, std::string chan_name);
 private:
	Command& operator = (const Command& copy);
	Command();
// MEMBER VARIABLE
 protected:
	std::string					_cmd;
	std::string					_sender;
	std::string					_chan_name;
	std::vector<std::string>	_receiver;
	std::string					_proto_msg;
// MEMBER FUNCITON
 public:
	const std::string	getCmd() const; // 찾는 용도라면
	virtual int			execute() = 0;
	virtual void		genProtocolMsg() = 0;
	virtual Command*	clone(std::stringstream input_cmd) = 0;
};

#endif
