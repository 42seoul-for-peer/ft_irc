#include <iostream>
#include <set>
#include "Command.hpp"

void Command::quit(Server& serv) {
	std::string prefix;
	std::string msg;

	std::map< int, Client* > 			serv_clients = serv.getClients();
	std::map< int, Client* >::iterator	clnt_target = serv_clients.begin();
	while (clnt_target != serv_clients.end()) {
		if (clnt_target->second->getNickname() == _send_nick)
			break ;
		clnt_target++;
	}
	//! sender가 서버 clients에 등록되지 않음(불가능)
	if (clnt_target == serv_clients.end())
		return ;
	Client* clnt = clnt_target->second;
	std::map< std::string, Channel* > 	serv_channels = serv.getChannels();
	std::vector< std::string > 			clnt_channels = clnt->getCurrChannel();
	const int 							clnt_chan_size = clnt_channels.size();

	std::set< std::string > related_clients;
	for (int i = 0; i < clnt_chan_size; i++) {
		std::map< std::string, Channel* >::iterator chan_target = serv_channels.find(clnt_channels[i]);
		//! channel이 서버 channel에 없음(불가능)
		if (chan_target == serv_channels.end())
			return ;
		std::vector< std::pair< bool, Client* > > chan_clients = chan_target->second->getClients();
		int chan_clnts_size = chan_clients.size();
		for (int j = 0; j < chan_clnts_size; j++) {
			std::string tmp = chan_clients[j].second->getNickname();
			//? 본인인 경우에는 추가하지 않음
			if (tmp != _send_nick)
				related_clients.insert(tmp);
		}
		chan_target->second->deleteClient(*clnt);
		//? client가 나가고 더이상 channel에 유저가 없음 -> channel 제거
		if (chan_target->second->getClients().size() == 0)
			serv.deleteChnl(chan_target->second);
	}
	clnt->setConnected(false);
	std::string quit_msg = _appendRemaining();
	if (quit_msg.empty())
		quit_msg = "leaving";
	setMsgs(_send_nick, "ERROR :Closing link: (" + clnt->getUsername() + "@" + _send_addr + ") [Quit: " + quit_msg + "]\n");
	for (std::set< std::string >::iterator it = related_clients.begin(); it != related_clients.end(); it++) {
		setMsgs(*it, _genMsg(0, "QUIT :Quit", quit_msg));
	}
}