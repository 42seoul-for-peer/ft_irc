#include "Command.hpp"
#include <iostream>

bool	Command::_valid_nick(std::string& new_nick) const {
	const std::string	special = "-[]\\`^{}";
	int len = new_nick.length();
	if (len < 1 || len > 9) // 9자 초과이거나 0글자 문자열인 경우
		return (false);
	if (!isalpha(new_nick[0]))
		return (false);
	for (int i = 1; i < len; i++){
		if (special.find(new_nick[i]) == std::string::npos && !isalnum(new_nick[i])) {
			return (false);
		}
	}
	return (true);
}

void	Command::nick(Client& send_clnt, Server& serv) {
	std::string msg;
	std::string	new_nick;

	if (_args.size() < 1) {
		setMsgs(_send_nick, _genMsg(ERR_NEEDMOREPARAMS, _cmd));
		return ;
	}
	new_nick = _args.front();
	// 유효성 검증1: 사용 가능한 문자로 구성되어 있는가
	if (_valid_nick(new_nick) == false) {
		setMsgs(_send_nick, _genMsg(ERR_ERRONEUSNICKNAME));
		return ;
	}
	// 유효성 검증2: 중복되지 않는가
	if (serv.getClient(new_nick)) {
		setMsgs(_send_nick, _genMsg(ERR_NICKNAMEINUSE, new_nick));
		return ;
	}
	// 닉네임 변경하는 경우
	if (send_clnt.getRegistered() == true) {
		serv.updateInvitedList(_send_nick, new_nick);
		send_clnt.setNickname(new_nick);
		if (send_clnt.getCurrChannel().size()) {
			const std::vector< std::string >& chnltitles = send_clnt.getCurrChannel();
			unsigned long idx = 0;
			msg = _genMsg(0, _cmd, new_nick);
			while (idx < chnltitles.size()) {
				Channel* chan = (serv.getChannels().find(chnltitles[idx])->second);
				const std::vector< std::pair <bool, Client* > >& clnts_in_chnl = chan->getClients();
				unsigned long i = 0;
				while (i < clnts_in_chnl.size()){
					setMsgs(clnts_in_chnl[i].second->getNickname(), msg);
					i++;
				}
				idx++;
			}
		}
		else
			setMsgs(new_nick, _genMsg(0, _cmd, new_nick));
		return ;
	}
	else {	// 새로 등록하는 경우
		if (send_clnt.getUsername() != "*") { // user name 있는 경우
			if (send_clnt.getPassValidity() == true) { // 비밀번호가 맞음
				send_clnt.setNickname(new_nick);
				send_clnt.setRegistered();
				_send_nick = new_nick;
				msg = _genMsg(RPL_WELCOME);
			}
			else { // 비밀번호가 틀려서 종료
				msg = "ERROR :Closing Link: [Access Denied by Configuration]\n";
				send_clnt.setConnected(false);
			}
			setMsgs(_send_nick, msg);
		}
		else { // user name이 아직 없어서 등록 확인을 하지 않음
			send_clnt.setNickname(new_nick);
		}
	}
}