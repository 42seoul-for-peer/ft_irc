#include <iostream>
#include "Channel.hpp"

// OCCF
Channel::~Channel() {
	// std::cout << "Default destructor called for Channel.\n";
}

// MEMBER FUNCTION
Channel::Channel(std::string title)
    : _title(title), _max_clients(-1), _mode(0) {}

void Channel::setTopic(std::string& str) {
    _topic = str;
}

void Channel::setPasswd(std::string& str) {
    _passwd = str;
}

void Channel::setMaxClients(int& num) {
    _max_clients = num;
}

void Channel::setMode(int& bitNum) {
    _mode = bitNum;
}

const std::string& Channel::getTitle(void) const {
    return (_title);
}

const std::string& Channel::getTopic(void) const {
    return (_topic);
}

const std::string& Channel::getPasswd(void) const {
    return (_passwd);
}

const std::vector< std::pair<bool, std::string> >& Channel::getClients() const {
    return (_clients);
}


const int& Channel::getMaxClients(void) const {
    return (_max_clients);
}

const int& Channel::getMode(void) const {
    return (_mode);
}

void Channel::addClient(const Client& client) {
    _clients.push_back(std::pair<bool, std::string>(false, client.getUsername()));
}

void Channel::leaveClient(const Client& client) {
    std::vector< std::pair<bool, std::string> >::iterator it = _clients.begin();

    while (it != _clients.end())
    {
        if (it->second == client.getUsername())
            break;
        it++;
    }
    if (it != _clients.end())
        _clients.erase(it);
}

void Channel::addInvitedClient(std::string& name) {
    if (std::find(_invited_clients.begin(), _invited_clients.end(), name) != _invited_clients.end())
        _invited_clients.push_back(name);
}