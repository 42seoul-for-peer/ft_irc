#include "Command.hpp"

const std::string	Command::_genErrMsg(int rpl_no) const {
	switch (rpl_no) {
		case ERR_NOSUCHNICK:
			return (":No such nick/channel\n");
		case ERR_NOSUCHCHANNEL:
			return (":No such channel\n");
		case ERR_CANNOTSENDTOCHAN:
			return (":Cannot sent to channel\n");
		case ERR_TOOMANYCHANNELS:
			return (":You have joined too many channels\n");
		case ERR_WASNOSUCHNICK:
			return (":There was no such nickname\n");
		case ERR_TOOMANYTARGETS:
			return (":Duplicate recipients. No message delivered\n");
		case ERR_NOORIGIN:
			return (":No origin specified\n");
		case ERR_NORECIPIENT:
			return (":No recipient given (" + _cmd + ")\n");
		case ERR_NOTEXTTOSEND:
			return (":No text to send\n");
		case ERR_UNKNOWNCOMMAND:
			return (":Unknown command\n");
		case ERR_NOMOTD:
			return (":MOTD File is missing\n");
		case ERR_NONICKNAMEGIVEN:
			return (":No nickname given\n");
		case ERR_ERRONEUSNICKNAME:
			return (":Erroneus nickname\n");
		case ERR_NICKNAMEINUSE:
			return (":Nickname is already in use\n");
		case ERR_USERNOTINCHANNEL:
			return (":They aren't on that channel\n");
		case ERR_USERONCHANNEL:
			return (":is already on channel\n");
		case ERR_NOTREGISTERED:
			return (":You have not registered\n");
		case ERR_NEEDMOREPARAMS:
			return (":Not enough parameters\n");
		case ERR_ALREADYREGISTRED:
			return (":You may not reregister\n");
		case ERR_KEYSET:
			return (":Channel key already set\n");
		case ERR_CHANNELISFULL:
			return (":Cannot join channel (+l)\n");
		case ERR_UNKNOWNMODE:
			return (":is unknown mode char to me\n");
		case ERR_INVITEONLYCHAN:
			return (":Cannot join channel (+i)\n");
		case ERR_BADCHANNELKEY:
			return (":Cannot join channel (+k)\n");
		case ERR_NOPRIVILEGES:
			return (":Permission Denied- You're not an IRC operator\n");
		case ERR_CHANOPRIVSNEEDED:
			return (":You're not channel operator\n");
		default:
			return (":Unknown Error\n");
	}
}