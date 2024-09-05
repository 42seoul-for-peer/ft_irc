#include <iostream>
#include "Command.hpp"

// OCCF
Command::~Command() {
	// std::cout << "Default destructor called for Command.\n";
}

// parameterized constructor
Command::Command(std::stringstream& input_cmd) : _msg("") {
	// std::cout << "Parameterized constructor called for Command.\n";
	std::string tmp;

	input_cmd >> _cmd;
	while (input_cmd >> tmp) {
		_args.push(tmp);
	}
}

// MEMBER FUNCTION
// getter
const std::string&	Command::getCmd() const {
	return (_cmd);
}

const std::string&	Command::getSender() const {
	return (_sender);
}

const std::map< std::string, int >& Command::getReceiver() const {
	return (_receiver);
}


const std::string	Command::getProtoMsg(const std::pair< std::string, int>& recv, const std::string& target) const {
	// std::map< std::string, int>::const_iterator it = _receiver.find(recv_name);
	// // it == end()인 경우 처리 해야하나?
	// std::cout << "it->second in proto msg generator: " << recv_name << " : " << it->second << '\n';
	switch (recv.second) {
		case RPL_WELCOME:
			return (target + "Welcome to local irc server\n");
		case ERR_NOSUCHNICK:
			return (_sender + " " + target + " :No such nick\n");
		case ERR_NOSUCHCHANNEL:
			return (_sender + " " + target + " :No such channel\n");
		case ERR_CANNOTSENDTOCHAN:
			return (target + " :Cannot send to channel\n");
		case ERR_TOOMANYCHANNELS:
			return (target + " :You have joined too many channels\n");
		case ERR_WASNOSUCHNICK:
			return (target + " :There was no such nickname\n");
		case ERR_TOOMANYTARGETS:
			return (target + " :Duplicate recipients. No message delievered\n");
		case ERR_NOORIGIN:
			return (":No origin specified\n");
		case ERR_NORECIPIENT:
			return (":No recipient given (" + _cmd + ")\n");
		case ERR_NOTEXTTOSEND:
			return (":No text to send\n");
		case ERR_UNKNOWNCOMMAND:
			return (_cmd + " :Unknown command\n");
		case ERR_NOMOTD:
			return (":MOTD File is missing\n");
		case ERR_NONICKNAMEGIVEN:
			return (":No nickname given\n");
		case ERR_ERRONEUSNICKNAME:
			return (target + " :Erroneus nickname\n");
		case ERR_NICKNAMEINUSE:
			return (target + " :Nickname already in use\n");
		case ERR_NOTONCHANNEL:
			return (target + " :You're not on that channel\n");
		case ERR_NOTREGISTERED:
			return (":You have not registered\n");
		case ERR_NEEDMOREPARAMS:
			return (_cmd + " :Not enough parameters\n");
		case ERR_ALREADYREGISTRED:
			return (":You may not reregister\n");
		case ERR_PASSWDMISMATCH:
			return (":Password incorrect\n");
		case ERR_CHANNELISFULL:
			return (target + " :Cannot join channel (+l)\n");
		case ERR_NOPRIVILEGES:
			return (":Permission Denied- You're not an IRC operator\n");
		case ERR_CHANOPRIVSNEEDED:
			return (target + " :You're not channel operator\n");
		case ERR_UMODEUNKNOWNFLAG:
			return (":Unknown MODE flag\n");
		case ERR_USERSDONTMATCH:
			return (":Cant change mode for other users\n");
		case RPL_NONE:
			return ("Dummy reply number. Not used.\n");
		case RPL_YOUREOPER:
			return (":You are now an IRC operator\n");
		default:
			return (_cmd + " " + target + " " + _msg + "\tvia default case\n"); // 마지막 문자열은 디버깅 용도
	}
	// if (it->second > 399) {
	// 	reply += std::to_string(it->second);
	// 	reply += " " + _sender + " " + recv_name + " :"; // 마지막에 error message 들어가야 함
	// 	reply += _genErrMsg(it->second);
	// }
}

const std::string Command::getMsg() const {
	return (_msg);
}

// setter

// usable function
void	Command::parse(int clnt_fd, Server& serv) {
	// _sender = serv.clnt_list.find(clnt_fd); << pass 때문에 각 명령어에서 처리해야됨
	Client&	send_clnt = *(serv.getClients().find(clnt_fd)->second);
	_sender = send_clnt.getNickname();

	if (_cmd == "PASS")
		pass(send_clnt, serv);
	else if (_cmd == "NICK")
		nick(send_clnt, serv);
	else if (_cmd == "USER")
		user(send_clnt, serv);
	else if (_cmd == "JOIN")
		join(send_clnt, serv);
	else if (_cmd == "PRIVMSG")
		privmsg(send_clnt, serv);
	else if (_cmd == "MODE")
		mode(send_clnt, serv);
	else if (_cmd != "")
		unknownCommand(send_clnt, serv);
}

std::string	Command::execute() {
	return (std::string("empty"));
}