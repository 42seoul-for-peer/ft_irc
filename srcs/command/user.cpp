#include "Command.hpp"
#include <iostream>

bool	Command::_valid_user(std::string& new_user) const {
	if (new_user.find('\n') != std::string::npos || new_user.find('\0') != std::string::npos
		|| new_user.find('\r') != std::string::npos || new_user.find(' ') != std::string::npos)
		return false;
	return true;
}

void	Command::user(Client& send_clnt, Server& serv) {
	std::string new_user;
	std::string	msg;

	if (_args.size() < 4) {
		(void) serv;
		setMsgs(_send_nick, _genMsg(ERR_NEEDMOREPARAMS, _cmd));
		return ;
	}
	// 중복 등록 방지
	if (send_clnt.getUsername() != "*") {
		msg = _genMsg(ERR_ALREADYREGISTRED);
		setMsgs(_send_nick, msg);
		return ;
	}
	new_user = _args.front();
	_args.pop();
	if (_valid_user(new_user) == false) {
		return ; // inspircd의 경우 자체 error num을 가지고 있긴 함
	}
	// nickname이 이미 존재하는 경우
	if (send_clnt.getNickname() != "*") {
		if (send_clnt.getPassValidity() == true) {
			send_clnt.setUsername(new_user);
			send_clnt.setRegistered();
			_send_user = new_user;
			msg = _genMsg(RPL_WELCOME);
		}
		else {
			msg = "ERROR :Closing Link: [Access Denied by Configuration]\n";
			send_clnt.setConnected(false);
		}
		setMsgs(_send_nick, msg);
	}
	else {
		send_clnt.setUsername(new_user);
	}
}
