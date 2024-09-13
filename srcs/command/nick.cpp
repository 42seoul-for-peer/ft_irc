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
	std::string msg;
	std::string	new_nick;

	if (_args.size() < 1) {
		setMsgs(_send_nick, _genMsg(ERR_NEEDMOREPARAMS, _cmd));
		return ;
	}
	new_nick = _args.front();
	// 유효성 검증1: 사용 가능한 문자로 구성되어 있는가
	if (_valid_nick(new_nick) == false) {
		setMsgs	(_send_nick, _genMsg(ERR_ERRONEUSNICKNAME));
		return ;
	}
	// 유효성 검증2: 중복되지 않는가
	if (serv.getClient(new_nick)) {
		setMsgs	(_send_nick, _genMsg(ERR_NICKNAMEINUSE, new_nick));
		return ;
	}
	// 닉네임 변경하는 경우
	if (send_clnt.getRegistered() == true) {
		send_clnt.setNickname(new_nick);
	}
	else {	// 새로 등록하는 경우
		if (send_clnt.getUsername() != "*") { // user name 있는 경우
			if (send_clnt.getPassValidity() == true) { // 비밀번호가 맞음
				send_clnt.setNickname(new_nick);
				send_clnt.setRegistered();
				_send_nick = new_nick;
				msg = _genMsg(RPL_WELCOME, new_nick);
			}
			else { // 비밀번호가 틀려서 종료
				msg = "ERROR :Closing Link: [Access Denied by Configuration]\n";
				send_clnt.setConnected(false);
			}
			setMsgs(_send_nick, msg);
		}
		else { // user name이 아직 없어서 등록 확인을 하지 않음
			send_clnt.setNickname(new_nick);
		}
	}
}