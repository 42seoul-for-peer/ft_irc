#ifndef REPLY_NUMBER_HPP
# define REPLY_NUMBER_HPP

// ERROR REPLIES ver. 1459
# define ERR_NOSUCHNICK 401 // "<nickname> :No such nick/channel"
# define ERR_NOSUCHCHANNEL 403 // "<channel name> :No such channel"
# define ERR_CANNOTSENDTOCHAN 404
# define ERR_TOOMANYCHANNELS 405
# define ERR_NORECIPIENT 411
# define ERR_NOTEXTTOSEND 412
# define ERR_UNKNOWNCOMMAND 421
# define ERR_NONICKNAMEGIVEN 431
# define ERR_ERRONEUSNICKNAME 432
# define ERR_NICKNAMEINUSE 433
# define ERR_USERNOTINCHANNEL 441
# define ERR_NOTONCHANNEL 442
# define ERR_USERONCHANNEL 443
# define ERR_NOTREGISTERED 451
# define ERR_NEEDMOREPARAMS 461
# define ERR_ALREADYREGISTRED 462
# define ERR_KEYSET 467
# define ERR_CHANNELISFULL 471
# define ERR_UNKNOWNMODE 472
# define ERR_INVITEONLYCHAN 473
# define ERR_BADCHANNELKEY 475
# define ERR_BADCHANMASK 476 // reserved reply number
# define ERR_CHANOPRIVSNEEDED 482

// COMMAND RESPONSES v.1459
# define RPL_CHANNELMODEIS 324
# define RPL_NOTOPIC 331
# define RPL_TOPIC 332
# define RPL_INVITING 341
# define RPL_NAMREPLY 353
# define RPL_ENDOFNAMES 366 // 353 종료 메세지라서 바로 같이 넣어줘야함
// v.2812
# define RPL_WELCOME 1

#endif