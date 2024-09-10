#include "Command.hpp"
#include <iostream>

// NICK <nickname> [ <hopcount> ]
// hopcount는 server에서만 의미 있는 인자라서 ft_irc에서는 처리 안 함

// ERR_NONICKNAMEGIVEN: 431
// ERR_ERRONEUSNICKNAME: 432비허용 문자 포함된 겨우
// ERR_NICNAMEINUSE 433

// 우린 멀티 서버 아니라 불필요
// ERR_NICKCOLLISION: 서버를 타고 갔을 때 다른 client와 겹치는 경우

// username이 있는 경우
	// pass correct-> 연결을 허용하면서 001을 보니거나
	// pass incorrect-> 연결을 허용하지 않는다
// username이 없는 경우
	// -> update만 함 (validity 확인하고)

bool	Command::_valid_nick(std::string& new_nick) const {
	const std::string	special = "-[]\\`^{}";
	int len = new_nick.length();
	if (len < 1) // int max 길이 초과해서 overflow 발생했거나 0글자 문자열인 경우
		return (false);
	for (int i = 0; i < len; i++){
		if (!isalnum(new_nick[i]) && special.find(new_nick[i]) != std::string::npos)
			return (false);
	}
	return (true);
}

void	Command::nick(Client& send_clnt, Server& serv) {
	std::string	prefix;
	std::string msg;
	std::string	new_nick;
	if (_args.size() < 1) {
		prefix = serv.genPrefix(_sender, ERR_NEEDMOREPARAMS);
		setMsgs(_sender, _genProtoMsg(ERR_NEEDMOREPARAMS, prefix));
		return ;
	}
	new_nick = _args.front();
	// nickname 유효성 검증
	if (_valid_nick(new_nick) == false) {
		prefix = serv.genPrefix(_sender, ERR_ERRONEUSNICKNAME);
		msg = _genProtoMsg(ERR_ERRONEUSNICKNAME, prefix);
		setMsgs	(_sender, msg);
		return ;
	}
	if (serv.getClient(new_nick)) {
		prefix = serv.genPrefix(_sender, ERR_NICKNAMEINUSE);
		msg = _genProtoMsg(ERR_NICKNAMEINUSE, prefix);
		setMsgs	(_sender, msg);
		return ;
	}
	// 새로 등록
	if (send_clnt.getRegistered() == false){
		if (send_clnt.getUsername() != "*") {
			if (send_clnt.getPassValidity() == true) {
				send_clnt.setNickname(new_nick);
				prefix = serv.genPrefix(_sender, RPL_WELCOME);
				msg = _genProtoMsg(RPL_WELCOME, prefix);
			}
			else {
				msg = "ERROR :Closing Link: [Access Denied by Configuration]\n";
			}
			setMsgs(_sender, msg);
		}
		else {
			send_clnt.setRegistered();
			send_clnt.setNickname(new_nick);
		}
	}
	// 기존 사용자
	else {
		send_clnt.getRegistered();
		send_clnt.setNickname(new_nick);
	}
}