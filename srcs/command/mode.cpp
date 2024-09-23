#include "Command.hpp"

std::queue< std::pair< bool, char > > getFlag(const std::string& string) {
	std::queue< std::pair< bool, char> > flag_queue;
	bool        enable_flag = true;
	const int   length = string.size();

	for (int i = 0; i < length; i++) {
		char flag = string[i];
		if (flag == '+')
			enable_flag = true;
		else if (flag == '-')
			enable_flag = false;
		else if (flag == 'o' || flag == 'i' || flag == 'l' || flag == 'k' || flag == 't')
			flag_queue.push(std::make_pair(enable_flag, flag));
		else
			flag_queue.push(std::make_pair(false, flag));
	}
	return (flag_queue);
}

void Command::_kMode(bool flag, Channel* chan) {
	std::string pref;
	std::string msg;

	if (_args.empty()) {
		setMsgs(_send_nick, _genMsg(ERR_NEEDMOREPARAMS, chan->getTitle(), "k *"));
		return ;
	}
	std::string pw_str = _args.front();
	// flag가 true(+)이고, 이미 +k 모드가 아닐 때
	if (flag == true && !(chan->getMode() & MODE_K)) {
		chan->setMode(flag, MODE_K);
		chan->setPasswd(pw_str);
		setMsgs(chan->getTitle(), _genMsg(0, _cmd + " " + chan->getTitle(), "+k :" + pw_str));
	}
	// flag가 flase(-)이고, 이미 -k 모드가 아닐 때
	else if (flag == false && (chan->getMode() & MODE_K)) {
		// 인자의 string이 설정된 비밀번호와 같을 때
		if (chan->getPasswd() == pw_str) {
			chan->setMode(flag, MODE_K);
			setMsgs(chan->getTitle(), _genMsg(0, _cmd + " " + chan->getTitle(), "-k :" + pw_str));
		}
		// 인자의 string과 설정된 비밀번호가 다를 때
		else
			setMsgs(_send_nick, _genMsg(ERR_KEYSET, chan->getTitle()));
	}
	_args.pop();
}

void Command::_iMode(bool flag, Channel* chan) {
	std::string msg;

	// flag가 '+', 채널은 '-'인 상태 / 또는 flag가 '-', 채널은 '+'인 상태
	if ((flag == true && !(chan->getMode() & MODE_I)) || \
			(flag == false && (chan->getMode() & MODE_I))) {
		chan->setMode(flag, MODE_I);
		if (flag == true)
			msg = _genMsg(0, _cmd + " " + chan->getTitle(), ":+i");
		else
			msg = _genMsg(0, _cmd + " " + chan->getTitle(), ":-i");
		setMsgs(chan->getTitle(), msg);
	}
}

void Command::_lMode(bool flag, Channel* chan) {
	std::string msg;

	// flag가 '-'
	if (flag == false && (chan->getMode() & MODE_L)) {
		chan->setMode(flag, MODE_L);
		setMsgs(chan->getTitle(), _genMsg(0, _cmd + " " + chan->getTitle(), ":-l"));
	}
	else if (flag == true) {
		// 인자가 없음
		if (_args.empty()) {
			setMsgs(_send_nick, _genMsg(ERR_NEEDMOREPARAMS, chan->getTitle(), "l *"));
			return ;
		}
		int limit_str = std::atoi(_args.front().c_str());
		std::stringstream stream;
		stream << limit_str;
		if (limit_str < 0)
			setMsgs(_send_nick, _genMsg(696, chan->getTitle() + " l", _args.front() + " :Invalid limit mode parameter. Syntax: <limit>."));
		else if (chan->getMaxClients() != limit_str || stream.str() != _args.front()) {
			chan->setMode(flag, MODE_L);
			chan->setMaxClients(limit_str);
			setMsgs(chan->getTitle(), _genMsg(0, _cmd + " " + chan->getTitle(), "+l :" + stream.str()));
		}
		_args.pop();
	}
}

void Command::_tMode(bool flag, Channel* chan) {
	std::string msg;

	if ((flag == true && !(chan->getMode() & MODE_T)) || \
				(flag == false && (chan->getMode() & MODE_T))) {
		chan->setMode(flag, MODE_T);
		if (flag == true)
			msg = _genMsg(0, _cmd + " " + chan->getTitle(), ":+t");
		else
			msg = _genMsg(0, _cmd + " " + chan->getTitle(), ":-t");
		setMsgs(chan->getTitle(), msg);
	}
}

void Command::_oMode(bool flag, Channel* chan) {
	std::vector< std::pair< bool, Client* > >::const_iterator clnt_list = chan->getClients().begin();

	// 전달받을 인자가 없음
	if (_args.empty()) {
		setMsgs(_send_nick, _genMsg(ERR_NEEDMOREPARAMS, chan->getTitle(), "o *"));
		return ;
	}
	//! chans_it 탐색
	while (clnt_list != chan->getClients().end()) {
		if (_args.front() == clnt_list->second->getNickname())
			break ;
		clnt_list++;
	}
	//! chans_it이 없음
	if (clnt_list == chan->getClients().end()) {
		setMsgs(_send_nick, _genMsg(ERR_NOSUCHNICK, _args.front()));
		return ;
	}
	//* (1) flag는 true, chans_it는 false
	if (flag == true && clnt_list->first == false) {   
		chan->setOperator(true, _args.front());
		setMsgs(chan->getTitle(), _genMsg(0, _cmd + " " + chan->getTitle(), "+o :" + clnt_list->second->getNickname()));
	}
	//* (2) flag는 false, chans_it은 true
	else if (flag == false && clnt_list->first == true) {
		chan->setOperator(false, _args.front());
		setMsgs(chan->getTitle(), _genMsg(0, _cmd + " " + chan->getTitle(), "-o :" + clnt_list->second->getNickname()));
	}
}

void Command::mode(Server& serv) {
	std::string prefix;
	//! 인자 부족
	if (_args.size() == 0) {
		setMsgs(_send_nick, _genMsg(ERR_NEEDMOREPARAMS, _cmd));
		return ;
	}
	std::string chan_title = _args.front();
	_args.pop();
	//! 첫 번째 인자가 채널이 아님
	if (chan_title[0] != '#')
		return ;
	
	std::map< std::string, Channel* > 			serv_channels = serv.getChannels();
	std::map< std::string, Channel* >::iterator	chans_it = serv_channels.find(chan_title);
	//! 채널이 존재하지 않음
	if (chans_it == serv_channels.end()) {
		setMsgs(_send_nick, _genMsg(ERR_NOSUCHCHANNEL, chan_title));
		return ;
	}
	std::vector< std::pair< bool, Client* > >			chan_clnts = chans_it->second->getClients();
	std::vector< std::pair< bool, Client* > >::iterator	clnts_it = chan_clnts.begin();
	while (clnts_it != chan_clnts.end()) {
		if (clnts_it->second->getNickname() == _send_nick)
			break ;
		clnts_it++;
	}
	//! 채널에 클라이언트가 속해있지 않음
	if (clnts_it == chan_clnts.end()) {
		setMsgs(_send_nick, _genMsg(ERR_NOTONCHANNEL, chan_title));
		return ;
	}
	//? 인자가 없을 경우 현재 채널에 대한 mode 상태 출력
	if (_args.empty()) {
		int chan_mode = chans_it->second->getMode();
		if (chan_mode == 0)
			setMsgs(_send_nick, _genMsg(RPL_CHANNELMODEIS, chans_it->first, ":+"));
		else {
			std::string tmp;
			if (!(chan_mode & MODE_K) && !(chan_mode & MODE_L))
				tmp += ":";
			tmp += "+";
			if (chan_mode & MODE_I)
				tmp += "i";
			if (chan_mode & MODE_K)
				tmp += "k";
			if (chan_mode & MODE_L)
				tmp += "l";
			if (chan_mode & MODE_T)
				tmp += "t";
			if (chan_mode & MODE_K && chan_mode & MODE_L) {
				tmp += " " + chans_it->second->getPasswd();
				tmp += " :";
				std::stringstream stream;
				stream << chans_it->second->getMaxClients();
				tmp += stream.str();
			}
			else if (chan_mode & MODE_K)
				tmp += " :" + chans_it->second->getPasswd();
			else if (chan_mode & MODE_L) {
				tmp += " :";
				std::stringstream stream;
				stream << chans_it->second->getMaxClients();
				tmp += stream.str();
			}
			setMsgs(_send_nick, _genMsg(RPL_CHANNELMODEIS, chans_it->first, tmp));
		}
		return ;
	}
	//! 채널에 존재하지만 operator가 아님
	if (clnts_it->first == false) {
		setMsgs(_send_nick, _genMsg(ERR_CHANOPRIVSNEEDED, chan_title));
		return ;
	}
	std::queue< std::pair< bool, char > > flag_queue = getFlag(_args.front());
	_args.pop();
	while (!flag_queue.empty()) {
		std::pair< bool, char > flag = flag_queue.front();
		switch (flag.second) {
			case 'k':
				_kMode(flag.first, chans_it->second);
				break ;
			case 'l':
				_lMode(flag.first, chans_it->second);
				break ;
			case 'o':
				_oMode(flag.first, chans_it->second);
				break ;
			case 'i':
				_iMode(flag.first, chans_it->second);
				break ;
			case 't':
				_tMode(flag.first, chans_it->second);
				break ;
			default:
				setMsgs(_send_nick, _genMsg(ERR_UNKNOWNMODE, std::string(1, flag.second)));
				break ;
		}
		flag_queue.pop();
	}
}