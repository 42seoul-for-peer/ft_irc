#include "Command.hpp"

// <command> :Unknown command"

void	Command::unknownCommand(Client& send_clnt, Server& serv) {
	(void) send_clnt;
	(void) serv;
	_receiver.push(make_pair(send_clnt.getNickname(), ERR_UNKNOWNCOMMAND));
	_msg = "Unknown command";
}