#include "Command.hpp"
#include <string>

void Command::privmsg(int clnt_fd, Server& serv) {

	// _arg 구조 확인 필요
	std::map< int, Client* >::const_iterator it = serv.getClients().find(clnt_fd);
	_sender = it->second.getNickname();
	if (_arg.empty()) {
		_receiver.push(std::make_pair(_sender, ERR_NORECIPIENT));
		return ;
	} else if (_arg.size() != 2) {
		_receiver.push(std::make_pair(_sender, ERR_NOTEXTTOSEND));
		return ;
	}

	_msg = _arg.back(); 
	
	std::stringstream recv_list(_args.front());
	std::string tmp;
	std::string 

	std::map< int, Client* >::const_iterator			clnt;
	std::map< std::string, Channel* >::const_iterator	chnl;

	while(std::getline(recv_list, tmp, ',')) {

		if (tmp[0] == '#') {
			chnl = serv.getChannels().begin();
			while (chnl != serv.getChannels().end()) {
				if (chnl->first == tmp.substr(1)) {
					if (chnl->first->isChannelMember())
						_receiver.push(std::make_pair(tmp, 0));
					else
						_recevier.push(std::make_pair(_sender, ERR_CANNOTSENDTOCHAN));
					break;
				}
				chnl++;
			}
			if (chnl == serv.getChannels().end())
				_receiver.push(std::make_pair(_sender, ERR_NOSUCHCHANNEL));
		} else {
			clnt = serv.getClients().begin();
			while (clnt != serv.getClients().end()) {
				if (clnt->second->getNickname() == tmp) {
					_receiver.push(std::make_pair(tmp, 0));
					break;
				}
				clnt++;
			}
			if (clnt == serv.getClients().end())
				_receiver.push(std::make_pair(_sender, ERR_NOSUCHNICK));
		}
	}
}