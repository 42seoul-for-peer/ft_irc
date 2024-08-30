#include "Command.hpp"

void	Command::unknownCommand(int clnt_fd, Server& serv) {
	(void) clnt_fd;
	(void) serv;
	_rpl_no = 421; // ERR_UNKNOWNCOMMAND
}