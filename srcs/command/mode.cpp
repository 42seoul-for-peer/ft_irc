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
// 7| ERR_NOSUCHNICK        |401| "<nickname> :No such nick/channel" (user는 관리하지 않음)
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
    char        flag;

    for (int i = 0; i < length; i++)
    {
        flag = string[i];
        if (flag == '+')
            enable_flag = true;
        else if (flag == '-')
            enable_flag = false;
        else if (flag == 'o' || flag == 'i' || flag == 'l' || flag == 'k' || flag == 't')
            flag_queue.push(std::make_pair(enable_flag, flag));
        else
            flag_queue.push(std::make_pair(false, 'X'));
    }
    return (flag_queue);
}

void Command::mode(Client& send_clnt, Server& serv)
{
    std::string pref;
    std::string msg;
    // 인자가 부족함 (최소 1개의 인자 필요 <channel>)
    if (_args.size() < 1)
    {
        //! 1| ERR_NEEDMOREPARAMS |461| "<command> :Not enough parameters"
        pref = serv.generatePrefix(send_clnt.getNickname(), ERR_NEEDMOREPARAMS, send_clnt.getSockFd());
		msg = _genProtoMsg(ERR_NEEDMOREPARAMS, pref, send_clnt.getNickname(), "MODE");
		setMsg(send_clnt.getNickname(), msg);
        // _receiver.insert(make_pair(send_clnt.getNickname(), ERR_NEEDMOREPARAMS));
        return ;
    }
    std::map< std::string, Channel* >::const_iterator chan_it = serv.getChannels().find(_args.front());
    _args.pop();
    // channel이 존재하지 않음
    if (chan_it == serv.getChannels().end())
    {
        //! 4| ERR_NOSUCHCHANNEL |403| "<channel name> :No such channel"
        pref = serv.generatePrefix(send_clnt.getNickname(), ERR_NOSUCHCHANNEL);
        msg = _genProtoMsg(ERR_NOSUCHCHANNEL, pref, send_clnt.getNickname(), chan_it->first);
        setMsg(send_clnt.getNickname(), msg);
        // _receiver.insert(make_pair(send_clnt.getNickname(), ERR_NOSUCHCHANNEL));
        return ;
    }
    std::vector< std::pair< bool, Client* > >::const_iterator chan_clnt_it = \
        chan_it->second->getClients().begin();
    while (1)
    {
        // channel에 속해있지 않음
        if (chan_clnt_it == chan_it->second->getClients().end())
        {
            //! 3| ERR_NOTONCHANNEL |442| "<channel> :You're not on that channel"
            pref = serv.generatePrefix(send_clnt.getNickname(), ERR_NOTONCHANNEL);
            msg = _genProtoMsg(ERR_NOTONCHANNEL, pref, send_clnt.getNickname(), chan_it->first);
            setMsg(send_clnt.getNickname(), msg);
            // _receiver.insert(make_pair(send_clnt.getNickname(), ERR_NOTONCHANNEL));
            return ;
        }
        if (chan_clnt_it->second->getNickname() == send_clnt.getNickname())
        {
            // channel에 존재하지만, operator가 아님
            if (chan_clnt_it->first != true)
            {
                //! 2| ERR_CHANOPRIVSNEEDED |482| "<channel> :You're not channel operator"
                pref = serv.generatePrefix(send_clnt.getNickname(), ERR_CHANOPRIVSNEEDED);
                msg = _genProtoMsg(ERR_CHANOPRIVSNEEDED, pref, send_clnt.getNickname(), chan_it->first);
                setMsg(send_clnt.getNickname(), msg);
                // _receiver.insert(make_pair(send_clnt.getNickname(), ERR_CHANOPRIVSNEEDED));
                return ;
            }
            else
                break ;
        }
        chan_clnt_it++;
    }
    std::queue< std::pair< bool, char > > flag_queue = getFlag(_args.front());
    _args.pop();
    while (!flag_queue.empty())
    {
        std::pair< bool, char > flag = flag_queue.front();
        // flag가 'k' 옵션
        if (flag.second == 'k')
        {
            // 'k' 옵션은 args가 필수인데, args가 비어있음
            // 이 에러는 원래 696 error code인데, rfc에 존재하지 않아서 임의로 지정한 메시지를 보냄
            if (_args.empty())
            {
                //! 1| ERR_NEEDMOREPARAMS |461| "<command> :Not enough parameters"
                pref = serv.generatePrefix(send_clnt.getNickname(), ERR_CHANOPRIVSNEEDED);
                msg = _genProtoMsg(ERR_CHANOPRIVSNEEDED, pref, send_clnt.getNickname(), chan_it->first, "k");
                setMsg(send_clnt.getNickname(), msg);
                // _receiver.insert(make_pair(send_clnt.getNickname(), ERR_NEEDMOREPARAMS));
                continue ;
            }
            std::string pw_str = _args.front();
            // flag가 true(+)이고, 이미 +k 모드가 아닐 때
            if (flag.first == true && !(chan_it->second->getMode() & MODE_K))
            {
                chan_it->second->setMode(flag.first, MODE_K);
                chan_it->second->setPasswd(pw_str);
                pref = serv.generatePrefix(send_clnt.getNickname(), 0);
                msg = _genProtoMsg(0, pref, "MODE", chan_it->first, "+k", ":" + pw_str);
                setMsg(chan_it->first, msg);
                // _receiver.insert(make_pair(chan_it->first, RPL_CHANNELMODEIS));
            }
            // flag가 flase(-)이고, 이미 -k 모드가 아닐 때
            else if(flag.first == false && (chan_it->second->getMode() & MODE_K))
            {
                // 인자의 string이 설정된 비밀번호와 같을 때
                if (chan_it->second->getPasswd() == pw_str)
                {
                    chan_it->second->setMode(false, MODE_K);
                    pref = serv.generatePrefix(send_clnt.getNickname(), 0);
                    msg = _genProtoMsg(0, pref, "MODE", chan_it->first, "-k", ":" + pw_str);
                    setMsg(chan_it->first, msg);
                    // _receiver.insert(make_pair(chan_it->first, RPL_CHANNELMODEIS));
                }
                // 인자의 string과 설정된 비밀번호가 다를 때
                else
                {
                    //! 5| ERR_KEYSET |467| "<channel> :Channel key already set"
                    pref = serv.generatePrefix(send_clnt.getNickname(), ERR_KEYSET);
                    msg = _genProtoMsg(ERR_KEYSET, pref, send_clnt.getNickname(), chan_it->first);
                    setMsg(send_clnt.getNickname(), msg);
                    // _receiver.insert(make_pair(send_clnt.getNickname(), ERR_KEYSET));
                }
            }
            _args.pop();
        }
        // flag가 'l'
        else if (flag.second == 'l')
        {
            // flag가 '-'
            if (flag.first == false && (chan_it->second->getMode() & MODE_L))
            {
                chan_it->second->setMode(flag.first, MODE_L);
                pref = serv.generatePrefix(send_clnt.getNickname(), 0);
                msg = _genProtoMsg(0, pref, "MODE", chan_it->first, ":-l");
                setMsg(chan_it->first, msg);
                // _receiver.insert(make_pair(send_clnt.getNickname(), ERR_KEYSET));
            }
            else if (flag.first == true)
            {
                if (_args.empty())
                {
                    //! 1| ERR_NEEDMOREPARAMS |461| "<command> :Not enough parameters"
                    pref = serv.generatePrefix(send_clnt.getNickname(), ERR_NEEDMOREPARAMS);
                    msg = _genProtoMsg(ERR_NEEDMOREPARAMS, pref, send_clnt.getNickname(), chan_it->first, "MODE");
                    setMsg(send_clnt.getNickname(), msg);
                    // _receiver.insert(make_pair(send_clnt.getNickname(), ERR_NEEDMOREPARAMS));
                    continue ;
                }
                int limit_str = std::atoi(_args.front().c_str());
                if (limit_str < 0)
                    limit_str = 0;
                // flag가 +고, 현재 채널의 최대 유저수와 바꾸려는 값이 다름
                if (chan_it->second->getMaxClients() != limit_str)
                {
                    chan_it->second->setMode(flag.first, MODE_L);
                    chan_it->second->setMaxClients(limit_str);
                    pref = serv.generatePrefix(send_clnt.getNickname(), 0);
                    msg = _genProtoMsg(0, pref, "MODE", chan_it->first, "+l", ":" + _args.front());
                    setMsg(chan_it->first, msg);
                }
                _args.pop();
            }
        }
        // flag가 'o'
        else if (flag.second == 'o')
        {
            ;
        }
        // flag가 'i'
        else if (flag.second == 'i')
        {
            // flag가 '+', 채널은 '-'인 상태 / 또는 flag가 '-', 채널은 '+'인 상태
            if (flag.first == true && !(chan_it->second->getMode() & MODE_I) || \
                    flag.first == false && (chan_it->second->getMode() & MODE_I))
            {
                chan_it->second->setMode(flag.first, MODE_I);
                pref = serv.generatePrefix(send_clnt.getNickname(), 0);
                if (flag.first == true)
                    msg = _genProtoMsg(0, pref, "MODE", chan_it->first, ":+i");
                else
                    msg = _genProtoMsg(0, pref, "MODE", chan_it->first, ":-i");
                setMsg(chan_it->first, msg);
                // _receiver.insert(make_pair(chan_it->first, RPL_CHANNELMODEIS));
            }
        }
        // flag가 't'
        else if (flag.second == 't')
        {
            if (flag.first == true && !(chan_it->second->getMode() & MODE_T) || \
                    flag.first == false && (chan_it->second->getMode() & MODE_T))
            {
                chan_it->second->setMode(flag.first, MODE_T);
                pref = serv.generatePrefix(send_clnt.getNickname(), 0);
                if (flag.first == true)
                    msg = _genProtoMsg(0, pref, "MODE", chan_it->first, ":+t");
                else
                    msg = _genProtoMsg(0, pref, "MODE", chan_it->first, ":-t");
                setMsg(chan_it->first, msg);
                // _receiver.insert(make_pair(chan_it->first, RPL_CHANNELMODEIS));
            }
        }
        // 해당 flag가 존재하지 않음
        else
        {
            //! 6| ERR_UNKNOWNMODE |472| "<char> :is unknown mode char to me"
            pref = serv.generatePrefix(send_clnt.getNickname(), ERR_UNKNOWNMODE);
            msg = _genProtoMsg(ERR_UNKNOWNMODE, pref, send_clnt.getNickname(), std::string(flag.second));
            setMsg(send_clnt.getNickname(), msg);
            // _receiver.insert(make_pair(send_clnt.getNickname(), ERR_UNKNOWNMODE));
        }
        flag_queue.pop();
    }
}

/* MODE #room0 +kli-tasdf password 20
!:irc.local 472 seungjun a :is not a recognised channel mode.
!:irc.local 472 seungjun d :is not a recognised channel mode.
!:irc.local 472 seungjun f :is not a recognised channel mode.
!:seungjun!root@127.0.0.1 MODE #room0 +kli-t password :20
*/