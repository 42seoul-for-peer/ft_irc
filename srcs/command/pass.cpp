#include <iostream>
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

	const std::map< int, Client* >::const_iterator it = serv.getClients().find(clnt_fd);
	if (it == serv.getClients().end()) {
		Client* new_clnt = new Client(clnt_fd);
		std::cout << new_clnt << std::endl;
	}
	else {
		_rpl_no = 462;
		_proto_msg = ":You may not register";
	}
}