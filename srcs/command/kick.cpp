#include "Command.hpp"


//       Command: KICK
//    Parameters: <channel> <user> [<comment>]
void Command::kick(Client& send_clnt, Server& serv) {
	(void) send_clnt;
	(void) serv;

	std::string prefix;
	std::string channel;
	std::string target;
	std::string comment;
	
	if (_args.size() < 2) {
		prefix = serv.generatePrefix(_sender, ERR_NEEDMOREPARAMS)
		msg = _genProtoMsg(ERR_NEEDMOREPARAMS, prefix);
		setMsgs(_sender, msg);
		return ;
	}

	channel = _args.front();
	_args.pop();
	target = _args.front();
	_args.pop();

	if (_args.empty())
		comment = _sender;
	while (!_args.empty()) {
		comment += _args.front();
		if (_args.size() > 1)
			comment += " ";
		_args.pop();
	}

	if (comment[0] == ':' && comment.size() > 1) {
		comment = comment.substr(1);

	
	//첫 인자 채널 검색해보고 없으면 ERR_nosuchchan
	// KICK #hihi nick
	// :irc.local 403 cat #hihi :No such channel

	// 센더가 그 채널에 있는지 보고 아니면 ERR_NOTONCHANNEL
	// KICK #channelll nick
	// :irc.local 442 cat #channelll :You're not on that channel!

	// 센더가 그 채널의 op인지 보고 아니면 ERR_CHANOPRIVSNEEDED
	// 	KICK #channelll nick
	// :irc.local 482 cat #channelll :You must be a channel op or higher to kick a more privileged user.

	//두번째 인자 유저 서버에서 검색해보고 없으면 nosuch nick

	//두번째 인자 유저를 위 채널에서 검색해보고 없으면 ERR_USERNOTINCHANNEL

	//채널이 있고, 센더가 그 채널에 있고, 오퍼레이터이면서, 타겟 유저가 서버에 있고 채널에도 있다면...
	//나머지 args 들 다 msg content로 묶기
}