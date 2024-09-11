#include "Command.hpp"
#include <iostream>

// USER <username> <servername> <servername> <realname>

// host name and server name are normally ignored by the IRC server
// when the USER command comes from a directly connected client

// real name은 ":firstname secondname" 처럼
// 공백이 포함될 수 있어서 :로 시작을 알리고, 마지막에 위치시킴

/*
? 해줘야 하는 것
* clnt_fd를 기준으로 Client를 find 해서
* 해당 Client 객체에 username을 저장,
! 처리해야 하는 err replies
* (1) ERR_NEEDMOREPARAMS<461>	: 클라이언트가 충분한 파라미터를 전달하지 않음
* (2) ERR_ALREADYREGISTERED<462>: 이미 등록된 클라이언트의 등록 정보 재정의 시도
*/

bool	Command::_valid_user(std::string& new_user) const {
	if (new_user.find('\n') != std::string::npos || new_user.find('\0') != std::string::npos
		|| new_user.find('\r') != std::string::npos || new_user.find(' ') != std::string::npos)
		return false;
	return true;
}

void	Command::user(Client& send_clnt, Server& serv) {
	std::string	prefix;
	std::string	msg;
	std::string new_user;

	if (_args.size() < 4){
		(void) serv;
		msg = _genMsg(ERR_NEEDMOREPARAMS, _cmd);
		setMsgs(_sender, msg);
		return ;
	}
	// 중복 등록 방지
	if (send_clnt.getUsername() != "*") {
		msg = _genMsg(ERR_ALREADYREGISTRED);
		setMsgs(_sender, msg);
		return ;
	}
	new_user = _args.front();
	_args.pop();
	if (_valid_user(new_user) == false) {
		return ; // inspircd의 경우 자체 error num을 가지고 있긴 함
	}
	// nick name이 있는 경우
	if (send_clnt.getNickname() != "*") {
		if (send_clnt.getPassValidity() == true) {
			send_clnt.setUsername(new_user);
			msg = _genMsg(RPL_WELCOME);
			send_clnt.setRegistered();
		}
		else {
			msg = "ERROR :Closing Link: [Access Denied by Configuration]\n";
		}
		setMsgs(_sender, msg);
	}
	else {
		send_clnt.setUsername(new_user);
	}
}
