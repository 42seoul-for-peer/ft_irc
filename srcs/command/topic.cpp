#include "Command.hpp"

void Command::topic(Client& send_clnt, Server& serv) {
	//! 인자의 개수가 부족
	if (_args.size() < 1) {
		setMsgs(_send_nick, _genMsg(ERR_NEEDMOREPARAMS, _cmd));
		return ;
	}
	if (!send_clnt.getRegistered()) {
		setMsgs(_send_nick, _genMsg(ERR_NOTREGISTERED, _cmd));
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
	if (_args.size() == 0) {
		std::string chan_topic = target->second->getTopic();
		//? 지정된 topic이 존재
		if (!chan_topic.empty())
			setMsgs(_send_nick, _genMsg(RPL_TOPIC, chan_title, ":" + chan_topic));
		//? 지정된 topic이 없음
		else
			setMsgs(_send_nick, _genMsg(RPL_NOTOPIC, chan_title));
	}
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
		else {
			std::string topic_str = _appendRemaining();
			target->second->setTopic(topic_str);
			setMsgs(target->first, _genMsg(0, _cmd + " " + chan_title + " :" + topic_str));
		}
	}
}