#include "Command.hpp"

/*
ERR_NEEDMOREPARAMS
ERR_NOSUCHCHANNEL
ERR_NOTONCHANNEL
*/

void Command::part(Client& send_clnt, Server& serv)
{
	std::string prefix;

	if (_args.size() < 1)
	{
		prefix = serv.genPrefix(_sender, ERR_NEEDMOREPARAMS);
		setMsgs(_sender, _genProtoMsg(ERR_NEEDMOREPARAMS, prefix));
		return ;
	}
	std::map< std::string, Channel* >::iterator chan_it;
	std::map< std::string, Channel* > chan_list = serv.getChannels();
	std::vector<std::string> channels = _parsebyComma(_args);
	_args.pop();
	const std::string leaveMessage = _appendRemaining();
	const int channel_size = channels.size();
	for (int i = 0; i < channel_size; i++)
	{
		// 채널 먼저 찾기
		chan_it = chan_list.find(channels[i]);
		// 존재하지 않는 채널명
		if (chan_it == chan_list.end())
		{
			prefix = serv.genPrefix(_sender, ERR_NOSUCHCHANNEL);
			setMsgs(_sender, _genProtoMsg(ERR_NOSUCHCHANNEL, prefix, channels[i]));
			continue ;
		}
		// 채널은 존재하지만 클라이언트가 속해있지 않음
		std::vector< std::string > currChannelList = send_clnt.getCurrChannel();
		if (std::find(currChannelList.begin(), currChannelList.end(), chan_it->first) == currChannelList.end())
		{
			prefix = serv.genPrefix(_sender, ERR_NOTONCHANNEL);
			setMsgs(_sender, _genProtoMsg(ERR_NOTONCHANNEL, prefix, channels[i]));
			continue ;
		}
		// 정상 작동 상황
		// (1) client의 current channel list에서 삭제
		// (2) channel의 client list에서 삭제
		// (3) channel이 비게 될 경우, server에서 channel 삭제
		send_clnt.leaveChannel(*(chan_it->second));
		chan_it->second->deleteClient(send_clnt);
		if (chan_it->second->getClients().size() == 0)
		{
			serv.deleteChnl(chan_it->second);
			continue ;
		}
		else
		{
			prefix = serv.genPrefix(_sender, 0);
			std::string msg = prefix + " PART " + chan_it->first + " :" + leaveMessage;
			setMsgs(chan_it->first, msg);
		}
	}
}