#include "Command.hpp"

bool isExistedClient(Server& serv, const std::string& nickname) {
	std::map< int, Client* > user_list = serv.getClients();
	std::map< int, Client* >::iterator user_target = user_list.begin();

	while (user_target != user_list.end()) {
		if (user_target->second->getNickname() == nickname)
			break ;
		user_target++;
	}
	if (user_target == user_list.end())
		return (false);
	return (true);
}

void Command::invite(Client& send_clnt, Server& serv) {
	std::string prefix;
	std::string msg;

	//! 인자의 개수가 없거나 1개(nickname or channel)
	if (_args.size() <= 1)
		return ;
	//! 인자의 개수가 2개를 초과 (ex; INVITE a b c ...)
	else if (_args.size() > 2)
		return ;
	//! 클라이언트가 등록되지 않음
	if (!send_clnt.getRegistered()) {
		setMsgs(_send_nick, _genMsg(ERR_NOTREGISTERED, _cmd));
		return ;
	}
	std::string nickname = _args.front();
	_args.pop();
	std::string channel = _args.front();
	_args.pop();

	std::map< std::string, Channel* > 			serv_channels = serv.getChannels();
	std::map< std::string, Channel* >::iterator target = serv_channels.find(channel);
	//! 존재하지 않는 channel
	if (target == serv_channels.end()) {
		setMsgs(_send_nick, _genMsg(ERR_NOSUCHCHANNEL, channel));
		return ;
	}
	//! 존재하지 않는 target nickname
	if (!isExistedClient(serv, nickname)) {
		setMsgs(_send_nick, _genMsg(ERR_NOSUCHNICK, nickname));
		return ;
	}
	std::vector< std::pair< bool, Client* > >	chan_clients = target->second->getClients();
	const int 									num_of_clients = chan_clients.size();
	int idx_sender = 0;
	//! sender가 채널에 속해있지 않음
	while (chan_clients[idx_sender].second->getNickname() != _send_nick) {
		idx_sender++;
		if (idx_sender == num_of_clients) {
			setMsgs(_send_nick, _genMsg(ERR_NOTONCHANNEL, channel));
			return ;
		}
	}
	//! target 유저가 이미 채널에 속해있음
	int idx_target = 0;
	while (idx_target < num_of_clients) {
		if (chan_clients[idx_target].second->getNickname() == nickname) {
			setMsgs(_send_nick, _genMsg(ERR_USERONCHANNEL, nickname, channel));
			return ;
		}
		idx_target++;
	}
	//! sender가 채널의 op 권한을 가지고 있지 않음
	if (target->second->getMode() && chan_clients[idx_sender].first != true) {
		setMsgs(_send_nick, _genMsg(ERR_CHANOPRIVSNEEDED, channel));
		return ;
	}
	//todo INVITE 명령 동작
	target->second->addInvitedClient(nickname);
	setMsgs(_send_nick, _genMsg(RPL_INVITING, nickname, ":" + channel));
	setMsgs(nickname, _genMsg(0, _cmd + " " + nickname, ":" + channel));
}