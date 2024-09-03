#include <iostream>
#include "Command.hpp"

// PASS <password>

// ERR_NEEDMOREPARAMS 461
// ERR_ALREADYREGISTERED 462
// ERR_PASSWDMISMATCH 464

void	Command::pass(Client& send_clnt, Server& serv) {
	if (_args.size() != 1) {
		_receiver.insert(make_pair(std::string(""), ERR_NEEDMOREPARAMS));
		return ;
	}

	if (send_clnt.getIsRegistered() == false) {
		if (serv.getPassword() != _args.front()) {
			_receiver.insert(make_pair(send_clnt.getNickname(), ERR_PASSWDMISMATCH));
			return ;
		}
		send_clnt.setRegistered();
	}
	else {
		_receiver.insert(make_pair(send_clnt.getNickname(), ERR_ALREADYREGISTRED));
	}
	// nick(send_clnt, serv);
	// user(send_clnt, serv);
	// _receiver.insert(make_pair(std::string(send_clnt.getNickname()), RPL_WELCOME));
	std::cout << "returned to pass\n" << _receiver.size() << std::endl;
}