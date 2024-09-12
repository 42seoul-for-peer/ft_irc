#include "Command.hpp"
/*
O (1) ERR_NEEDMOREPARAMS<461>   -> "<command> :Not enough parameters"
//(2) ERR_BANNEDFROMCHAN<474>   -> "<channel> :Cannot join channle (+b)" (구현 불필요)
O (3) ERR_INVITEONLYCHAN<473>   -> "<channel> :Cannot join channel (+i)"
O (4) ERR_BADCHANNELKEY<475>    -> "<channel> :Cannot join channel (+k)"
O (5) ERR_CHANNELISFULL<471>    -> "<channel> :Cannot join channel (+l)"
//(6) ERR_BADCHANMASK<476>      -> No Manual in RFC 1459 (:irc.local 476 what asdf :Invalid channel name)
O (7) ERR_NOSUCHCHANNEL<403>    -> "<channel name> :No such channel"
O (8) ERR_TOOMANYCHANNELS<405>  -> "<channel name> :You have joined too many channels"
* (9) RPL_TOPIC<332>            -> "<channel> :<topic>" 채널의 topic을 결정하기 위해 TOPIC 메시지를 보낼 때,
*                                                       topic이 설정되면 RPL_TOPIC을 보내고 그렇지 않으면 RPL_NOTOPIC을 보냅니다.
* (11) RPL_NAMREPLY<353>		-> "<channel> :[[@|+]<nick> [[@|+]<nick> [...]]]"
* (12) RPL_ENDOFNAMES<366>		-> "<channel> :End of /NAMES list"

* 채널의 이름은 '&' 또는 '#'로 시작하는 200자 이하의 string입니다.
* 첫 번째 문자가 '&' 또는 '#' 이어야 한다는 것 외의 요구사항은
* 공백, ctrl+G, ','를 포함할 수 없다는 것입니다.
* &: 모든 서버에 알려진 분산 채널
*/

#include <iostream>

void Command::join(Client& send_clnt, Server& serv)
{
	if (_args.size() < 1)
	{
		setMsgs(_sender, _genMsg(ERR_NEEDMOREPARAMS, _cmd));
		return ;
	}

	std::vector<std::string>	titles = _parsebyComma();
	const int					title_size = titles.size();
	_args.pop();
	std::vector<std::string>	passwords = _parsebyComma();
	const int					passwd_size = passwords.size();
	// _args.pop();
	std::map< std::string, Channel* > 			channels = serv.getChannels();
	std::map< std::string, Channel* >::iterator	chan_it;

	for (int i = 0; i < title_size; i++)
	{
		//! 현재 접속한 채널의 수가 10개 이상
		if (send_clnt.getCurrChannel().size() >= 10)
		{
			setMsgs(_sender, _genMsg(ERR_TOOMANYCHANNELS, titles[i]));
			continue ;
		}
		//! title의 첫 글자가 #가 아니거나, title이 200자를 초과
		if (titles[i][0] != '#' || titles[i].size() > 200)
		{
			setMsgs(_sender, _genMsg(ERR_NOSUCHCHANNEL, titles[i]));
			continue ;
		}
		//? title이 존재하는 채널인지 탐색
		chan_it = channels.find(titles[i]);
		//todo 채널이 존재하지 않음(신규 생성)
		if (chan_it == channels.end())
		{
			Channel* new_channel = new Channel(titles[i], &send_clnt);
			serv.addNewChnl(new_channel);
			setMsgs(titles[i], _genPrefix(0) + "JOIN :" + titles[i] + "\n");
			setMsgs(_sender, _genMsg(RPL_NAMREPLY, "= " + titles[i], ":@" + new_channel->printClientsList()));
			setMsgs(_sender, _genMsg(RPL_ENDOFNAMES, titles[i]));
		}
		//todo 채널이 존재함 (접속 시도, 비밀번호 확인 필요)
		else
		{
			//! 이미 접속중인 채널(출력 없음)
			std::vector< std::string > curr_chan = send_clnt.getCurrChannel();
			if (std::find(curr_chan.begin(), curr_chan.end(), titles[i]) != curr_chan.end())
				continue ;
			const int chan_mode = chan_it->second->getMode();
			std::vector< std::string > invited_list = chan_it->second->getInvitedClients();
			//! Case 'k'; 인자로 입력받은 key 값이 없거나 비밀번호가 다름
			if (chan_mode & MODE_K && \
					(i > passwd_size - 1 || passwords[i] != chan_it->second->getPasswd()))
			{
				setMsgs(_sender, _genMsg(ERR_BADCHANNELKEY, titles[i]));
				continue ;
			}
			//! Case 'i'; invited_list에 send_clnt의 username이 존재하지 않음
			if (chan_mode & MODE_I && \
					std::find(invited_list.begin(), invited_list.end(), send_clnt.getUsername()) == invited_list.end())
			{
				setMsgs(_sender, _genMsg(ERR_BADCHANNELKEY, titles[i]));
				continue ;
			}
			//! Case 'l'; 현재 접속 유저 수가 channel의 최대 유저 수와 같거나 큼('i'가 활성화 됐을 때, 'l'은 무시됨)
			if (chan_mode & MODE_L && !(chan_mode & MODE_I) && \
						static_cast<int>(chan_it->second->getClients().size()) >= chan_it->second->getMaxClients())
			{
				setMsgs(_sender, _genMsg(ERR_CHANNELISFULL, titles[i]));
				break ;
			}
			// invite 채널도 아니고, 최대 접속 제한도 아님 (자유롭게 접속 가능)
			if (chan_mode & MODE_I)
				invited_list.erase(std::find(invited_list.begin(), invited_list.end(), send_clnt.getUsername()));
			chan_it->second->addClient(std::make_pair(false, &send_clnt));
			setMsgs(titles[i], _genPrefix(0) + "JOIN :" + titles[i] + "\n");
			if (!chan_it->second->getTitle().empty())
				setMsgs(_sender, _genMsg(RPL_TOPIC, titles[i], ":" + chan_it->second->getTitle()));
			setMsgs(_sender, _genMsg(RPL_NAMREPLY, "= " + titles[i], ":@" + chan_it->second->printClientsList()));
			setMsgs(_sender, _genMsg(RPL_ENDOFNAMES, titles[i]));
		}
	}    
}