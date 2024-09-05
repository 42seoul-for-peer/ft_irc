#include "Command.hpp"
#include <string>

void Command::privmsg(Client& send_clnt, Server& serv) {  // send clnt 안받아도 됨 여기는 ..
	(void) send_clnt;
	if (_args.empty()) {
		_receiver.insert(std::make_pair("", ERR_NORECIPIENT));
		return ;
	} else if (_args.size() < 2) {
		_receiver.insert(std::make_pair(_args.front(), ERR_NOTEXTTOSEND));
		return ;
	}
	
	std::stringstream recv_list(_args.front());
	std::string tmp;

	std::map< int, Client* >::const_iterator clnt;
	std::map< std::string, Channel* >::const_iterator chnl;

	_args.pop();
	while (!_args.empty()) {
		_msg += _args.front();
		if (_args.size() > 1)
			_msg += " ";
		_args.pop();
	}

	if (_msg[0] == ':' && _msg.size() > 1) {
		_msg = _msg.substr(1);
	} else if (_msg[0] == ':' && _msg.size() == 0) {
		_receiver.insert(std::make_pair(_args.front(), ERR_NOTEXTTOSEND));
		return ;
	}

	while(std::getline(recv_list, tmp, ',')) {

		if (tmp[0] == '#') {
			chnl = serv.getChannels().find(tmp);
			if (chnl != serv.getChannels().end()) {
				if (chnl->first == tmp) {
					if (chnl->second->isChannelMember(_sender))
						_receiver.insert(std::make_pair(tmp, 0));
					else
						_receiver.insert(std::make_pair(tmp, ERR_CANNOTSENDTOCHAN));
				}
			} else
				_receiver.insert(std::make_pair(tmp, ERR_NOSUCHCHANNEL));
		} else {
			clnt = serv.getClients().begin();
			while (clnt != serv.getClients().end()) {
				if (clnt->second->getNickname() == tmp) {
					_receiver.insert(std::make_pair(tmp, 0));
					break;
				}
				clnt++;
			}
			if (clnt == serv.getClients().end())
				_receiver.insert(std::make_pair(tmp, ERR_NOSUCHNICK));
		}
	}
}