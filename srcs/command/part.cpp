#include "Command.hpp"

void Command::part(Client& send_clnt, Server& serv)
{
	if (_args.size() < 1)
	{
		setMsgs(_send_nick, _genMsg(ERR_NEEDMOREPARAMS, _cmd));
		return ;
	}

	std::map< std::string, Channel* > 			serv_channels = serv.getChannels();
	std::vector< std::string >					clnt_channels = send_clnt.getCurrChannel();
	std::map< std::string, Channel* >::iterator	target;

	std::vector<std::string>	chan_args = _parsebyComma();
	const int					chan_size = chan_args.size();
	_args.pop();
	const std::string			leaveMessage = _appendRemaining();

	for (int i = 0; i < chan_size; i++)
	{
		target = serv_channels.find(chan_args[i]);
		//! 존재하지 않는 채널명
		if (target == serv_channels.end())
		{
			setMsgs(_send_nick, _genMsg(ERR_NOSUCHCHANNEL, chan_args[i]));
			continue ;
		}
		//! 채널은 존재하지만 클라이언트가 속해있지 않음
		if (std::find(clnt_channels.begin(), clnt_channels.end(), target->first) == clnt_channels.end())
		{
			setMsgs(_send_nick, _genMsg(ERR_NOTONCHANNEL, chan_args[i]));
			continue ;
		}
		send_clnt.leaveChannel(*(target->second));
		target->second->deleteClient(send_clnt);
		std::string msg;
		if (leaveMessage.empty())
			msg = _genMsg(0, _cmd, target->first);
		else 
			msg = _genMsg(0, _cmd + " " + target->first, ":" + leaveMessage);
		setMsgs(_send_nick, msg);
		if (target->second->getClients().size() == 0)
			serv.deleteChnl(target->second);
		else
			setMsgs(target->first, msg);
	}
}