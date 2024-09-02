#include "Command.hpp"

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

void	Command::user(int clnt_fd, Server& serv) {
	// PASS, NICK 명령에서 _rpl_no가 설정됨
	if (_rpl_no != 0)
		return ;
	// 클라이언트가 충분한 파라미터를 전달하지 않음
	if (_args.size() < 4)
	{
		_rpl_no = ERR_NEEDMOREPARAMS;
		_proto_msg = "USER :Not enough parameters";
		return ;
	}
	// 이미 등록된 클라이언트임 (현재 판단 조건: username이 이미 존재함)
	std::map< int, Client* >::const_iterator it = serv.getClients().find(clnt_fd);
	if (!it->second->getUsername().empty())
	{
		_rpl_no = ERR_ALREADYREGISTRED;
		_proto_msg= ":You may not reregister";
		return ;
	}
	it->second->setUsername(_args.front());
}
