#include "Command.hpp"
#include <iostream>

// NICK <nickname> [ <hopcount> ]
// hopcount는 server에서만 의미 있는 인자라서 ft_irc에서는 처리 안 함

// ERR_NOTREGISTERED: 451 PASS 없이 온 경우

// ERR_NONICKNAMEGIVEN: 431
// ERR_ERRONEUSNICKNAME: 432비허용 문자 포함된 겨우
// ERR_NICNAMEINUSE 433

// 우린 멀티 서버 아니라 불필요
// ERR_NICKCOLLISION: 서버를 타고 갔을 때 다른 client와 겹치는 경우

// invalid character
// ~!@#$%&*()_+-=
# define INVALID_CHARS "~!@#$%&*()_+-="

void	Command::nick(Client& send_clnt, Server& serv) {
	// 비밀번호 있는지 확인해야함
	if (send_clnt.getIsRegistered() == false) {
		_receiver.push(make_pair(send_clnt.getNickname(), ERR_NOTREGISTERED));
		_msg = "You have not registerd";
		return ;
	}
	// 비밀번호가 같이 안 들어온 경우
	if (_args.size() < 1) {
		_receiver.push(make_pair(send_clnt.getNickname(), ERR_NONICKNAMEGIVEN));
		_msg = "No nickname given";
		return ;
	}
	// nickname으로 쓸 수 없는 문자 들어있는지 검사
	std::string invalid_chars("~!@#$%&*()_+-=");
	std::string	newNick =_args.front();
	_args.pop();
	int	new_nick_len = newNick.length();
	for (int i = 0; i < new_nick_len; i++) {
		if (invalid_chars.find(newNick[i])) {
			_receiver.push(make_pair(send_clnt.getNickname(), ERR_ERRONEUSNICKNAME));
			_msg = "Erroneus nickname";
			return ;
		}
	}
	// 중복되지 않는지 확인하는 상황
	std::map< int, Client* >::const_iterator it = serv.getClients().begin();
	while (it != serv.getClients().end()) {
		if (it->second->getNickname() == newNick) {
			_receiver.push(make_pair(send_clnt.getNickname(), ERR_NICKNAMEINUSE));
			_msg = "Nickname is already in use";
			return ;
		}
		it++;
	}
	send_clnt.setNickname(newNick);
}