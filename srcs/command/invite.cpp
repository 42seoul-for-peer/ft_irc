#include "Command.hpp"
/*
? 인자가 없거나 1개
	*-> 337 (RFC 1459 아님)
	":irc.local 337 test1 :End of INVITE list"
	*-> ERR_NEEDMOREPARAMS<461> 가 뜨지 않는다
? 인자가 3개 이상
	":irc.local NOTICE nickname :*** Invalid duration for invite"
? 존재하지 않는 채널
	*-> ERR_NOSUCHCHANNEL<403> (INVITE 매뉴얼에는 없음)
	":irc.local 403 [_sender] [channel] :No such channel"
? 존재하지 않는 닉네임
	*-> ERR_NOSUCHNICK<401>
	":irc.local 401 [_sender] [nickname] :No such nick"
? _sender가 채널에 속해있지 않음
	*-> ERR_NOTONCHANNEL<442>
? 이미 유저가 채널에 있음
	*-> ERR_USERONCHANNEL<443>
	":irc.local 443 [_sender] [nickname] [channel] :is already on channel"
? 채널의 op가 아님
	*-> ERR_CHANOPRIVSNEEDED<482>
	":irc.local 482 [_sender] [channel] :You must be a channel op or higher to send an invite."

! 유의사항
	* "INVITE <nickname> <channel>" 의 형태로 다른 명령어와 순서가 반대
	* nickname과 channel은 무조건 하나씩 받음
	* args 개수에 따른 rpl_no 처리가 이상함
	* 복합적인 에러 상황에서의 처리 순서
		! ERR_NOSUCHCHANNEL -> ERR_NOSUCHNICK -> ERR_NOTONCHANNEL -> ERR_USERONCHANNEL -> ERR_CHANOPRRIVSNEEDED
todo
	*-> 초대가 왔다는 표시-> :[_sender]!a@127.0.0.1 INVITE [nickname] :[channel]
	*-> 초대를 보냈다는 표시-> :irc.local 341 [_sender] [nickname] :[channel]
	*-> 341     RPL_INVITING
*/

bool isExistedClient(Server& serv, const std::string& nickname)
{
	std::map< int, Client* > user_list = serv.getClients();
	std::map< int, Client* >::iterator user_target = user_list.begin();

	while (user_target != user_list.end())
	{
		if (user_target->second->getNickname() == nickname)
			break ;
		user_target++;
	}
	if (user_target == user_list.end())
		return (false);
	return (true);
}

void Command::invite(Server& serv)
{
	std::string prefix;
	std::string msg;

	//! 인자의 개수가 없거나 1개(nickname or channel)
	if (_args.size() <= 1)
	{
		// RFC 1459에 없는 rpl_no
		// ERR_NEEDMOREPARAMS<461>이 안뜬다..?
		return ;
	}
	//! 인자의 개수가 2개를 초과 (ex; INVITE a b c ...)
	else if (_args.size() > 2)
	{
		// rpl_no 형태가 아닌 NOTICE 형태의 protocol message
		return ;
	}
	// todo channel->invite_list에 nickname 추가
	std::string nickname = _args.front();
	_args.pop();
	std::string channel = _args.front();
	_args.pop();

	std::map< std::string, Channel* > chan_list = serv.getChannels();
	std::map< std::string, Channel* >::iterator chan_target = chan_list.find(channel);
	//! 존재하지 않는 channel
	if (chan_target == chan_list.end())
	{
		prefix = serv.genPrefix(_sender, ERR_NOSUCHCHANNEL);
		setMsgs(_sender, _genProtoMsg(ERR_NOSUCHCHANNEL, prefix, channel));
		return ;
	}
	//! 존재하지 않는 target nickname
	if (!isExistedClient(serv, nickname))
	{
		prefix = serv.genPrefix(_sender, ERR_NOSUCHNICK);
		setMsgs(_sender, _genProtoMsg(ERR_NOSUCHNICK, prefix, nickname));
		return ;
	}
	//! sender가 채널에 속해있지 않음
	std::vector< std::pair< bool, Client* > > client_list = chan_target->second->getClients();
	const int number_of_clients = client_list.size();
	int sender_idx = 0;
	while (client_list[sender_idx].second->getNickname() != _sender)
	{
		sender_idx++;
		if (sender_idx == number_of_clients)
		{
			prefix = serv.genPrefix(_sender, ERR_NOTONCHANNEL);
			setMsgs(_sender, _genProtoMsg(ERR_NOTONCHANNEL, prefix));
			return ;
		}
	}
	//! target 유저가 이미 채널에 속해있음
	int target_idx = 0;
	while (target_idx < number_of_clients)
	{
		if (client_list[target_idx].second->getNickname() == nickname)
		{
			prefix = serv.genPrefix(_sender, ERR_USERONCHANNEL);
			setMsgs(_sender, _genProtoMsg(ERR_USERONCHANNEL, prefix, nickname, channel));
			return ;
		}
		target_idx++;
	}
	//! sender가 채널의 op 권한을 가지고 있지 않음
	if (chan_target->second->getMode() && client_list[sender_idx].first != true)
	{
		prefix = serv.genPrefix(_sender, ERR_CHANOPRIVSNEEDED);
		setMsgs(_sender, _genProtoMsg(ERR_CHANOPRIVSNEEDED, prefix));
	}
	//todo INVITE 명령 동작
	chan_target->second->addInvitedClient(nickname);
	prefix = serv.genPrefix(_sender, RPL_INVITING);
	msg = prefix + " " + nickname + " :" + channel + "\n";
	//* sender에게 보내는 메시지
	setMsgs(_sender, msg);
	prefix = serv.genPrefix(_sender, 0);
	msg = prefix + " INVITE " + nickname + " :" + channel + "\n";
	//* nickname(target)에게 보내는 메시지
	setMsgs(nickname, msg);
}