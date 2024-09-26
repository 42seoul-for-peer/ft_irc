#include "Command.hpp"
#include <iomanip>

const std::string	Command::_genMsg(int rpl_no) const {
	std::string msg = _genPrefix(rpl_no);

	if (rpl_no < 400)
		msg += _genRplMsg(rpl_no);
	else
		msg += _genErrMsg(rpl_no);
	return (msg);
}

const std::string	Command::_genMsg(int rpl_no, const std::string& param) const {
	std::string	msg = _genPrefix(rpl_no);

	msg += param + " ";
	if (rpl_no < 400)
		msg += _genRplMsg(rpl_no);
	else
		msg += _genErrMsg(rpl_no);
	return (msg);
}

const std::string	Command::_genMsg(int rpl_no, const std::string& param1, const std::string& param2) const {
	std::string	msg = _genPrefix(rpl_no);

	msg += param1 + " " + param2 + " ";
	if (rpl_no < 400)
		msg += _genRplMsg(rpl_no);
	else
		msg += _genErrMsg(rpl_no);
	return (msg);
}

const std::string	Command::_genPrefix(int rpl_no) const {
	std::string			prefix = ":";
	std::stringstream	ss;

	ss << std::setw(3) << std::setfill('0') << rpl_no;
	if (rpl_no)
		prefix += "irc.local " + ss.str() + " " + _send_nick + " ";
	else
		prefix += _send_nick + "!" + _send_user + "@" + _send_addr + " ";
	return (prefix);
}

const std::string	Command::_genRplMsg(int rpl_no) const {
	switch(rpl_no) {
		case RPL_WELCOME:
			return (":Welcome to the Localnet Relay Network " + _send_nick + "!" + _send_user + "@" + _send_addr + "\r\n");
		case RPL_NOTOPIC:
			return (":No topic is set\r\n");
		case RPL_ENDOFNAMES:
			return (":End of /NAMES list\r\n");
		default:
			return ("\r\n");
	}
}

const std::string	Command::_genErrMsg(int rpl_no) const {
	switch (rpl_no) {
		case ERR_NOSUCHNICK:
			return (":No such nick/channel\r\n");
		case ERR_NOSUCHCHANNEL:
			return (":No such channel\r\n");
		case ERR_CANNOTSENDTOCHAN:
			return (":Cannot sent to channel\r\n");
		case ERR_TOOMANYCHANNELS:
			return (":You have joined too many channels\r\n");
		case ERR_NORECIPIENT:
			return (":No recipient given (" + _cmd + ")\r\n");
		case ERR_NOTEXTTOSEND:
			return (":No text to send\r\n");
		case ERR_UNKNOWNCOMMAND:
			return (":Unknown command\r\n");
		case ERR_NONICKNAMEGIVEN:
			return (":No nickname given\r\n");
		case ERR_ERRONEUSNICKNAME:
			return (":Erroneus nickname\r\n");
		case ERR_NICKNAMEINUSE:
			return (":Nickname is already in use\r\n");
		case ERR_USERNOTINCHANNEL:
			return (":They aren't on that channel\r\n");
		case ERR_NOTONCHANNEL:
			return (":You're not on channel\r\n");
		case ERR_USERONCHANNEL:
			return (":is already on channel\r\n");
		case ERR_NOTREGISTERED:
			return (":You have not registered\r\n");
		case ERR_NEEDMOREPARAMS:
			return (":Not enough parameters\r\n");
		case ERR_ALREADYREGISTRED:
			return (":You may not reregister\r\n");
		case ERR_KEYSET:
			return (":Channel key already set\r\n");
		case ERR_CHANNELISFULL:
			return (":Cannot join channel (+l)\r\n");
		case ERR_UNKNOWNMODE:
			return (":is unknown mode char to me\r\n");
		case ERR_INVITEONLYCHAN:
			return (":Cannot join channel (+i)\r\n");
		case ERR_BADCHANNELKEY:
			return (":Cannot join channel (+k)\r\n");
		case ERR_BADCHANMASK:
			return (":Invalid channel name\r\n");
		case ERR_CHANOPRIVSNEEDED:
			return (":You're not channel operator\r\n");
		default:
			return (":Unknown error\r\n");
	}
}