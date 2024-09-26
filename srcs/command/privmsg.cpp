#include "Command.hpp"
#include <string>

void Command::privmsg(Client& send_clnt, Server& serv) {

	std::string prefix;
	std::string msg;
	std::string msg_content;

	if (_args.empty()) {
		setMsgs(_send_nick, _genMsg(ERR_NORECIPIENT));
		return ;
	}
	else if (_args.size() < 2) {
		setMsgs(_send_nick, _genMsg(ERR_NOTEXTTOSEND));
		return ;
	}
	
	std::stringstream recv_list(_args.front());
	std::string target;

	std::map< int, Client* >::const_iterator clnt;
	std::map< std::string, Channel* >::const_iterator chnl;
	std::vector< std::pair< bool, Client* > >::const_iterator clnt_in_chnl;

	_args.pop();
	msg_content = _appendRemaining();
	if (msg_content.size() == 0) {
		setMsgs(_send_nick, _genMsg(ERR_NOTEXTTOSEND));
		return ;
	}
	if (!send_clnt.getRegistered()) {
		setMsgs(_send_nick, _genMsg(ERR_NOTREGISTERED, _cmd));
		return ;
	}

	while(std::getline(recv_list, target, ',')) {
		if (target[0] == '#') {
			chnl = serv.getChannels().find(target);
			if (chnl != serv.getChannels().end()) {
				if (chnl->first == target) {
					if (chnl->second->isChannelMember(_send_nick)) {
						msg = _genMsg(0, _cmd, target + " :" + msg_content);

						clnt_in_chnl = chnl->second->getClients().begin();
						while (clnt_in_chnl != chnl->second->getClients().end()) {
							if (clnt_in_chnl->second->getNickname() != _send_nick)
								setMsgs(clnt_in_chnl->second->getNickname(), msg);
							clnt_in_chnl++;
						}
					}
					else
						setMsgs(_send_nick, _genMsg(ERR_CANNOTSENDTOCHAN, target));
				}
			}
			else
				setMsgs(_send_nick, _genMsg(ERR_NOSUCHCHANNEL, target));
		}
		else {
			clnt = serv.getClients().begin();
			while (clnt != serv.getClients().end()) {
				if (clnt->second->getNickname() == target) {
					msg = _genMsg(0,  _cmd, target + " :" + msg_content);
					setMsgs(target, msg);
					break;
				}
				clnt++;
			}
			if (clnt == serv.getClients().end()) {
				setMsgs(_send_nick, _genMsg(ERR_NOSUCHNICK, target));
			}
		}
	}
}