#include "Command.hpp"

void	Command::unknownCommand(Server& serv) {
	std::string	prefix = serv.genPrefix(_sender, ERR_UNKNOWNCOMMAND);
	std::string msg = _genProtoMsg(ERR_UNKNOWNCOMMAND, prefix);
	setMsgs(_sender, msg);
	return ;
}