#include "Command.hpp"
#include <algorithm>

std::vector<std::string> parsebyComma(std::string& line) {
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
		setMsgs(_send_nick, _genMsg(ERR_NEEDMOREPARAMS));
		return ;
	}

	channel = _args.front();
	_args.pop();
	targets = parsebyComma(_args.front());
	_args.pop();
	comment = _appendRemaining();

	// 없는 채널
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
		setMsgs(_send_nick, _genMsg(ERR_NOSUCHCHANNEL, channel));
		return ;
	}

	// 센더가 그 채널에 있는지, 센더가 그 채널의 op인지 확인
	std::vector< std::pair< bool, Client* > >::const_iterator channel_member_it;

	channel_member_it = channel_addr->getClients().begin();
	while (channel_member_it != channel_addr->getClients().end()) {
		if (_send_nick == channel_member_it->second->getNickname()) {
			if (channel_member_it->first == 0) {
				setMsgs(_send_nick, _genMsg(ERR_CHANOPRIVSNEEDED, channel));
				return ;
			}
			break ;
		}
		channel_member_it++;
	}
	if (channel_member_it == channel_addr->getClients().end()) {
		setMsgs(_send_nick, _genMsg(ERR_NOTONCHANNEL, channel));
		return ;
	}

	// 서버에 존재하는 유저인지
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
			setMsgs(_send_nick, _genMsg(ERR_NOSUCHNICK, target));
			continue ;
		}

		// 채널 내 존재하는 유저인지
		std::vector< std::string > curr_channel = serv_clnt_it->second->getCurrChannel();
		std::vector< std::string >::iterator it = find(curr_channel.begin(), curr_channel.end(), channel);
		if (it == curr_channel.end()) {
			setMsgs(_send_nick, _genMsg(ERR_USERNOTINCHANNEL, target, channel));
			continue ;
		}

		//kick 정상적으로 수행
		if (comment.size() != 0)
			msg = _genMsg(0, _cmd, channel + " " + target + " :" + comment);
		else
			msg = _genMsg(0, _cmd, channel + " " + target + " :" + _send_nick);
		
		std::vector< std::pair< bool, Client* > >::const_iterator channel_member_it = channel_addr->getClients().begin();
		while (channel_member_it != channel_addr->getClients().end()) {
			setMsgs(channel_member_it->second->getNickname(), msg);
			channel_member_it++;
		}

		kicked_client->leaveChannel(*channel_addr);
		channel_addr->deleteClient(*kicked_client);
	}

	if (channel_addr->getClients().size() == 0) {
		serv.deleteChnl(channel_addr);
	}
	return ;
	
}