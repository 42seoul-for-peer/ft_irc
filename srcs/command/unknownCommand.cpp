#include "Command.hpp"

void	Command::unknownCommand(Server& serv) {
	_rep_no = 421; // ERR_UNKNOWNCOMMAND
}