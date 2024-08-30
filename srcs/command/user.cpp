#include "Command.hpp"

// USER <username> <hostname> <servername> <realname>

// host name and server name are normally ignored by the IRC server
// when the USER command comes from a directly connected client

// real name은 ":firstname secondname" 처럼
// 공백이 포함될 수 있어서 :로 시작을 알리고, 마지막에 위치시킴

// ERR_NEEDMOREPARAMS
// ERR_ALREADYREGISTERED

void	Command::user(int clnt_fd, Server& serv) {
	(void) clnt_fd;
	(void) serv;
}