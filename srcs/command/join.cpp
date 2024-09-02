#include "Command.hpp"

/*
? 해야 하는 것
! 처리해야 하는 오류
* (1) ERR_NEEDMOREPARAMS
* (2) ERR_BANNEDFROMCHAN
* (3) ERR_INVITEONLYCHAN
* (4) ERR_BADCHANNELKEY
* (5) ERR_CHANNELISFULL
* (6) ERR_BADCHANMASK
* (7) ERR_NOSUCHCHANNEL
* (8) ERR_TOOMANYCHANNELS
* (9) RPL_TOPIC
*/

void Command::join(Client& send_clnt, Server& serv)
{
    
    (void) send_clnt;
    (void) serv;
}