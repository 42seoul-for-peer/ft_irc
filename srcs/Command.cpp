#include <iostream>
#include "Command.hpp"

// OCCF
Command::~Command() {
}

// parameterized constructor
Command::Command(std::stringstream& input_cmd) {
	std::string token;

	std::getline(input_cmd, token, ' ');
	while (token == "")
		std::getline(input_cmd, token, ' ');
	_cmd = token;
	for (size_t i = 0; i < _cmd.length(); i++) {
		if (std::islower(_cmd[i])) {
			_cmd[i] = toupper(_cmd[i]);
		}
	}
	while (std::getline(input_cmd, token, ' ')){
		if (token == "")
			continue ;
		_args.push(token);
	}
}

// MEMBER FUNCTION
// getter
const std::string&	Command::getCmd() const {
	return (_cmd);
}

const std::map< std::string, std::string >& Command::getMsgs() const {
	return (_msgs);
}

// setter
void	Command::setMsgs(const std::string& name, const std::string& msg) {
	std::map< std::string, std::string >::iterator it = _msgs.find(name);
	if (it == _msgs.end()) {
		_msgs.insert(make_pair(name, msg));
	}
	else {
		if (it->second.find(msg) == std::string::npos)
			it->second += msg;
	}
}
// usable function
void	Command::parse(int clnt_fd, Server& serv) {
	Client&	send_clnt = *(serv.getClients().find(clnt_fd)->second);
	_send_nick = send_clnt.getNickname();
	_send_user = send_clnt.getUsername();
	_send_addr = send_clnt.getAddr();

	if (_cmd == "PASS")
		pass(send_clnt, serv);
	else if (_cmd == "NICK")
		nick(send_clnt, serv);
	else if (_cmd == "USER")
		user(send_clnt, serv);
	else if (_cmd == "JOIN")
		join(send_clnt, serv);
	else if (_cmd == "PRIVMSG")
		privmsg(serv);
	else if (_cmd == "MODE")
		mode(serv);
	else if (_cmd == "PART")
		part(send_clnt, serv);
	else if (_cmd == "KICK")
		kick(serv);
	else if (_cmd == "INVITE")
		invite(serv);
	else if (_cmd == "TOPIC")
		topic(serv);
	else if (_cmd == "QUIT")
		quit(serv);
	else if (_cmd == "PING") {
		setMsgs(_send_nick, ":irc.local PONG irc.local :irc.local\r\n");
	}
	else if (_cmd != "")
		unknownCommand();
}

std::string	Command::execute() {
	return (std::string("empty"));
}

std::vector<std::string> Command::_parsebyComma() {
    std::vector<std::string>    token_vec;

	if (_args.empty())
		return (std::vector< std::string >(0));
    std::stringstream           stream(_args.front());
    std::string                 token;

    while (std::getline(stream, token, ','))
        token_vec.push_back(token);
    return (token_vec);
}

const std::string Command::_appendRemaining() {
    if (_args.empty())
        return ("");

    std::string leaving_msg;

    while (!_args.empty())
    {
        leaving_msg += _args.front();
        _args.pop();
        if (!_args.empty())
            leaving_msg += " ";
    }

	if (leaving_msg[0] == ':' && leaving_msg.size() > 1)
		leaving_msg = leaving_msg.substr(1);
	else if (leaving_msg[0] == ':' && leaving_msg.size() == 1)
		leaving_msg = "";

    return (leaving_msg);
}