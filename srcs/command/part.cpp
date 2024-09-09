#include "Command.hpp"

/*
ERR_NEEDMOREPARAMS
ERR_NOSUCHCHANNEL
ERR_NOTONCHANNEL
*/

void Command::part(Server& serv)
{
    std::string prefix;

    if (_args.size() < 1)
    {
        prefix = serv.generatePrefix(_sender, ERR_NEEDMOREPARAMS);
        setMsgs(_sender, _genProtoMsg(ERR_NEEDMOREPARAMS, prefix));
        return ;
    }
    std::map< std::string, Channel* >::iterator chan_it;
    std::map< std::string, Channel* > chan_list = serv.getChannels();
    std::vector<std::string> channels = _parsebyComma(_args);
    _args.pop();
    const std::string leaveMessage = appendRemaining(_args);
    const int channel_size = channels.size();
    for (int i = 0; i < channel_size; i++)
    {
        // 채널 먼저 찾기
        chan_it = chan_list.find(channels[i]);
        // 존재하지 않는 채널명
        if (chan_it == chan_list.end())
        {
            prefix = serv.generatePrefix(_sender, ERR_NOSUCHCHANNEL);
            setMsgs(_sender, _genProtoMsg(ERR_NOSUCHCHANNEL, prefix, channels[i]));
            break ;
        }
        // 채널은 존재하지만 클라이언트가 속해있지 않음
        const int chan_clnt_num = chan_it->second->getClients().size();
        int clnt_idx;
        for (clnt_idx = 0; clnt_idx < chan_clnt_num; clnt_idx++)
        {
            if ()
        }
    }
    (void) serv;
}