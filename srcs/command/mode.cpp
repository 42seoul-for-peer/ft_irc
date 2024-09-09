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

void Command::_kMode(bool flag, Client& send_clnt, Server& serv, Channel* chan)
{
    std::string pref;
    std::string msg;
    // 'k' 옵션은 args가 필수인데, args가 비어있음
    // 이 에러는 원래 696 error code인데, rfc에 존재하지 않아서 임의로 지정한 메시지를 보냄
    if (_args.empty())
    {
        //! 1| ERR_NEEDMOREPARAMS |461| "<command> :Not enough parameters"
        pref = serv.generatePrefix(send_clnt.getNickname(), ERR_CHANOPRIVSNEEDED);
        msg = _genProtoMsg(ERR_CHANOPRIVSNEEDED, pref, send_clnt.getNickname(), chan->getTitle(), "k");
        setMsg(send_clnt.getNickname(), msg);
        // _receiver.insert(make_pair(send_clnt.getNickname(), ERR_NEEDMOREPARAMS));
        return ;
    }
    std::string pw_str = _args.front();
    // flag가 true(+)이고, 이미 +k 모드가 아닐 때
    if (flag == true && !(chan->getMode() & MODE_K))
    {
        chan->setMode(flag, MODE_K);
        chan->setPasswd(pw_str);
        pref = serv.generatePrefix(send_clnt.getNickname(), 0);
        msg = _genProtoMsg(0, pref, "MODE", chan->getTitle(), "+k", ":" + pw_str);
        setMsg(chan->getTitle(), msg);
        // _receiver.insert(make_pair(chan_it->first, RPL_CHANNELMODEIS));
    }
    // flag가 flase(-)이고, 이미 -k 모드가 아닐 때
    else if(flag == false && (chan->getMode() & MODE_K))
    {
        // 인자의 string이 설정된 비밀번호와 같을 때
        if (chan->getPasswd() == pw_str)
        {
            chan->setMode(false, MODE_K);
            pref = serv.generatePrefix(send_clnt.getNickname(), 0);
            msg = _genProtoMsg(0, pref, "MODE", chan->getTitle(), "-k", ":" + pw_str);
            setMsg(chan_it->first, msg);
            // _receiver.insert(make_pair(chan_it->first, RPL_CHANNELMODEIS));
        }
        // 인자의 string과 설정된 비밀번호가 다를 때
        else
        {
            //! 5| ERR_KEYSET |467| "<channel> :Channel key already set"
            pref = serv.generatePrefix(send_clnt.getNickname(), ERR_KEYSET);
            msg = _genProtoMsg(ERR_KEYSET, pref, send_clnt.getNickname(), chan->getTitle());
            setMsg(send_clnt.getNickname(), msg);
            // _receiver.insert(make_pair(send_clnt.getNickname(), ERR_KEYSET));
        }
    }
    _args.pop();
}

void Command::_iMode(bool flag, Client& send_clnt, Server& serv, Channel* chan)
{
    std::string pref;
    std::string msg;

    // flag가 '+', 채널은 '-'인 상태 / 또는 flag가 '-', 채널은 '+'인 상태
    if (flag == true && !(chan->getMode() & MODE_I) || \
            flag == false && (chan->getMode() & MODE_I))
    {
        chan->setMode(flag, MODE_I);
        pref = serv.generatePrefix(send_clnt.getNickname(), 0);
        if (flag == true)
            msg = _genProtoMsg(0, pref, "MODE", chan->getTitle(), ":+i");
        else
            msg = _genProtoMsg(0, pref, "MODE", chan->getTitle(), ":-i");
        setMsg(chan->getTitle(), msg);
        // _receiver.insert(make_pair(chan->getTitle(), RPL_CHANNELMODEIS));
    }
}

void Command::_lMode(bool flag, Client& send_clnt, Server& serv, Channel* chan)
{
    std::string pref;
    std::string msg;

    // flag가 '-'
    if (flag == false && (chan->getMode() & MODE_L))
    {
        chan->setMode(flag, MODE_L);
        pref = serv.generatePrefix(send_clnt.getNickname(), 0);
        msg = _genProtoMsg(0, pref, "MODE", chan->getTitle(), ":-l");
        setMsg(chan->getTitle(), msg);
        // _receiver.insert(make_pair(send_clnt.getNickname(), ERR_KEYSET));
    }
    else if (flag == true)
    {
        if (_args.empty())
        {
            //! 1| ERR_NEEDMOREPARAMS |461| "<command> :Not enough parameters"
            pref = serv.generatePrefix(send_clnt.getNickname(), ERR_NEEDMOREPARAMS);
            msg = _genProtoMsg(ERR_NEEDMOREPARAMS, pref, send_clnt.getNickname(), chan->getTitle(), "MODE");
            setMsg(send_clnt.getNickname(), msg);
            // _receiver.insert(make_pair(send_clnt.getNickname(), ERR_NEEDMOREPARAMS));
            continue ;
        }
        int limit_str = std::atoi(_args.front().c_str());
        if (limit_str < 0)
            limit_str = 0;
        // flag가 +고, 현재 채널의 최대 유저수와 바꾸려는 값이 다름
        if (chan->getMaxClients() != limit_str)
        {
            chan->setMode(flag, MODE_L);
            chan->setMaxClients(limit_str);
            pref = serv.generatePrefix(send_clnt.getNickname(), 0);
            msg = _genProtoMsg(0, pref, "MODE", chan->getTitle(), "+l", ":" + _args.front());
            setMsg(chan->getTitle(), msg);
        }
        _args.pop();
    }
}

void Command::_tMode(bool flag, Client& send_clnt, Server& serv, Channel* chan)
{
    std::string pref;
    std::string msg;

    if (flag == true && !(chan->getMode() & MODE_T) || \
                flag == false && (chan->getMode() & MODE_T))
    {
        chan->setMode(flag, MODE_T);
        pref = serv.generatePrefix(send_clnt.getNickname(), 0);
        if (flag == true)
            msg = _genProtoMsg(0, pref, "MODE", chan->getTitle(), ":+t");
        else
            msg = _genProtoMsg(0, pref, "MODE", chan->getTitle(), ":-t");
        setMsg(chan->getTitle(), msg);
        // _receiver.insert(make_pair(chan_it->first, RPL_CHANNELMODEIS));
    }
}

void Command::_oMode(bool flag, Client& send_clnt, Server& serv, Channel* chan)
{
    std::vector< std::pair< bool, Client* > >::const_iterator clnt_list = chan->getClients().begin();
    Client*     target;
    std::string pref;
    std::string msg;

    // 전달받을 인자가 없음
    if (_args.empty())
    {
        pref = serv.generatePrefix(send_clnt.getNickname(), ERR_CHANOPRIVSNEEDED);
        msg = _genProtoMsg(ERR_CHANOPRIVSNEEDED, pref, send_clnt.getNickname(), chan->getTitle(), "k");
        setMsg(send_clnt.getNickname(), msg);
        return ;
    }
    //! target 탐색
    while (clnt_list != chan->getClients().end())
    {
        if (_args.front() == clnt_list->second->getNickname())
            break ;
        clnt_list++;
    }
    //! target이 없음
    if (clnt_list == chan->getClients().end())
    {
        pref = serv.generatePrefix(send_clnt.getNickname(), ERR_NOSUCHNICK);
        msg = _genProtoMsg(ERR_NOSUCHNICK, pref, send_clnt.getNickname(), _args.front());
        setMsg(send_clnt.getNickname(), msg);
        return ;
    }
    //* (1) flag는 true, target는 false
    if (flag == true && clnt_list->first == false)
    {   
        chan->setOperator(true, _args.front());
        pref = serv.generatePrefix(send_clnt.getNickname(), 0);
        msg = _genProtoMsg(0, pref, "MODE", chan->getTitle(), "+o :", clnt_list->second->getNickname());
        setMsg(chan->getTitle(), msg);
    }
    //* (2) flag는 false, target은 true
    else if (flag == false && clnt_list->first == true)
    {
        chan->setOperator(false, _args.front());
        pref = serv.generatePrefix(send_clnt.getNickname(), 0);
        msg = _genProtoMsg(0, pref, "MODE", chan->getTitle(), "-o :", clnt_list->second->getNickname());
        setMsg(chan->getTitle(), msg);
    }
}

void Command::mode(Client& send_clnt, Server& serv)
{
    std::string pref;
    std::string msg;
    // 인자가 부족함 (최소 1개의 인자 필요 <channel>)
    if (_args.size() < 1)
    {
        //! 1| ERR_NEEDMOREPARAMS |461| "<command> :Not enough parameters"
        pref = serv.generatePrefix(send_clnt.getNickname(), ERR_NEEDMOREPARAMS);
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
        switch (flag.second)
        {
            case 'k':
                kMode(flag.first, send_clnt, serv, chan_it->second);
                break ;
            case 'l':
                lMode(flag.first, send_clnt, serv, chan_it->second);
                break ;
            case 'o':
                oMode(flag.first, send_clnt, serv, chan_it->second);
                break ;
            case 'i':
                iMode(flag.first, send_clnt, serv, chan_it->second);
                break ;
            case 't':
                tMode(flag.first, send_clnt, serv, chan_it->second);
                break ;
            default:
                //! 6| ERR_UNKNOWNMODE |472| "<char> :is unknown mode char to me"
                pref = serv.generatePrefix(send_clnt.getNickname(), ERR_UNKNOWNMODE);
                msg = _genProtoMsg(ERR_UNKNOWNMODE, pref, send_clnt.getNickname(), flag.second);
                setMsg(send_clnt.getNickname(), msg);
                break ;
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