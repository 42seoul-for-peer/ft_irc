#include "Command.hpp"
#include <algorithm>

std::vector<std::string> parsebyComma(std::string& line) { // 이거 Command 멤버변수랑 구조가 달라서 일단 임시로 여기 둠
    std::vector<std::string>    token_vec;
    std::stringstream           stream(line);
    std::string                 token;

    while (std::getline(stream, token, ','))
        token_vec.push_back(token);
    return (token_vec);
}

void Command::kick(Server& serv) {

	std::string prefix;
	std::string msg;
	std::string channel;
	std::vector< std::string > targets;
	std::string target;
	std::string comment;

	Client* kicked_client;
	
	if (_args.size() < 2) {
		prefix = serv.genPrefix(_sender, ERR_NEEDMOREPARAMS);
		msg = _genProtoMsg(ERR_NEEDMOREPARAMS, prefix);
		setMsgs(_sender, msg);
		return ;
	}

	channel = _args.front();
	_args.pop();
	targets = parsebyComma(_args.front());
	_args.pop();
	comment = _appendRemaining();

	//첫 인자 채널 검색해보고 없으면 ERR_nosuchchan
	// KICK #hihi nick
	// :irc.local 403 cat #hihi :No such channel
	std::map< std::string, Channel* >::const_iterator channel_it = serv.getChannels().begin();
	Channel* channel_addr;

	while (channel_it != serv.getChannels().end()) {
		if (channel_it->first == channel) {
			channel_addr = channel_it->second; 
			break ;
		}
		channel_it ++;
	}
	if (channel_it == serv.getChannels().end()) {
		prefix = serv.genPrefix(_sender, ERR_NOSUCHCHANNEL);
		msg = _genProtoMsg(ERR_NOSUCHCHANNEL, prefix, channel);
		setMsgs(_sender, msg);
		return ;
	}

	// 센더가 그 채널에 있는지 보고 아니면 ERR_NOTONCHANNEL
	// KICK #channelll nick
	// :irc.local 442 cat #channelll :You're not on that channel!

	// 센더가 그 채널의 op인지 보고 아니면 ERR_CHANOPRIVSNEEDED
	// 	KICK #channelll nick
	// :irc.local 482 cat #channelll :You must be a channel op or higher to kick a more privileged user.

	std::vector< std::pair< bool, Client* > >::const_iterator channel_member_it;

	channel_member_it = channel_addr->getClients().begin();
	while (channel_member_it != channel_addr->getClients().end()) {
		if (_sender == channel_member_it->second->getNickname()) {
			if (channel_member_it->first == 0) {
				prefix = serv.genPrefix(_sender, ERR_CHANOPRIVSNEEDED);
				msg = _genProtoMsg(ERR_CHANOPRIVSNEEDED, prefix, channel);
				setMsgs(_sender, msg);
				return ;
			}
			break ;
		}
		channel_member_it++;
	}
	if (channel_member_it == channel_addr->getClients().end()) {
		prefix = serv.genPrefix(_sender, ERR_NOTONCHANNEL);
		msg = _genProtoMsg(ERR_NOTONCHANNEL, prefix, channel);
		setMsgs(_sender, msg);
		return ;
	}

	//두번째 인자 유저 서버에서 검색해보고 없으면 nosuch nick
	std::vector< std::string >::iterator targets_it = targets.begin();
	while (targets_it != targets.end()) {
		target = *targets_it;
		targets_it++;

		std::map< int, Client* >::const_iterator serv_clnt_it = serv.getClients().begin();
		while (serv_clnt_it != serv.getClients().end()) {
			if (target == serv_clnt_it->second->getNickname()) {
				kicked_client = serv_clnt_it->second;
				break ;
			}
			serv_clnt_it++;
		}
		if (serv_clnt_it == serv.getClients().end()) {
			prefix = serv.genPrefix(_sender, ERR_NOSUCHNICK);
			msg = _genProtoMsg(ERR_NOSUCHNICK, prefix, target);
			setMsgs(_sender, msg);
			continue ;
		}

		//두번째 인자 유저를 위 채널에서 검색해보고 없으면 ERR_USERNOTINCHANNEL
		std::vector< std::string > curr_channel = serv_clnt_it->second->getCurrChannel();
		std::vector< std::string >::iterator it = find(curr_channel.begin(), curr_channel.end(), channel);
		if (it == curr_channel.end()) {
			prefix = serv.genPrefix(_sender, ERR_USERNOTINCHANNEL);
			msg = _genProtoMsg(ERR_USERNOTINCHANNEL, prefix, target, channel);
			setMsgs(_sender, msg);
			continue ;
		}

		//채널이 있고, 센더가 그 채널에 있고, 오퍼레이터이면서, 타겟 유저가 서버에 있고 채널에도 있다면...
		prefix = serv.genPrefix(_sender, 0);
		msg = prefix + " " + _cmd + " " + channel + " " + target;
		if (comment.size() != 0)
			msg += " :" + comment + "\n";
		else
			msg += "\n";
		setMsgs(target, msg);
		setMsgs(channel, msg);

		kicked_client->leaveChannel(*channel_addr);
		channel_addr->deleteClient(*kicked_client);
	}

	if (channel_addr->getClients().size() == 0) {
		serv.deleteChnl(channel_addr);
	}
	return ;
	
}