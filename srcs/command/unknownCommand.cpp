#include "Command.hpp"

void	Command::unknownCommand(Server& serv) {
	setMsgs(_send_nick, _genMsg(ERR_UNKNOWNCOMMAND));
	return ;
}