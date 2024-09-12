#include "Command.hpp"
/*
O (1) ERR_NEEDMOREPARAMS<461>   -> "<command> :Not enough parameters"
//(2) ERR_BANNEDFROMCHAN<474>   -> "<channel> :Cannot join channle (+b)" (구현 불필요)
O (3) ERR_INVITEONLYCHAN<473>   -> "<channel> :Cannot join channel (+i)"
O (4) ERR_BADCHANNELKEY<475>    -> "<channel> :Cannot join channel (+k)"
O (5) ERR_CHANNELISFULL<471>    -> "<channel> :Cannot join channel (+l)"
//(6) ERR_BADCHANMASK<476>      -> No Manual in RFC 1459 (:irc.local 476 what asdf :Invalid channel name)
O (7) ERR_NOSUCHCHANNEL<403>    -> "<channel name> :No such channel"
O (8) ERR_TOOMANYCHANNELS<405>  -> "<channel name> :You have joined too many channels"
* (9) RPL_TOPIC<332>            -> "<channel> :<topic>" 채널의 topic을 결정하기 위해 TOPIC 메시지를 보낼 때,
*                                                       topic이 설정되면 RPL_TOPIC을 보내고 그렇지 않으면 RPL_NOTOPIC을 보냅니다.
* (10) RPL_NOTOPIC<331>         -> "<channel> :No topic is set"
* (11) RPL_NAMREPLY<353>		-> "<channel> :[[@|+]<nick> [[@|+]<nick> [...]]]"
* (12) RPL_ENDOFNAMES<366>		-> "<channel> :End of /NAMES list"

* 채널의 이름은 '&' 또는 '#'로 시작하는 200자 이하의 string입니다.
* 첫 번째 문자가 '&' 또는 '#' 이어야 한다는 것 외의 요구사항은
* 공백, ctrl+G, ','를 포함할 수 없다는 것입니다.
* &: 모든 서버에 알려진 분산 채널

! 접속 시도시 영향을 주는 요소
* 초대 전용 채널인가? (+i)
* 채널 키가 존재하고, 활성화된 채널인가? (+k)
* 최대 인원 수를 초과하였는가? (+l)
? 만약 +kli인 채널에 접속을 시도할 경우
* (1) 비밀번호가 틀림 -> Bad channel key
* (2) 초대되지 않음 -> You must be invited
* (3) 최대 접속 가능 유저 수에는 제한받지 않음 (초대 전용 서버일 경우)

*/

#include <iostream>

void Command::join(Client& send_clnt, Server& serv)
{
	std::string	prefix;
	std::string msg;

	if (_args.size() < 1)
	{
		prefix = serv.genPrefix(send_clnt.getNickname(), ERR_NEEDMOREPARAMS);
		setMsgs(send_clnt.getNickname(), _genProtoMsg(ERR_NEEDMOREPARAMS, prefix));
		return ;
	}

	std::vector<std::string> titles = _parsebyComma(_args);
	_args.pop();
	std::vector<std::string> passwords = _parsebyComma(_args);
	const int title_size = titles.size();
	const int password_size = passwords.size();

	std::map< std::string, Channel* > chan_list = serv.getChannels();
	std::map< std::string, Channel* >::iterator chan_it;

	for (int i = 0; i < title_size; i++)
	{
		// 클라이언트가 현재 접속한 채널의 수가 10개 이상임
		if (send_clnt.getCurrChannel().size() >= 10)
		{
			//! :irc.local 405 seungjun [channelname] :You are on too many channels
			prefix = serv.genPrefix(send_clnt.getNickname(), ERR_TOOMANYCHANNELS);
			setMsgs(send_clnt.getNickname(), _genProtoMsg(ERR_TOOMANYCHANNELS, prefix, titles[i]));
			break ;
			
		}
		// protocol message로 전달 받은 channel title의 첫 글자가 '#'가 아님 (nc로 직접 보낼 때 발생 우려 있음)
		// 또는, title의 길이가 200자를 초과함
		if (titles[i][0] != '#' || titles[i].size() > 200)
		{
			prefix = serv.genPrefix(send_clnt.getNickname(), ERR_NOSUCHCHANNEL);
			setMsgs(send_clnt.getNickname(), _genProtoMsg(ERR_NOSUCHCHANNEL, prefix, titles[i]));
			break ;
		}
		// titles[i]이 channel list에 있는지 찾음
		chan_it = chan_list.find(titles[i]);
		// 채널이 존재하지 않음 (신규 생성)
		if (chan_it == chan_list.end())
		{
			Channel* new_channel = new Channel(titles[i], &send_clnt);
			send_clnt.joinChannel(*new_channel);
			serv.addNewChnl(new_channel);
			// channel에게 보낼 메시지
			setMsgs(titles[i], serv.genPrefix(_sender, 0) + "JOIN :" + titles[i] + "\n");
			prefix = serv.genPrefix(_sender, RPL_NAMREPLY);
			msg = prefix + "= " + titles[i] + " :@" + new_channel->printClientsList() + "\n";
			// client에게 보낼 메시지
			setMsgs(_sender, msg);
			prefix = serv.genPrefix(_sender, RPL_ENDOFNAMES);
			msg = prefix + titles[i] + " :End of /NAMES list." + "\n";
			setMsgs(_sender, msg);
		}
		// 채널이 존재함 (접속 시도, 비밀번호 확인 필요)
		else
		{
			// 이미 접속중인 채널
			std::cout << "존재하는 채널" << std::endl;
			std::vector< std::string > curr_chan = send_clnt.getCurrChannel();
			if (std::find(curr_chan.begin(), curr_chan.end(), titles[i]) != curr_chan.end())
				continue ;
			const int chan_mode = chan_it->second->getMode();
			// key가 필요한 채널
			if (chan_mode & MODE_K)
			{
				// 인자로 입력받은 key 값이 없거나 비밀번호가 다름
				if (i > password_size - 1 || passwords[i] != chan_it->second->getPasswd())
				{
					prefix = serv.genPrefix(send_clnt.getNickname(), ERR_BADCHANNELKEY);
					setMsgs(send_clnt.getNickname(), _genProtoMsg(ERR_BADCHANNELKEY, prefix, titles[i]));
					break ;
				}
				else
				{
					chan_it->second->addClient(std::make_pair(false, &send_clnt));
					send_clnt.joinChannel(*chan_it->second);
					prefix = serv.genPrefix(send_clnt.getNickname(), 0);
					// channel에게 보낼 메시지
					setMsgs(chan_it->first, serv.genPrefix(send_clnt.getNickname(), 0) + "JOIN :" + chan_it->first + "\n");
					prefix = serv.genPrefix(send_clnt.getNickname(), RPL_NAMREPLY);
					msg = prefix + "= " + chan_it->first + ":@" + chan_it->second->printClientsList() + "\n";
					// client에게 보낼 메시지
					setMsgs(send_clnt.getNickname(), msg);
					prefix = serv.genPrefix(send_clnt.getNickname(), RPL_ENDOFNAMES);
					msg = prefix + chan_it->first + ":End of /NAMES list." + "\n";
					setMsgs(send_clnt.getNickname(), msg);
				}
			}
			// invite 채널인 경우 (이때, 최대 인원 수를 초과할 수 있음)
			if (chan_mode & MODE_I)
			{
				std::vector< std::string > invited_list = chan_it->second->getInvitedClients();
				// invited_list에 send_clnt의 username이 존재하지 않음
				if (std::find(invited_list.begin(), invited_list.end(), send_clnt.getUsername()) == invited_list.end())
				{
					prefix = serv.genPrefix(send_clnt.getNickname(), ERR_INVITEONLYCHAN);
					setMsgs(send_clnt.getNickname(), _genProtoMsg(ERR_INVITEONLYCHAN, prefix, titles[i]));
					break ;
				}
				// invited_list에 존재함
				else
				{
					chan_it->second->addClient(std::make_pair(false, &send_clnt));
					send_clnt.joinChannel(*chan_it->second);
					// invited_list에서 username 삭제
					invited_list.erase(std::find(invited_list.begin(), invited_list.end(), send_clnt.getUsername()));
					prefix = serv.genPrefix(send_clnt.getNickname(), 0);
					// channel에게 보낼 메시지
					setMsgs(chan_it->first, serv.genPrefix(send_clnt.getNickname(), 0) + " JOIN :" + chan_it->first + "\n");
					prefix = serv.genPrefix(send_clnt.getNickname(), RPL_NAMREPLY);
					msg = prefix + "= " + chan_it->first + ":@" + chan_it->second->printClientsList() + "\n";
					// client에게 보낼 메시지
					setMsgs(send_clnt.getNickname(), msg);
					prefix = serv.genPrefix(send_clnt.getNickname(), RPL_ENDOFNAMES);
					msg = prefix + chan_it->first + ":End of /NAMES list." + "\n";
					setMsgs(send_clnt.getNickname(), msg);
				}
			}
			// 최대 접속 가능 유저수가 지정됨
			else if (chan_mode & MODE_L)
			{
				// 현재 접속 유저 수가 channel의 최대 유저 수와 같거나 큼(초대 채널 상태였다가 해제된 경우)
				if (static_cast<int>(chan_it->second->getClients().size()) >= chan_it->second->getMaxClients())
				{
					prefix = serv.genPrefix(send_clnt.getNickname(), ERR_CHANNELISFULL);
					setMsgs(send_clnt.getNickname(), _genProtoMsg(ERR_CHANNELISFULL, prefix, titles[i]));
					break ;
				}
				else
				{
					chan_it->second->addClient(std::make_pair(false, &send_clnt));
					send_clnt.joinChannel(*chan_it->second);
					
					prefix = serv.genPrefix(send_clnt.getNickname(), 0);
					// channel에게 보낼 메시지
					setMsgs(chan_it->first, serv.genPrefix(send_clnt.getNickname(), 0) + "JOIN :" + chan_it->first + "\n");
					prefix = serv.genPrefix(send_clnt.getNickname(), RPL_NAMREPLY);
					msg = prefix + "= " + chan_it->first + ":@" + chan_it->second->printClientsList() + "\n";
					// client에게 보낼 메시지
					setMsgs(send_clnt.getNickname(), msg);
					prefix = serv.genPrefix(send_clnt.getNickname(), RPL_ENDOFNAMES);
					msg = prefix + chan_it->first + ":End of /NAMES list." + "\n";
					setMsgs(send_clnt.getNickname(), msg);
				}
			}
			// invite 채널도 아니고, 최대 접속 제한도 아님 (자유롭게 접속 가능)
			else
			{
				chan_it->second->addClient(std::make_pair(false, &send_clnt));
				send_clnt.joinChannel(*chan_it->second);
				prefix = serv.genPrefix(send_clnt.getNickname(), 0);
				// channel에게 보낼 메시지
				setMsgs(chan_it->first, serv.genPrefix(send_clnt.getNickname(), 0) + " JOIN :" + chan_it->first + "\n");
				prefix = serv.genPrefix(send_clnt.getNickname(), RPL_NAMREPLY);
				msg = prefix + "= " + chan_it->first + ":@" + chan_it->second->printClientsList() + "\n";
				// client에게 보낼 메시지
				setMsgs(send_clnt.getNickname(), msg);
				prefix = serv.genPrefix(send_clnt.getNickname(), RPL_ENDOFNAMES);
				msg = prefix + chan_it->first + ":End of /NAMES list." + "\n";
				setMsgs(send_clnt.getNickname(), msg);
			}
		}
	}    
}