#include "Command.hpp"

// <command> :Unknown command"

void	Command::unknownCommand(int clnt_fd, Server& serv) {
	(void) clnt_fd;
	(void) serv;
	_rpl_no = ERR_UNKNOWNCOMMAND; // ERR_UNKNOWNCOMMAND 421
}