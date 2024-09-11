#include "Command.hpp"
#include <string>

void Command::privmsg(Server& serv) {

	std::string prefix;
	std::string msg;
	std::string msg_content;

	if (_args.empty()) {
		prefix = serv.genPrefix(_sender, ERR_NORECIPIENT);
		msg = _genProtoMsg(ERR_NORECIPIENT, prefix);
		setMsgs(_sender, msg);
		return ;
	} else if (_args.size() < 2) {
		prefix = serv.genPrefix(_sender, ERR_NOTEXTTOSEND);
		msg = _genProtoMsg(ERR_NOTEXTTOSEND, prefix);
		setMsgs(_sender, msg);
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
		prefix = serv.genPrefix(_sender, ERR_NOTEXTTOSEND);
		msg = _genProtoMsg(ERR_NOTEXTTOSEND, prefix);
		setMsgs(_sender, msg);
		return ;
	}



	while(std::getline(recv_list, target, ',')) {

		if (target[0] == '#') {
			chnl = serv.getChannels().find(target);
			if (chnl != serv.getChannels().end()) {
				if (chnl->first == target) {
					if (chnl->second->isChannelMember(_sender)) {
						prefix = serv.genPrefix(_sender, 0);
						msg = prefix + " " + _cmd + " " + target + " :" + msg_content + "\n";

						clnt_in_chnl = chnl->second->getClients().begin();
						while (clnt_in_chnl != chnl->second->getClients().end()) {
							if (clnt_in_chnl->second->getNickname() != _sender)
								setMsgs(clnt_in_chnl->second->getNickname(), msg);
							clnt_in_chnl++;
						}
					} else {
						prefix = serv.genPrefix(_sender, ERR_CANNOTSENDTOCHAN);
						msg = _genProtoMsg(ERR_CANNOTSENDTOCHAN, prefix, target);
						setMsgs(_sender, msg);
					}
				}
			} else {
				prefix = serv.genPrefix(_sender, ERR_NOSUCHCHANNEL);
				msg = _genProtoMsg(ERR_NOSUCHCHANNEL, prefix, target);
				setMsgs(_sender, msg);
			}
		} else {
			clnt = serv.getClients().begin();
			while (clnt != serv.getClients().end()) {
				if (clnt->second->getNickname() == target) {
					prefix = serv.genPrefix(target, 0);
					msg = prefix + " " + _cmd + " " + target + " :" + msg_content + "\n";
					setMsgs(target, msg);
					break;
				}
				clnt++;
			}
			if (clnt == serv.getClients().end()) {
				prefix = serv.genPrefix(_sender, ERR_NOSUCHNICK);
				msg = _genProtoMsg(ERR_NOSUCHNICK, prefix, target);
				setMsgs(_sender, msg);
			}
		}
	}
}