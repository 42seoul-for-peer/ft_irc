#include <iostream>
#include "Command.hpp"

// PASS <password>

// ERR_NEEDMOREPARAMS 461
// ERR_ALREADYREGISTERED 462
// ERR_PASSWDMISMATCH 464

void	Command::pass(Client& send_clnt, Server& serv) {
	std::string			prefix;
	const std::string	send_nick = send_clnt.getNickname();
	if (_args.size() != 1) {
		prefix = serv.genPrefix(send_nick, ERR_NEEDMOREPARAMS);
		setMsgs(send_nick, _genProtoMsg(ERR_NEEDMOREPARAMS, prefix));
		return ;
	}

	if (send_clnt.getIsRegistered() == false) {
		if (serv.getPassword() != _args.front()) {
			prefix = serv.genPrefix(send_nick, ERR_PASSWDMISMATCH);
			setMsgs(send_nick, _genProtoMsg(ERR_PASSWDMISMATCH, prefix));
			return ;
		}
		send_clnt.setIsRegistered();
	}
	else {
		prefix = serv.genPrefix(send_nick, ERR_ALREADYREGISTRED);
		setMsgs(send_nick, _genProtoMsg(ERR_ALREADYREGISTRED, prefix));
	}
}