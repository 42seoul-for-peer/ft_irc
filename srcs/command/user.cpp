#include "Command.hpp"

// USER <username> <servername> <servername> <realname>

// host name and server name are normally ignored by the IRC server
// when the USER command comes from a directly connected client

// real name은 ":firstname secondname" 처럼
// 공백이 포함될 수 있어서 :로 시작을 알리고, 마지막에 위치시킴

// ERR_NEEDMOREPARAMS
// ERR_ALREADYREGISTERED

/*
std::string					_cmd;
std::queue< std::string >	_args;
std::queue< std::string >	_confirmed_args; // 검사된 인자들

std::string					_sender;
int							_receiver_cnt;
std::vector< std::string >	_receiver;
std::string					_proto_msg;
int							_rpl_no;
*/

/*
? 해줘야 하는 것
* clnt_fd를 기준으로 Client를 find 해서
* 해당 Client 객체에 username을 저장,
! 처리해야 하는 err replies
* (1) ERR_NEEDMOREPARAMS<461>	: 클라이언트가 충분한 파라미터를 전달하지 않음
* (2) ERR_ALREADYREGISTERED<462>: 이미 등록된 클라이언트의 등록 정보 재정의 시도
*/
void	Command::user(int clnt_fd, Server& serv) {
	(void) clnt_fd;
	(void) serv;
}
