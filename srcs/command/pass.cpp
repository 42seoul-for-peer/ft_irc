#include <iostream>
#include "Command.hpp"

// PASS <password>

// ERR_NEEDMOREPARAMS 461
// ERR_ALREADYREGISTERED 462
// ERR_PASSWDMISMATCH 464

// 등록 전까지는 pass를 계속 받을 수 있고, update 되어야 함
// passwd의 경우는 띄어쓰기를 포함할 수 있으니까 다 받아서 처리해야함

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