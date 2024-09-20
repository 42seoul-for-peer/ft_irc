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

void Command::topic(Server& serv) {
	//! 인자의 개수가 부족
	if (_args.size() < 1) {
		setMsgs(_send_nick, _genMsg(ERR_NEEDMOREPARAMS, _cmd));
		return ;
	}

	std::string chan_title = _args.front();
	_args.pop();
	std::map< std::string, Channel* > 			serv_channels = serv.getChannels();
	std::map< std::string, Channel* >::iterator target = serv_channels.find(chan_title);

	//! 존재하지 않는 채널
	if (target == serv_channels.end()) {
		setMsgs(_send_nick, _genMsg(ERR_NOSUCHCHANNEL, chan_title));
		return ;
	}
	//todo Case; 인자 없음 -> 현재 channel의 topic 출력
	if (_args.size() == 0) {
		std::string chan_topic = target->second->getTopic();
		//? 지정된 topic이 존재
		if (!chan_topic.empty())
			setMsgs(_send_nick, _genMsg(RPL_TOPIC, chan_title, ":" + chan_topic));
		//? 지정된 topic이 없음
		else
			setMsgs(_send_nick, _genMsg(RPL_NOTOPIC, chan_title));
	}
	//todo Case; 인자 있음 -> 현재 channel의 topic 설정
	else {
		std::vector< std::pair< bool, Client* > >	chan_clnts = target->second->getClients();
		const int 									num_of_clnts = chan_clnts.size();
		int idx = 0;
		while (idx < num_of_clnts && chan_clnts[idx].second->getNickname() != _send_nick)
			idx++;
		//! 채널에 속해있지 않음
		if (idx == num_of_clnts)
			setMsgs(_send_nick, _genMsg(ERR_NOTONCHANNEL, chan_title));
		//! 채널에 속해있지만, +t이며 이 방의 op 권한을 가지고 있지 않음
		else if ((target->second->getMode() & MODE_T) && chan_clnts[idx].first == false)
			setMsgs(_send_nick, _genMsg(ERR_CHANOPRIVSNEEDED, chan_title));
		//todo TOPIC 명령 동작
		else {
			std::string topic_str = _appendRemaining();
			target->second->setTopic(topic_str);
			setMsgs(_send_nick, _genMsg(0, _cmd + " " + chan_title + " :" + topic_str));
		}
	}
}