#include "Command.hpp"
#include <string>

void Command::privmsg(Client& send_clnt, Server& serv) {  // send clnt 안받아도 됨 여기는 ..
	(void) send_clnt;

	std::string prefix;
	std::string msg;
	std::string msg_content;

	if (_args.empty()) {
		prefix = serv.generatePrefix(_sender, ERR_NORECIPIENT);
		msg = _genProtoMsg(ERR_NORECIPIENT, prefix, _sender);
		setMsgs(_sender, msg);
		return ;
	} else if (_args.size() < 2) {
		prefix = serv.generatePrefix(_sender, ERR_NOTEXTTOSEND);
		msg = _genProtoMsg(ERR_NOTEXTTOSEND, prefix, _sender);
		setMsgs(_sender, msg);
		return ;
	}
	
	std::stringstream recv_list(_args.front());
	std::string target;

	std::map< int, Client* >::const_iterator clnt;
	std::map< std::string, Channel* >::const_iterator chnl;

	_args.pop();
	while (!_args.empty()) {
		msg_content += _args.front();
		if (_args.size() > 1)
			msg_content += " ";
		_args.pop();
	}

	if (msg_content[0] == ':' && msg_content.size() > 1) {
		msg_content = msg_content.substr(1);
	} else if (msg_content[0] == ':' && msg_content.size() == 0) {
		prefix = serv.generatePrefix(_sender, ERR_NOTEXTTOSEND);
		msg = _genProtoMsg(ERR_NOTEXTTOSEND, prefix, _sender);
		setMsgs(_sender, msg);
		return ;
	}

	while(std::getline(recv_list, target, ',')) {

		if (target[0] == '#') {
			chnl = serv.getChannels().find(target);
			if (chnl != serv.getChannels().end()) {
				if (chnl->first == target) {
					if (chnl->second->isChannelMember(_sender)) {
						prefix = serv.generatePrefix(_sender, 0);
						msg = prefix + " " + _cmd + " " + target + " :" + msg_content;
						setMsgs(target, msg);
					} else {
						prefix = serv.generatePrefix(_sender, ERR_CANNOTSENDTOCHAN, 0);
						msg = getErrMsg(ERR_CANNOTSENDTOCHAN, prefix, _sender, target);
						setMsgs(_sender, msg);
					}
				}
			} else {
				prefix = serv.generatePrefix(_sender, ERR_NOSUCHCHANNEL, 0);
				msg = _genProtoMsg(ERR_NOSUCHCHANNEL, prefix, _sender, target);
				setMsgs(_sender, msg);
			}
		} else {
			clnt = serv.getClients().begin();
			while (clnt != serv.getClients().end()) {
				if (clnt->second->getNickname() == target) {
					prefix = serv.generatePrefix(target, 0);
					msg = _genProtoMsg(0, prefix, target, msg_content);
					setMsgs(target, msg);
					break;
				}
				clnt++;
			}
			if (clnt == serv.getClients().end()) {
				prefix = serv.generatePrefix(_sender, ERR_NOSUCHNICK, 0);
				msg = _genProtoMsg(ERR_NOSUCHNICK, prefix, _sender, target);
				setMsgs(_sender, msg);
			}
		}
	}
}