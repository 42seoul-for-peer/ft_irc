#include "Command.hpp"

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
	if (_confirmed_args.empty()) {
		_rpl_no = 451;
		_proto_msg = ":You have not registerd";
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
	for (int i = 0; i < newNick.length(); i++) {
		if (newNick[i] == '#') {
			_rpl_no = 432;
			_proto_msg = newNick + ":Erroneus nickname";
			return ;
		}
	}
	std::map< int, Client* >::iterator it = serv.getClients().begin();
	while (it != serv.getClients().end()) {
		if (it->second->getNickname() == newNick) {
			_rpl_no = 433;
			_proto_msg = newNick + ":Nickname is already in use";
			return ;
		}
		it++;
	}
	_confirmed_args.push(newNick);
}