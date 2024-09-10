#include <iostream>
#include "Command.hpp"

// PASS <password>

// ERR_NEEDMOREPARAMS 461
// ERR_ALREADYREGISTERED 462
// ERR_PASSWDMISMATCH 464

// 등록 전까지는 pass를 계속 받을 수 있고, update 되어야 함
// passwd의 경우는 띄어쓰기를 포함할 수 있으니까 다 받아서 처리해야함

void	Command::pass(Client& send_clnt, Server& serv) {
	std::string	prefix;
	std::string	passwd;
	if (_args.size() != 1) {
		prefix = serv.genPrefix(_sender, ERR_NEEDMOREPARAMS);
		setMsgs(_sender, _genProtoMsg(ERR_NEEDMOREPARAMS, prefix));
		return ;
	}
	// passwd 문자열 형태로 복원
	// 비밀번호가 "p(공백)(공백)(공백)wd"인 경우 복원 결과가 p(공백)wd가 되는 문제가 있음
	while (_args.size()){
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