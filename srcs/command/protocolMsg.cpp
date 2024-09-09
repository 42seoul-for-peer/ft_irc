#include "Command.hpp"
#include <string>

const std::string	Command::_genProtoMsg(int rpl_no, const std::string& prefix) const {
	switch (rpl_no) {
		case ERR_NOORIGIN:
			return (prefix + ":No origin specified\n");
		case ERR_NORECIPIENT:
			return (prefix + ":No recipient given (" + _cmd + ")\n");
		case ERR_NOTEXTTOSEND:
			return (prefix + ":No text to send\n");
		case ERR_NOMOTD:
			return (prefix + ":MOTD File is missing\n");
		case ERR_NONICKNAMEGIVEN:
			return (prefix + ":No nickname given\n");
		case ERR_NOTREGISTERED:
			return (prefix + ":You have not registered\n");
		case ERR_ALREADYREGISTRED:
			return (prefix + ":You may not reregister\n");
		case ERR_PASSWDMISMATCH:
			return (prefix + ":Password incorrect\n");
		case ERR_UNKNOWNCOMMAND:
			return (prefix + _cmd + " :Unknown command\n");
		case ERR_NEEDMOREPARAMS:
			return (prefix + _cmd + " :Not enough parameters\n");
		case ERR_NOPRIVILEGES:
			return (prefix + " :Permission Denied- You're not an IRC operator\n");
		default:
			return (prefix + ":Unknown error\n");
	}
}
const std::string	Command::_genProtoMsg(int rpl_no, const std::string& prefix, const std::string& target) const {
	switch (rpl_no) {
		case RPL_WELCOME:
			return (prefix + target + " :Welcome to local irc server\n");
		case ERR_CANNOTSENDTOCHAN:
			return (prefix + target + " :Cannot send to channel\n");
		case ERR_TOOMANYCHANNELS:
			return (prefix + target + " :You have joined too many channels\n");
		case ERR_WASNOSUCHNICK:
			return (prefix + target + " :There was no such nickname\n");
		case ERR_TOOMANYTARGETS:
			return (prefix + target + " :Duplicate recipients. No message delievered\n");
		case ERR_ERRONEUSNICKNAME:
			return (prefix + target + " :Erroneus nickname\n");
		case ERR_NICKNAMEINUSE:
			return (prefix + target + " :Nickname already in use\n");
		case ERR_NOTONCHANNEL:
			return (prefix + target + " :You're not on that channel\n");
		case ERR_CHANNELISFULL:
			return (prefix + target + " :Cannot join channel (+l)\n");
		case ERR_CHANOPRIVSNEEDED:
			return (prefix + target + " :You're not channel operator\n");
		case ERR_KEYSET:
			return (prefix + target + " :Channel key already set\n");
		case ERR_UNKNOWNMODE:
			return (prefix + target + " :is unknown mode char to me\n");
		case ERR_INVITEONLYCHAN:
			return (prefix + target + " :Cannot join channel (+i)\n");
		case ERR_BADCHANNELKEY:
			return (prefix + target + " :Cannot join channel (+k)\n");
		default:
			return (prefix + target + " :Unknown error with one target\n");
	}
}
const std::string	Command::_genProtoMsg(int rpl_no, const std::string& prefix, const std::string& target1, const std::string& target2) const {
	switch (rpl_no) {
		case ERR_NOSUCHNICK:
			return (prefix + target1 + " " + target2 + " :No such nick/channel\n");
		case ERR_NOSUCHCHANNEL:
			return (prefix + target1 + " " + target2 + " :No such channel\n");
		case ERR_USERNOTINCHANNEL:
			return (prefix + target1 + " " + target2 + " :They aren't on that channel\n");
		default:
			return (prefix + target1 + " " + target2 + " :Unknown error with two target\n");
	}
}