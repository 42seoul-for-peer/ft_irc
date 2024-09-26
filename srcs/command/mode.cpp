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

bool Command::_kMode(bool flag, Channel* chan) {
	std::string pref;
	std::string msg;

	if (_args.empty()) {
		setMsgs(_send_nick, _genMsg(ERR_NEEDMOREPARAMS, chan->getTitle(), "k *"));
		return (false);
	}
	std::string pw_str = _args.front();
	// flag가 true(+)이고, 이미 +k 모드가 아닐 때
	if (flag == true && !(chan->getMode() & MODE_K)) {
		chan->setMode(flag, MODE_K);
		chan->setPasswd(pw_str);
		return (true);
	}
	// flag가 flase(-)이고, 이미 -k 모드가 아닐 때
	else if (flag == false && (chan->getMode() & MODE_K)) {
		// 인자의 string이 설정된 비밀번호와 같을 때
		if (chan->getPasswd() == pw_str) {
			chan->setMode(flag, MODE_K);
		}
		// 인자의 string과 설정된 비밀번호가 다를 때
		else
		{
			setMsgs(_send_nick, _genMsg(ERR_KEYSET, chan->getTitle()));
			_args.pop();
			return (false);
		}
		return (true);
	}
	_args.pop();
	return (false);
}

bool Command::_iMode(bool flag, Channel* chan) {
	std::string msg;

	// flag가 '+', 채널은 '-'인 상태 / 또는 flag가 '-', 채널은 '+'인 상태
	if ((flag == true && !(chan->getMode() & MODE_I)) || \
			(flag == false && (chan->getMode() & MODE_I))) {
		chan->setMode(flag, MODE_I);
		if (flag == true)
			msg = _genMsg(0, _cmd + " " + chan->getTitle(), ":+i");
		else
			msg = _genMsg(0, _cmd + " " + chan->getTitle(), ":-i");
		return (true);
	}
	return (false);
}

bool Command::_lMode(bool flag, Channel* chan) {
	std::string msg;

	// flag가 '-'
	if (flag == false && (chan->getMode() & MODE_L)) {
		chan->setMode(flag, MODE_L);
		return (true);
	}
	else if (flag == true) {
		// 인자가 없음
		if (_args.empty()) {
			setMsgs(_send_nick, _genMsg(ERR_NEEDMOREPARAMS, chan->getTitle(), "l *"));
			return (false);
		}
		int limit_str = std::atoi(_args.front().c_str());
		std::stringstream stream;
		stream << limit_str;
		if (limit_str < 0)
		{
			setMsgs(_send_nick, _genMsg(696, chan->getTitle() + " l", _args.front() + " :Invalid limit mode parameter. Syntax: <limit>."));
			_args.pop();
			return (false);
		}
		else if (chan->getMaxClients() != limit_str || stream.str() != _args.front()) {
			chan->setMode(flag, MODE_L);
			chan->setMaxClients(limit_str);
			return (true);
		}
	}
	_args.pop();
	return (false);
}

bool Command::_tMode(bool flag, Channel* chan) {
	std::string msg;

	if ((flag == true && !(chan->getMode() & MODE_T)) || \
				(flag == false && (chan->getMode() & MODE_T))) {
		chan->setMode(flag, MODE_T);
		if (flag == true)
			msg = _genMsg(0, _cmd + " " + chan->getTitle(), ":+t");
		else
			msg = _genMsg(0, _cmd + " " + chan->getTitle(), ":-t");
		return (true);
	}
	return (false);
}

bool Command::_oMode(bool flag, Channel* chan) {
	std::vector< std::pair< bool, Client* > >::const_iterator clnt_list = chan->getClients().begin();

	// 전달받을 인자가 없음
	if (_args.empty()) {
		setMsgs(_send_nick, _genMsg(ERR_NEEDMOREPARAMS, chan->getTitle(), "o *"));
		return (false);
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
		_args.pop();
		return (false);
	}
	//* (1) flag는 true, chans_it는 false
	if (flag == true && clnt_list->first == false) {   
		chan->setOperator(true, _args.front());
		return (true);
	}
	//* (2) flag는 false, chans_it은 true
	else if (flag == false && clnt_list->first == true) {
		chan->setOperator(false, _args.front());
		return (true);
	}
	_args.pop();
	return (false);
}

void Command::_promptMode(bool is_joined, Channel* chan)
{
	int chan_mode = chan->getMode();
	if (chan_mode == 0)
	{
		setMsgs(_send_nick, _genMsg(RPL_CHANNELMODEIS, chan->getTitle(), ":+"));
		return ;
	}

	std::string chan_passwd;
	if (is_joined == false)
		chan_passwd = "<key>";
	else
		chan_passwd = chan->getPasswd();

	std::string token;
	if (!(chan_mode & MODE_K) && !(chan_mode & MODE_L))
		token += ":";
	token += "+";
	if (chan_mode & MODE_I)
		token += "i";
	if (chan_mode & MODE_K)
		token += "k";
	if (chan_mode & MODE_L)
		token += "l";
	if (chan_mode & MODE_T)
		token += "t";
	if (chan_mode & MODE_K && chan_mode & MODE_L) {
		token += " " + chan_passwd;
		token += " :";
		std::stringstream stream;
		stream << chan->getMaxClients();
		token += stream.str();
	}
	else if (chan_mode & MODE_K)
		token += " :" + chan_passwd;
	else if (chan_mode & MODE_L) {
		token += " :";
		std::stringstream stream;
		stream << chan->getMaxClients();
		token += stream.str();
	}
	setMsgs(_send_nick, _genMsg(RPL_CHANNELMODEIS, chan->getTitle(), token));
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

	//? 채널 접속 여부 확인
	bool is_joined = true;
	if (clnts_it == chan_clnts.end())
		is_joined = false;

	//? 들어온 안자가 채널밖에 없음 -> 채널의 모드 출력
	if (_args.empty()) {
		_promptMode(is_joined, chans_it->second);
		return ;
	}

	//! 채널에 클라이언트가 속해있지 않음
	if (clnts_it == chan_clnts.end()) {
		setMsgs(_send_nick, _genMsg(ERR_NOTONCHANNEL, chan_title));
		return ;
	}
	//! 채널에 존재하지만 operator가 아님
	if (clnts_it->first == false) {
		setMsgs(_send_nick, _genMsg(ERR_CHANOPRIVSNEEDED, chan_title));
		return ;
	}
	std::queue< std::pair< int, std::string > >	token_queue;
	std::queue< std::pair< bool, char > >		flag_queue = getFlag(_args.front());
	int											is_enable = 1;
	_args.pop();
	while (!flag_queue.empty()) {
		std::pair< bool, char > flag = flag_queue.front();
		if (flag.first == true)
			is_enable = 1;
		else
			is_enable = -1;
		switch (flag.second) {
			case 'k':
				if (_kMode(flag.first, chans_it->second))
				{
					token_queue.push(std::make_pair(MODE_K * is_enable, _args.front()));
					_args.pop();
				}
				break ;
			case 'l':
				if (_lMode(flag.first, chans_it->second))
				{
					if (is_enable == 1)
					{
						int limit_str = std::atoi(_args.front().c_str());
						std::stringstream stream;
						stream << limit_str;
						token_queue.push(std::make_pair(MODE_L * is_enable, stream.str()));
						_args.pop();
					}
					else
						token_queue.push(std::make_pair(MODE_L * is_enable, ""));
				}
				break ;
			case 'o':
				if (_oMode(flag.first, chans_it->second))
				{
					token_queue.push(std::make_pair(MODE_O * is_enable, _args.front()));
					_args.pop();
				}
				break ;
			case 'i':
				if (_iMode(flag.first, chans_it->second))
					token_queue.push(std::make_pair(MODE_I * is_enable, ""));
				break ;
			case 't':
				if (_tMode(flag.first, chans_it->second))
					token_queue.push(std::make_pair(MODE_T * is_enable, ""));
				break ;
			default:
				setMsgs(_send_nick, _genMsg(ERR_UNKNOWNMODE, std::string(1, flag.second)));
				break ;
		}
		flag_queue.pop();
	}
	std::string set_mode_msg;
	std::queue< std::string > param_queue;
	int recent_flag = 0;
	while (!token_queue.empty())
	{
		std::pair< int, std::string > token = token_queue.front();
		// 처음 token을 계산하는 상황이거나 이전 flag와 +-값이 다른 상황
		if (set_mode_msg.empty() || recent_flag * token.first < 0)
			set_mode_msg += token.first > 0 ? "+" : "-";
		recent_flag = token.first > 0 ? 1 : -1;
		if (token.first < 0)
			token.first *= -1;
		switch (token.first)
		{
			case MODE_K:
				set_mode_msg += "k";
				if (!token.second.empty())
					param_queue.push(token.second);
				break;
			case MODE_L:
				set_mode_msg += "l";
				if (!token.second.empty())
					param_queue.push(token.second);
				break;
			case MODE_O:
				set_mode_msg += "o";
				if (!token.second.empty())
					param_queue.push(token.second);
				break;
			case MODE_T:
				set_mode_msg += "t";
				break;
			case MODE_I:
				set_mode_msg += "i";
				break;
		}
		token_queue.pop();
	}
	if (!set_mode_msg.empty())
	{
		set_mode_msg += " ";
		while (!param_queue.empty())
		{
			set_mode_msg += param_queue.front();
			param_queue.pop();
			if (!param_queue.empty())
				set_mode_msg += " ";
		}
		setMsgs(chans_it->first, _genMsg(0, _cmd + " " + chans_it->first, set_mode_msg));
	}
}