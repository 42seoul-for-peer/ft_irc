
# Error Codes Documentation

| Error Number | Error Name             | Error Message                                   |
|:------------:|------------------------|-------------------------------------------------|
| 401          | ERR_NOSUCHNICK         | "<nickname> :No such nick/channel"             |X(mode)
| 403          | ERR_NOSUCHCHANNEL      | "<channel name> :No such channel"              |O
| 404          | ERR_CANNOTSENDTOCHAN   | "<channel name> :Cannot send to channel"       |
| 405          | ERR_TOOMANYCHANNELS    | "<channel name> :You have joined too many channels" |O
| 406          | ERR_WASNOSUCHNICK      | "<nickname> :There was no such nickname"       |
| 407          | ERR_TOOMANYTARGETS     | "<target> :Duplicate recipients. No message delivered" |
| 409          | ERR_NOORIGIN           | ":No origin specified"                         |
| 411          | ERR_NORECIPIENT        | ":No recipient given (<command>)"              |
| 412          | ERR_NOTEXTTOSEND       | ":No text to send"                             |
| 421          | ERR_UNKNOWNCOMMAND     | "<command> :Unknown command"                   |
| 422          | ERR_NOMOTD             | ":MOTD File is missing"                        |
| 431          | ERR_NONICKNAMEGIVEN    | ":No nickname given"                           |O
| 432          | ERR_ERRONEUSNICKNAME   | "<nick> :Erroneus nickname"                    |O
| 433          | ERR_NICKNAMEINUSE      | "<nick> :Nickname is already in use"           |O
| 436          | ERR_NICKCOLLISION      | "<nick> :Nickname collision KILL"              |X(nick)
| 441          | ERR_USERNOTINCHANNEL   | "<nick> <channel> :They aren't on that channel"|
| 442          | ERR_NOTONCHANNEL       | "<channel> :You're not on that channel"        |O
| 443          | ERR_USERONCHANNEL      | "<user> <channel> :is already on channel"      |
| 444          | ERR_NOLOGIN            | "<user> :User not logged in"                   |
| 445          | ERR_SUMMONDISABLED     | ":SUMMON has been disabled"                    |
| 446          | ERR_USERSDISABLED      | ":USERS has been disabled"                     |
| 451          | ERR_NOTREGISTERED      | ":You have not registered"                     |O
| 461          | ERR_NEEDMOREPARAMS     | "<command> :Not enough parameters"             |O
| 462          | ERR_ALREADYREGISTRED   | ":You may not reregister"                      |
| 464          | ERR_PASSWDMISMATCH     | ":Password incorrect"                          |
| 467          | ERR_KEYSET             | "<channel> :Channel key already set"           |O
| 471          | ERR_CHANNELISFULL      | "<channel> :Cannot join channel (+l)"          |O
| 472          | ERR_UNKNOWNMODE        | "<char> :is unknown mode char to me"           |O
| 473          | ERR_INVITEONLYCHAN     | "<channel> :Cannot join channel (+i)"          |O
| 475          | ERR_BADCHANNELKEY      | "<channel> :Cannot join channel (+k)"          |O
| 481          | ERR_NOPRIVILEGES       | ":Permission Denied- You're not an IRC operator" |
| 482          | ERR_CHANOPRIVSNEEDED   | "<channel> :You're not channel operator"       |O
| 501          | ERR_UMODEUNKNOWNFLAG   | ":Unknown MODE flag"                           |X(mode)
| 502          | ERR_USERSDONTMATCH     | ":Cant change mode for other users"            |X(mode)
