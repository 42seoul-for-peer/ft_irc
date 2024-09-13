#include "Command.hpp"

void	Command::unknownCommand() {
	setMsgs(_send_nick, _genMsg(ERR_UNKNOWNCOMMAND));
	return ;
}