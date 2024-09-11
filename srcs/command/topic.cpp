#include "Command.hpp"

/*
? 인자가 없음
	*-> ERR_NEEDMOREPARAMS<461>
? 존재하지 않는 channel
	*-> ERR_NOSUCHCHANNEL<403> (RFC1459에는 없음)
? <channel> 인자만 전달(속해있지 않아도 됨):
	*-> RPL_TOPIC<332>, 333(RFC1459 아님) 코드와 함께 채널의 topic (+topic 지정자 정보) 출력
		! :irc.local 332 test #room1 :hi
		! :irc.local 333 sponge #room seungjun!root@127.0.0.1 :1726026075
	*-> 지정된 topic이 없을 경우: RPL_NOTOPIC<331>
		! :irc.local 331 test #room3 :No topic is set.
? 속해있지 않은 channel에 대한 topic '설정' 명령
	*-> ERR_NOTONCHANNEL<442>
? 속해있지만 +t옵션인 채널의 op가 아님
	*-> ERR_CHANOPRIVSNEEDED<482>
*/

void Command::topic(Server& serv)
{
	std::string prefix;
	std::string msg;

	//! 인자의 개수가 부족 (ERR_NEEDMOREPARAMS)
	if (_args.size() < 1)
	{
		prefix = serv.genPrefix(_sender, ERR_NEEDMOREPARAMS);
		setMsgs(_sender, _genProtoMsg(ERR_NEEDMOREPARAMS, prefix));
		return ;
	}
	std::map< std::string, Channel* > chan_list = serv.getChannels();
	std::map< std::string, Channel* >::iterator chan_target = chan_list.find(_args.front());
	//! 존재하지 않는 채널
	if (chan_target == chan_list.end())
	{
		prefix = serv.genPrefix(_sender, ERR_NOSUCHCHANNEL);
		setMsgs(_sender, _genProtoMsg(ERR_NOSUCHCHANNEL, prefix, _args.front()));
		return ;
	}
	//todo 현재 channel의 topic 출력 (Case; _args.size() == 1)
	if (_args.size() == 1)
	{
		std::string chan_topic = chan_target->second->getTopic();
		//? 지정된 topic이 존재
		if (chan_topic.empty())
		{
			prefix = serv.genPrefix(_sender, RPL_NOTOPIC);
			msg = prefix + _args.front() + ":" + chan_target->second->getTopic() + "\n";
		}
		//? 지정된 topic이 없음
		else
		{
			prefix = serv.genPrefix(_sender, RPL_TOPIC);
			msg = prefix + _args.front() + ":No topic is set.\n";
		}
		setMsgs(_sender, msg);
	}
	//todo 현재 channel의 topic 설정 (Case; _args.size() > 1)
	else
	{
		_args.pop();
		std::vector< std::pair< bool, Client* > > client_list = chan_target->second->getClients();
		const int number_of_clients = client_list.size();
		int client_idx = 0;
		//todo client_list에서 _sender의 idx 탐색
		//! 채널에 속해있지 않음
		while (client_list[client_idx].second->getNickname() != _sender)
		{
			client_idx++;
			if (client_idx == number_of_clients)
			{
				prefix = serv.genPrefix(_sender, ERR_NOTONCHANNEL);
				setMsgs(_sender, _genProtoMsg(ERR_NOTONCHANNEL, prefix));
				return ;
			}
		}
		//! 채널에 속해있지만, +t이며 이 방의 op 권한을 가지고 있지 않음
		if ((chan_target->second->getMode() & MODE_T) && client_list[client_idx].first != true)
		{
			prefix = serv.genPrefix(_sender, ERR_CHANOPRIVSNEEDED);
			setMsgs(_sender, _genProtoMsg(ERR_CHANOPRIVSNEEDED, prefix));
		}
		//todo TOPIC 명령 동작
		else
		{
			std::string topic_str = _appendRemaining();
			chan_target->second->setTopic(topic_str);
			prefix = serv.genPrefix(_sender, 0);
			// :kkk!a@127.0.0.1 TOPIC #room10 :hi
			msg = prefix + " TOPIC " + chan_target->first + " :" + topic_str + "\n";
			setMsgs(chan_target->first, msg);
		}
	}
}