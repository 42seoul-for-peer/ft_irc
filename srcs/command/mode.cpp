#include "Command.hpp"

/*
todo: "<channel> ([+|-]|k|i|l|o|t) [<limit>] [<user>]

? 규칙
* 들어오는 flag의 순서에 따라 입력해야 하는 parameter의 순서가 결정
* | ex) mode <channel> +kl -> [key] [limit_user]
* | ex) mode <channel> +lk -> [limit_user] [key]
* enable(+)과 disable(-)은 혼합해서 사용 가능
* | ex) mode <channel> +k-i
* mode <channel>: 324, 329 코드를 가진 현재 상태와 시간 출력
* | :irc.local 324 seugnjun #room0 :+n
* | :irc.local 329 seugnjun #room0 :1725511747
* +k (인자 필요)
! | 우선순위가 높은 친구, 근데 rpl_no에는 없는 오류가 있음
* | :irc.local 696 seungjun #room0 k * :You must specify a parameter for the key mode. Syntax: <key>.
* +i (인자 불필요)
* +l (인자 필요)
* | 숫자가 아닌 값: 0이 입력됨, 제한 없음(그러나, mode는 여전히 활성화된 상태)
* | 0: 0이 입력됨
* | 음수: :irc.local 696 seugnjun #room0 l -1234 :Invalid limit mode parameter. Syntax: <limit>.
* +o (인자 불필요)
* +t (인자 불필요)

! 처리해야 하는 오류
O  1| ERR_NEEDMOREPARAMS    |461| "<command> :Not enough parameters"
O  2| ERR_CHANOPRIVSNEEDED  |482| "<channel> :You're not channel operator"
O  3| ERR_NOTONCHANNEL      |442| "<channel> :You're not on that channel"
O  4| ERR_NOSUCHCHANNEL     |403| "<channel name> :No such channel"
O  5| ERR_KEYSET            |467| "<channel> :Channel key already set"
O  6| ERR_UNKNOWNMODE       |472| "<char> :is unknown mode char to me"
O  7| ERR_NOSUCHNICK        |401| "<nickname> :No such nick/channel" (user는 관리하지 않음)
// 8| ERR_USERSDONTMATCH    |502| ":Cant change mode for other users" (user는 관리하지 않음)
// 9| ERR_UMODEUNKNOWNFLAG  |501| ":Unknown MODE flag" (user는 관리하지 않음)
O 10| RPL_CHANNELMODEIS     |324| "<channel> <mode> <mode params>"
// 11| RPL_BANLIST           |367| "<channel> <banid>" (ban은 구현 범위가 아님)
// 12| RPL_ENDOFBANLIST      |368| "<channel> :End of channel ban list" (ban은 구현 범위가 아님)
// 13| RPL_UMODEIS           |221| "<user mode string>" (user는 관리하지 않음)
To answer a query about a client's own mode, RPL_UMODEIS is sent back.
*/

std::queue< std::pair< bool, char > > getFlag(const std::string& string)
{
	std::queue< std::pair< bool, char> > flag_queue;
	bool        enable_flag = true;
	const int   length = string.size();

	for (int i = 0; i < length; i++)
	{
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

void Command::_kMode(bool flag, Server& serv, Channel* chan)
{
	std::string pref;
	std::string msg;
	// 'k' 옵션은 args가 필수인데, args가 비어있음
	// 이 에러는 원래 696 error code인데, rfc에 존재하지 않아서 임의로 지정한 메시지를 보냄
	if (_args.empty())
	{
		pref = serv.genPrefix(_sender, ERR_NEEDMOREPARAMS);
		// overriding 필요
		setMsgs(_sender, _genProtoMsg(ERR_NEEDMOREPARAMS, pref, chan->getTitle(), "k *"));
		return ;
	}
	std::string pw_str = _args.front();
	// flag가 true(+)이고, 이미 +k 모드가 아닐 때
	if (flag == true && !(chan->getMode() & MODE_K))
	{
		chan->setMode(flag, MODE_K);
		chan->setPasswd(pw_str);
		pref = serv.genPrefix(_sender, 0);
		msg = pref + "MODE" + chan->getTitle() + " +k :" + pw_str + "\n";
		setMsgs(chan->getTitle(), msg);
	}
	// flag가 flase(-)이고, 이미 -k 모드가 아닐 때
	else if (flag == false && (chan->getMode() & MODE_K))
	{
		// 인자의 string이 설정된 비밀번호와 같을 때
		if (chan->getPasswd() == pw_str)
		{
			chan->setMode(flag, MODE_K);
			pref = serv.genPrefix(_sender, 0);
			msg = pref + "MODE" + chan->getTitle() + " -k :" + pw_str + "\n";
			setMsgs(chan->getTitle(), msg);
		}
		// 인자의 string과 설정된 비밀번호가 다를 때
		else
		{
			pref = serv.genPrefix(_sender, ERR_KEYSET);
			setMsgs(_sender, _genProtoMsg(ERR_KEYSET, pref, chan->getTitle()));
		}
	}
	_args.pop();
}

void Command::_iMode(bool flag, Server& serv, Channel* chan)
{
	std::string pref;
	std::string msg;

	// flag가 '+', 채널은 '-'인 상태 / 또는 flag가 '-', 채널은 '+'인 상태
	if ((flag == true && !(chan->getMode() & MODE_I)) || \
			(flag == false && (chan->getMode() & MODE_I)))
	{
		chan->setMode(flag, MODE_I);
		pref = serv.genPrefix(_sender, 0);
		if (flag == true)
			msg = pref + " MODE " + chan->getTitle() + " :+i" + "\n";
		else
			msg = pref + " MODE " + chan->getTitle() + " :-i" + "\n";
		setMsgs(chan->getTitle(), msg);
	}
}

void Command::_lMode(bool flag, Server& serv, Channel* chan)
{
	std::string pref;
	std::string msg;

	// flag가 '-'
	if (flag == false && (chan->getMode() & MODE_L))
	{
		chan->setMode(flag, MODE_L);
		pref = serv.genPrefix(_sender, 0);
		msg = pref + " MODE " + chan->getTitle() + " :-l" + "\n";;
		setMsgs(chan->getTitle(), msg);
	}
	else if (flag == true)
	{
		// 인자가 없음
		if (_args.empty())
		{
			pref = serv.genPrefix(_sender, ERR_NEEDMOREPARAMS);
			// overriding 필요
			setMsgs(_sender, _genProtoMsg(ERR_NEEDMOREPARAMS, pref, chan->getTitle(), "l *"));
			return ;
		}
		int limit_str = std::atoi(_args.front().c_str());
		if (limit_str < 0)
		{
			// 특수한 케이스
			pref = serv.genPrefix(_sender, 696);
			msg = pref + " " + chan->getTitle() + " l " + _args.front() + " :Invalid limit mode parameter. Syntax: <limit>." + "\n";;
			setMsgs(_sender, msg);
		}
		// flag가 +고, 현재 채널의 최대 유저수와 바꾸려는 값이 다름
		// :seungjun!root@127.0.0.1 MODE #room0 +l :12
		if (chan->getMaxClients() != limit_str)
		{
			chan->setMode(flag, MODE_L);
			chan->setMaxClients(limit_str);
			pref = serv.genPrefix(_sender, 0);
			msg = pref + " MODE " + chan->getTitle() + " +l :" + _args.front() + "\n";;
			setMsgs(chan->getTitle(), msg);
		}
		_args.pop();
	}
}

void Command::_tMode(bool flag, Server& serv, Channel* chan)
{
	std::string pref;
	std::string msg;

	if ((flag == true && !(chan->getMode() & MODE_T)) || \
				(flag == false && (chan->getMode() & MODE_T)))
	{
		chan->setMode(flag, MODE_T);
		pref = serv.genPrefix(_sender, 0);
		if (flag == true)
			msg = pref + " MODE " + chan->getTitle() + " :+t" + "\n";
		else
			msg = pref + " MODE " + chan->getTitle() + " :-t" + "\n";
		setMsgs(chan->getTitle(), msg);
	}
}

void Command::_oMode(bool flag, Server& serv, Channel* chan)
{
	std::vector< std::pair< bool, Client* > >::const_iterator clnt_list = chan->getClients().begin();
	std::string pref;
	std::string msg;

	// 전달받을 인자가 없음
	if (_args.empty())
	{
		pref = serv.genPrefix(_sender, ERR_CHANOPRIVSNEEDED);
		setMsgs(_sender, _genProtoMsg(ERR_CHANOPRIVSNEEDED, pref, chan->getTitle(), "k"));
		return ;
	}
	//! chans_it 탐색
	while (clnt_list != chan->getClients().end())
	{
		if (_args.front() == clnt_list->second->getNickname())
			break ;
		clnt_list++;
	}
	//! chans_it이 없음
	if (clnt_list == chan->getClients().end())
	{
		pref = serv.genPrefix(_sender, ERR_NOSUCHNICK);
		setMsgs(_sender, _genProtoMsg(ERR_NOSUCHNICK, pref, _args.front()));
		return ;
	}
	//* (1) flag는 true, chans_it는 false
	if (flag == true && clnt_list->first == false)
	{   
		chan->setOperator(true, _args.front());
		pref = serv.genPrefix(_sender, 0);
		msg = pref + " MODE " + chan->getTitle() + " +o :" + clnt_list->second->getNickname() + "\n";
		setMsgs(chan->getTitle(), msg);
	}
	//* (2) flag는 false, chans_it은 true
	else if (flag == false && clnt_list->first == true)
	{
		chan->setOperator(false, _args.front());
		pref = serv.genPrefix(_sender, 0);
		msg = pref + " MODE " + chan->getTitle() + " +o :" + clnt_list->second->getNickname() + "\n";
		setMsgs(chan->getTitle(), msg);
	}
}

void Command::mode(Server& serv)
{
	std::string prefix;
	//! 인자 부족
	if (_args.size() == 0)
	{
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
	if (chans_it == serv_channels.end())
	{
		setMsgs(_send_nick, _genMsg(ERR_NOSUCHCHANNEL, chan_title));
		return ;
	}
	std::vector< std::pair< bool, Client* > >			chan_clnts = chans_it->second->getClients();
	std::vector< std::pair< bool, Client* > >::iterator	clnts_it = chan_clnts.begin();
	while (clnts_it != chan_clnts.end())
	{
		if (clnts_it->second->getNickname() == _send_nick)
			break ;
		clnts_it++;
	}
	//! 채널에 클라이언트가 속해있지 않음
	if (clnts_it == chan_clnts.end())
	{
		setMsgs(_send_nick, _genMsg(ERR_NOTONCHANNEL, chan_title));
		return ;
	}
	//! 채널에 존재하지만 operator가 아님
	if (clnts_it->first == false)
	{
		setMsgs(_send_nick, _genMsg(ERR_CHANOPRIVSNEEDED, chan_title));
		return ;
	}
	std::queue< std::pair< bool, char > > flag_queue = getFlag(_args.front());
	_args.pop();
	while (!flag_queue.empty())
	{
		std::pair< bool, char > flag = flag_queue.front();
		switch (flag.second)
		{
			case 'k':
				_kMode(flag.first, serv, chans_it->second);
				break ;
			case 'l':
				_lMode(flag.first, serv, chans_it->second);
				break ;
			case 'o':
				_oMode(flag.first, serv, chans_it->second);
				break ;
			case 'i':
				_iMode(flag.first, serv, chans_it->second);
				break ;
			case 't':
				_tMode(flag.first, serv, chans_it->second);
				break ;
			default:
				setMsgs(_send_nick, _genMsg(ERR_UNKNOWNMODE, std::string(1, flag.second)));
				break ;
		}
		flag_queue.pop();
	}
}