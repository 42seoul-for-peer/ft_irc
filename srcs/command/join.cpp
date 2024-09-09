
#include "Command.hpp"

/*
TODO
"JOIN <channel>{,<channel>} [<key>{,<key}]
* JOIN  JOIN 명령은 클라이언트가 특정 채널을 listen하기 시작할 때 사용합니다.
* 클라이언트가 채널에 가입할 수 있는지 여부는 클라이언트가 연결한 서버에서만 확인합니다.
* 채널이 다른 서버로부터 수신을 받으면, 다른 모든 서버는 자동으로 사용자를 채널에 추가합니다.
* 이에 영향을 미치는 조건은 다음과 같습니다:
* 1. 채널이 초대 전용인 경우 사용자는 초대받아야 합니다.
* 2. 사용자의 nickname/username/hostname는 활성화된 bans와 일치하지 않아야 합니다.
* 3. key(password)가 설정되어 있는 경우 올바른 key(password)를 입력해야 합니다.
* 이것들에 대해서는 MODE 명령에서 더 자세히 설명됩니다(자세한 내용은 섹션 4.2.3 참조).

* 일단 user가 채널에 join하면, 서버가 수신하는 채널에 영향을 미치는 모든 명령에 대한 알림을 받습니다.
* 여기에는 MODE, KICK, PART, QUIT, 그리고 PRIVMSG/NOTICE가 포함됩니다.
* JOIN 명령이 성공하면 사용자는 채널의 토픽(RPL_TOPIC 사용)과 채널에 있는 사용자 목록(RPL_NAMREPLY 사용)을 보내며,
* 여기에는 사용자가 포함되어야 합니다.

todo 해야 하는 것
? * 성공
* 채널이 존재하지 않음
    - 채널이 새로 생김
    - client가 채널에 속하게 됨
    - client가 채널의 operator가 됨
* 채널이 존재했음
    - 채널에 접속함(새로 생성된 것은 아님)
    - client가 채널에 속하게 됨

! * 실패/오류 
* asdf

todo 처리해야 하는 오류
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
To reply to a NAMES message, a reply pair consisting
                  of RPL_NAMREPLY and RPL_ENDOFNAMES is sent by the
                  server back to the client.  If there is no channel
                  found as in the query, then only RPL_ENDOFNAMES is
                  returned.  The exception to this is when a NAMES
                  message is sent with no parameters and all visible
                  channels and contents are sent back in a series of
                  RPL_NAMEREPLY messages with a RPL_ENDOFNAMES to mark
                  the end.

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

todo "JOIN <channel>{,<channel>} [<key>{,<key}]
*/

#include <iostream>

std::vector<std::string> parsebyComma(std::queue< std::string >& _args)
{
    std::vector<std::string>    token_vec;
	if (_args.empty())
		return (std::vector< std::string >(0));
    std::stringstream           stream(_args.front());
    std::string                 token;
    
    while (std::getline(stream, token, ','))
        token_vec.push_back(token);
	_args.pop();
    return (token_vec);
}

void Command::join(Client& send_clnt, Server& serv)
{
    if (_args.size() < 1)
    {
        //! ERR_NEEDMOREPARAMS<461>   -> "<command> :Not enough parameters"
		std::string pref = serv.generatePrefix(send_clnt.getNickname(), ERR_NEEDMOREPARAMS);
		std::string msg = _genProtoMsg(ERR_NEEDMOREPARAMS, pref, send_clnt.getNickname(), "JOIN");
		setMsgs(send_clnt.getNickname(), msg);
		// _receiver.insert(make_pair(send_clnt.getNickname(), ERR_NEEDMOREPARAMS));
		return ;
    }

    std::vector<std::string> titles = parsebyComma(_args);
    std::vector<std::string> passwords = parsebyComma(_args);
    const int title_size = titles.size();
    const int password_size = passwords.size();


    std::map< std::string, Channel* > chan_list = serv.getChannels();
    std::map< std::string, Channel* >::iterator chan_it;
    
    for (int i = 0; i < title_size; i++)
    {
		// 클라이언트가 현재 접속한 채널의 수가 10개 이상임
		if (send_clnt.getCurrChannel().size() >= 10)
		{
			//! ERR_TOOMANYCHANNELS<405>  -> "<channel name> :You have joined too many channels"
			//! :irc.local 405 seungjun [channelname] :You are on too many channels
			std::string pref = serv.generatePrefix(send_clnt.getNickname(), ERR_TOOMANYCHANNELS);
			std::string msg = _genProtoMsg(ERR_TOOMANYCHANNELS, pref, send_clnt.getNickname(), titles[i]);
			setMsgs(send_clnt.getNickname(), msg);
			// _receiver.insert(make_pair(send_clnt.getNickname(), ERR_TOOMANYCHANNELS));
			break ;
			
		}
		// protocol message로 전달 받은 channel title의 첫 글자가 '#'가 아님 (nc로 직접 보낼 때 발생 우려 있음)
		// 또는, title의 길이가 200자를 초과함
		if (titles[i][0] != '#' || titles[i].size() > 200)
		{
			//! ERR_NOSUCHCHANNEL<403>    -> "<channel name> :No such channel"
			std::string pref = serv.generatePrefix(send_clnt.getNickname(), ERR_NOSUCHCHANNEL);
			std::string msg = _genProtoMsg(ERR_NOSUCHCHANNEL, pref, send_clnt.getNickname(), titles[i]);
			setMsgs(send_clnt.getNickname(), msg);
			// _receiver.insert(make_pair(send_clnt.getNickname(), ERR_NOSUCHCHANNEL));
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
			std::string pref = serv.generatePrefix(send_clnt.getNickname(), 0);
			// channel에게 보낼 메시지
			setMsgs(chan_it->first, pref + " JOIN :" + chan_it->first);
			pref = serv.generatePrefix(send_clnt.getNickname(), RPL_NAMREPLY);
			std::string msg = _genProtoMsg(RPL_NAMREPLY, pref, chan_it->first, ":[channelMode]");
			// client에게 보낼 메시지
			setMsgs(send_clnt.getNickname(), msg);
			// _receiver.insert(make_pair(new_channel->getTitle(), RPL_NAMREPLY));
        }
        // 채널이 존재함 (접속 시도, 비밀번호 확인 필요)
        else
        {
			const int chan_mode = chan_it->second->getMode();
			// key가 필요한 채널
			if (chan_mode & MODE_K)
			{
				// 인자로 입력받은 key 값이 없거나 비밀번호가 다름
				if (i > password_size - 1 || passwords[i] != chan_it->second->getPasswd())
				{
					//! ERR_BADCHANNELKEY<475>    -> "<channel> :Cannot join channel (+k)"
					std::string pref = serv.generatePrefix(send_clnt.getNickname(), ERR_BADCHANNELKEY);
					std::string msg = _genProtoMsg(ERR_BADCHANNELKEY, pref, send_clnt.getNickname(), titles[i]);
					setMsgs(send_clnt.getNickname(), msg);
					// _receiver.insert(make_pair(send_clnt.getNickname(), ERR_BADCHANNELKEY));
					break ;
				}
				else
				{
					chan_it->second->addClient(std::make_pair(false, &send_clnt));
					send_clnt.joinChannel(*chan_it->second);
					std::string pref = serv.generatePrefix(send_clnt.getNickname(), 0);
					// channel에게 보낼 메시지
					setMsgs(chan_it->first, pref + " JOIN :" + chan_it->first);
					pref = serv.generatePrefix(send_clnt.getNickname(), RPL_NAMREPLY);
					std::string msg = _genProtoMsg(RPL_NAMREPLY, pref, chan_it->first, ":[channelMode]");
					// client에게 보낼 메시지
					setMsgs(send_clnt.getNickname(), msg);
					// _receiver.insert(make_pair(chan_it->second->getTitle(), RPL_NAMREPLY));
				}
			}
			// invite 채널인 경우 (이때, 최대 인원 수를 초과할 수 있음)
			if (chan_mode & MODE_I)
			{
				std::vector< std::string > invited_list = chan_it->second->getInvitedClients();
				// invited_list에 send_clnt의 username이 존재하지 않음
				if (std::find(invited_list.begin(), invited_list.end(), send_clnt.getUsername()) == invited_list.end())
				{
					//! ERR_INVITEONLYCHAN<473>   -> "<channel> :Cannot join channel (+i)"
					std::string pref = serv.generatePrefix(send_clnt.getNickname(), ERR_INVITEONLYCHAN);
					std::string msg = _genProtoMsg(ERR_INVITEONLYCHAN, pref, send_clnt.getNickname(), titles[i]);
					setMsgs(send_clnt.getNickname(), msg);
					// _receiver.insert(make_pair(send_clnt.getNickname(), ERR_INVITEONLYCHAN));
					break ;
				}
				// invited_list에 존재함
				else
				{
					chan_it->second->addClient(std::make_pair(false, &send_clnt));
					send_clnt.joinChannel(*chan_it->second);
					// invited_list에서 username 삭제
					invited_list.erase(std::find(invited_list.begin(), invited_list.end(), send_clnt.getUsername()));
					std::string pref = serv.generatePrefix(send_clnt.getNickname(), 0);
					// channel에게 보낼 메시지
					setMsgs(chan_it->first, pref + " JOIN :" + chan_it->first);
					pref = serv.generatePrefix(send_clnt.getNickname(), RPL_NAMREPLY);
					std::string msg = _genProtoMsg(RPL_NAMREPLY, pref, chan_it->first, ":[channelMode]");
					// client에게 보낼 메시지
					setMsgs(send_clnt.getNickname(), msg);
					// _receiver.insert(make_pair(chan_it->second->getTitle(), RPL_NAMREPLY));
				}
			}
			// 최대 접속 가능 유저수가 지정됨
			else if (chan_mode & MODE_L)
			{
				// 현재 접속 유저 수가 channel의 최대 유저 수와 같거나 큼(초대 채널 상태였다가 해제된 경우)
				if (static_cast<int>(chan_it->second->getClients().size()) >= chan_it->second->getMaxClients())
				{
					//! ERR_CHANNELISFULL<471>    -> "<channel> :Cannot join channel (+l)"
					std::string pref = serv.generatePrefix(send_clnt.getNickname(), ERR_CHANNELISFULL);
					std::string msg = _genProtoMsg(ERR_CHANNELISFULL, pref, send_clnt.getNickname(), titles[i]);
					setMsgs(send_clnt.getNickname(), msg);
					// _receiver.insert(make_pair(send_clnt.getNickname(), ERR_CHANNELISFULL));
					break ;
				}
				else
				{
					chan_it->second->addClient(std::make_pair(false, &send_clnt));
					send_clnt.joinChannel(*chan_it->second);
					
					std::string pref = serv.generatePrefix(send_clnt.getNickname(), 0);
					// channel에게 보낼 메시지
					setMsgs(chan_it->first, pref + " JOIN :" + chan_it->first);
					pref = serv.generatePrefix(send_clnt.getNickname(), RPL_NAMREPLY);
					// 현재 channel의 mode를 출력해야하는데, 미구현이라서 [channelMode] 로 임시 조치
					std::string msg = _genProtoMsg(RPL_NAMREPLY, pref, chan_it->first, ":[channelMode]");
					// client에게 보낼 메시지
					setMsgs(send_clnt.getNickname(), msg);
					// _receiver.insert(make_pair(chan_it->second->getTitle(), RPL_NAMREPLY));
				}
			}
			// invite 채널도 아니고, 최대 접속 제한도 아님 (자유롭게 접속 가능)
			else
			{
				chan_it->second->addClient(std::make_pair(false, &send_clnt));
				send_clnt.joinChannel(*chan_it->second);
				std::string pref = serv.generatePrefix(send_clnt.getNickname(), 0);
				// channel에게 보낼 메시지
				setMsgs(chan_it->first, pref + " JOIN :" + chan_it->first);
				pref = serv.generatePrefix(send_clnt.getNickname(), RPL_NAMREPLY);
				std::string msg = _genProtoMsg(RPL_NAMREPLY, pref, chan_it->first, ":[channelMode]");
				// client에게 보낼 메시지
				setMsgs(send_clnt.getNickname(), msg);
				// _receiver.insert(make_pair(chan_it->second->getTitle(), RPL_NAMREPLY));
			}
        }
    }    
}