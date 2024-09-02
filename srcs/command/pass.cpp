#include <iostream>
#include "Command.hpp"

// PASS <password>

// ERR_NEEDMOREPARAMS 461
// ERR_ALREADYREGISTERED 462
// ERR_PASSWDMISMATCH 464

void	Command::pass(int clnt_fd, Server& serv) {
	std::cout << "arg size: " << _args.size() << std::endl;
	if (_args.size() != 1) {
		_rpl_no = ERR_NEEDMOREPARAMS;
		_proto_msg = _cmd + " :Not enough parameters";
		return ;
	}

	const std::map< int, Client* >::const_iterator it = serv.getClients().find(clnt_fd);
	if (it->second->getIsRegistered() == false) {
		if (serv.getPassword() != _args.front()) {
			_rpl_no = ERR_PASSWDMISMATCH;
			_proto_msg = _cmd + ":Password incorrect";
			std::cout << _proto_msg << std::endl;
			return ;
		}
		it->second->setRegistered();
		std::cout << "password registered\n";
	}
	else {
		_rpl_no = ERR_ALREADYREGISTRED;
		_proto_msg = ":You may not register";
		std::cout << _proto_msg << std::endl;
	}
	nick(clnt_fd, serv);
	user(clnt_fd, serv);
}