#include <iostream>
#include "Command.hpp"

// PASS <password>

// ERR_NEEDMOREPARAMS 461
// ERR_ALREADYREGISTERED 462
// ERR_PASSWDMISMATCH 464

// 등록 전까지는 pass를 계속 받을 수 있고, update 되어야 함
// passwd의 경우는 띄어쓰기를 포함할 수 있으니까 다 받아서 처리해야함

void	Command::pass(Client& send_clnt, Server& serv) {
	std::string			prefix;
	const std::string	send_nick = send_clnt.getNickname();
	if (_args.size() != 1) {
		prefix = serv.genPrefix(_sender, ERR_NEEDMOREPARAMS);
		setMsgs(_sender, _genProtoMsg(ERR_NEEDMOREPARAMS, prefix));
		return ;
	}
	// 등록되지 않은 사용자인 경우
	if (send_clnt.getIsRegistered() == false) {
		if (serv.getPassword() != _args.front()) {
			prefix = serv.genPrefix(_sender, 0);
			setMsgs(_sender, "Wrong Password for this server.\nCONNECTION ENDED BY SERVER\n");
			send_clnt.setConnected(false);
			return ;
		}
		send_clnt.setIsRegistered();
	}
	// 재등록하려는 경우
	else {
		prefix = serv.genPrefix(_sender, ERR_ALREADYREGISTRED);
		setMsgs(_sender, _genProtoMsg(ERR_ALREADYREGISTRED, prefix));
	}
}