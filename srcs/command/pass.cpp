#include <iostream>
#include "Command.hpp"

// PASS <password>

// ERR_NEEDMOREPARAMS 461
// ERR_ALREADYREGISTERED 462
// ERR_PASSWDMISMATCH 464

void	Command::pass(Client& send_clnt, Server& serv) {
	if (_args.size() != 1) {
		_receiver.push(make_pair(send_clnt.getNickname(), ERR_NEEDMOREPARAMS));
		_msg = "Not enought parameters";
		return ;
	}

	if (send_clnt.getIsRegistered() == false) {
		if (serv.getPassword() != _args.front()) {
			_receiver.push(make_pair(send_clnt.getNickname(), ERR_PASSWDMISMATCH));
			_msg = _cmd + "Password incorrect";
			return ;
		}
		send_clnt.setRegistered();
	}
	else {
		_receiver.push(make_pair(send_clnt.getNickname(), ERR_ALREADYREGISTRED));
		_msg = "You may not register";
	}
	nick(send_clnt, serv);
	user(send_clnt, serv);
	if (_receiver.empty())
		_receiver.push(make_pair(send_clnt.getNickname(), RPL_WELCOME));
}