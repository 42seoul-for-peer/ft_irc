#include "Command.hpp"

// PASS <password>

// ERR_NEEDMOREPARAMS
// ERR_ALREADYREGISTERED

void	Command::pass(int clnt_fd, Server& serv) {
	if (_args.size() != 1) {
		_rpl_no = 461;
		_proto_msg = _cmd + " :Not enough parameters";
		return ;
	}
	std::map< int, Client* >::iterator it = serv.getClients().find(clnt_fd);
	if (it == serv.getClients().end()) {
		_confirmed_args.push(_args.front());
	}
	else {
		_rpl_no = 462;
		_proto_msg = ":You may not register";
	}
}