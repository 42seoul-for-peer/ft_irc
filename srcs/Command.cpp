#include <iostream>
#include "Command.hpp"

// OCCF
Command::~Command() {
	// std::cout << "Default destructor called for Command.\n";
}

// parameterized constructor
Command::Command(std::stringstream& input_cmd) {
	// std::cout << "Parameterized constructor called for Command.\n";
	std::string token;
	const int	len = _cmd.length();

	std::getline(input_cmd, token, ' ');
	_cmd = token;
	for (int i = 0; i < len; i++) {
		if (islower(_cmd[i]))
			_cmd[i] = toupper(_cmd[i]);
	}
	while (std::getline(input_cmd, token, ' ')){
		_args.push(token);
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
		it->second += msg;
	}
}
// usable function
void	Command::parse(int clnt_fd, Server& serv) {
	// _sender = serv.clnt_list.find(clnt_fd); << pass 때문에 각 명령어에서 처리해야됨
	Client&	send_clnt = *(serv.getClients().find(clnt_fd)->second);
	_sender = send_clnt.getNickname();
	_send_nick = send_clnt.getNickname();
	_send_user = send_clnt.getUsername();

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
	else if (_cmd == "QUIT")
		quit(serv);
	else if (_cmd != "")
		unknownCommand(serv);
}

std::string	Command::execute() {
	return (std::string("empty"));
}

std::vector<std::string> Command::_parsebyComma(std::queue< std::string >& _args) {
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