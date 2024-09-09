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

void	Command::user(Client& send_clnt, Server& serv) {
	std::string	prefix;
	std::string send_nick = send_clnt.getNickname();
	// PASS, NICK 명령에서 문제가 발생 -> _msgs에 해당 클라이언트가 있음
	if (_msgs.find(send_nick)!=_msgs.end()){
		return ;
	}
	// 클라이언트가 충분한 파라미터를 전달하지 않음
	if (_args.size() < 4)
	{
		prefix = serv.genPrefix(send_nick, ERR_NEEDMOREPARAMS);
		setMsgs(send_nick, _genProtoMsg(ERR_NEEDMOREPARAMS, prefix));
		return ;
	}
	// 이미 등록된 클라이언트임 (현재 판단 조건: username이 이미 존재함)
	if (!send_clnt.getUsername().empty())
	{
		prefix = serv.genPrefix(send_nick, ERR_ALREADYREGISTRED);
		setMsgs(send_nick, _genProtoMsg(ERR_ALREADYREGISTRED, prefix));
		return ;
	}
	send_clnt.setUsername(_args.front());
	prefix = serv.genPrefix(send_nick, RPL_WELCOME);
	setMsgs(send_nick, _genProtoMsg(RPL_WELCOME, prefix));
}
