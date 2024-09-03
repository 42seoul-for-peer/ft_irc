#include "Command.hpp"
#include <string>

void Command::privmsg(int clnt_fd, Server& serv) {

	std::map< int, Client* >::const_iterator it = serv.getClients().find(clnt_fd);
	_sender = it->second.getNickname();
	if (_arg.empty()) {
		_receiver.push(std::make_pair("", ERR_NORECIPIENT));
		return ;
	} else if (_arg.size() < 3) {
		_receiver.push(std::make_pair(_arg.front(), ERR_NOTEXTTOSEND));
		return ;
	}
	
	std::stringstream recv_list(_args.front());
	std::string tmp;
	std::string 

	std::map< int, Client* >::const_iterator			clnt;
	std::map< std::string, Channel* >::const_iterator	chnl;

	_args.pop();
	while (!_args.empty()) {
		_msg += _arg.front();
		if (_args.size() > 1)
			_msg += " ";
		args.pop();
	}
	if (_msg[0] != ':') {
		_receiver.push(std::make_pair(_arg.front(), ERR_NOTEXTTOSEND));
		return ;
	} else {
		_msg = _msg.substr(1);
	}

	while(std::getline(recv_list, tmp, ',')) {

		if (tmp[0] == '#') {
			chnl = serv.getChannels().begin();
			while (chnl != serv.getChannels().end()) {
				if (chnl->first == tmp.substr(1)) {
					if (chnl->first->isChannelMember())
						_receiver.push(std::make_pair(tmp, 0));
					else
						_recevier.push(std::make_pair(tmp, ERR_CANNOTSENDTOCHAN));
					break;
				}
				chnl++;
			}
			if (chnl == serv.getChannels().end())
				_receiver.push(std::make_pair(tmp, ERR_NOSUCHCHANNEL));
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
				_receiver.push(std::make_pair(tmp, ERR_NOSUCHNICK));
		}
	}
}