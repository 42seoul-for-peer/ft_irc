#include <iostream>
#include "Command.hpp"

void	Command::pass(Client& send_clnt, Server& serv) {
	std::string	passwd;
	if (_args.size() < 1) {
		setMsgs(_send_nick, _genMsg(ERR_NEEDMOREPARAMS, _cmd));
		return ;
	}
	// passwd 문자열 형태로 복원 (공백 여러개가 들어가는 경우는 잘못되었다고 판단함)
	while (_args.size()) {
		passwd += _args.front();
		_args.pop();
		if (!_args.empty()) {
			passwd += ' ';
		}
	}
	// 잘못된 비밀번호
	if (passwd != serv.getPassword()){
		send_clnt.setPassValidity(0);
	}
	else {
		send_clnt.setPassValidity(1);
	}
}