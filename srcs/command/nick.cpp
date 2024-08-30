#include "Command.hpp"
#include <iostream>

// NICK <nickname> [ <hopcount> ]
// hopcount는 server에서만 의미 있는 인자라서 ft_irc에서는 처리 안 함

// ERR_NOTREGISTERED: PASS 없이 온 경우

// ERR_NONICKNAMEGIVEN:
// ERR_ERRONEUSNICKNAME: 비허용 문자 포함된 겨우
// ERR_NICNAMEINUSE

// 우린 멀티 서버 아니라 괜찮을듯?
// ERR_NICKCOLLISION: 서버를 타고 갔을 때 다른 client와 겹치는 경우

void	Command::nick(int clnt_fd, Server& serv) {
	// 비밀번호 있는지 확인해야함
	std::map< int, Client* >::const_iterator it = serv.getClients().find(clnt_fd);
	if (it == serv.getClients().end() || it->second->getIsRegistered() == false) {
		_rpl_no = 451;
		_proto_msg = ":You have not registerd";
		std::cout << _proto_msg << std::endl;
		return ;
	}
	if (_args.size() < 1) {
		_rpl_no = 431; // no nickname given
		_proto_msg = ":No nickname given";
		return ;
	}
	// 인자가 한 개보다 많은 경우는 무시되나? (프로토콜 뜯어보기)
	std::string	newNick =_args.front();
	_args.pop();
	int	new_nick_len = newNick.length();
	for (int i = 0; i < new_nick_len; i++) {
		if (newNick[i] == '#') {
			_rpl_no = 432;
			_proto_msg = newNick + ":Erroneus nickname";
			return ;
		}
	}
	// 중복되지 않는지 확인하는 상황
	while (it != serv.getClients().end()) {
		if (it->second->getNickname() == newNick) {
			_rpl_no = 433;
			_proto_msg = newNick + ":Nickname is already in use";
			return ;
		}
		it++;
	}
	it->second->setNickname(newNick);
}