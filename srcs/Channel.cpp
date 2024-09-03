#include <iostream>
#include "Channel.hpp"

// OCCF
Channel::~Channel() {
	// std::cout << "Default destructor called for Channel.\n";
}

// MEMBER FUNCTION
Channel::Channel(std::string title, Client* client)
: _title(title), _max_clients(-1), _mode(0) {
    _clients.push_back(std::pair< bool, Client* >(true, client));
}

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

const std::vector< std::pair< bool, Client* > >& Channel::getClients() const {
    return (_clients);
}


const int& Channel::getMaxClients(void) const {
    return (_max_clients);
}

const int& Channel::getMode(void) const {
    return (_mode);
}

void Channel::addClient(std::pair< bool, Client* > new_client) {
    _clients.push_back(new_client);
}

void Channel::leaveClient(const Client& client) {
    std::vector< std::pair< bool, Client* > >::iterator it = _clients.begin();

    while (it != _clients.end())
    {
        if (it->second->getUsername() == client.getUsername())
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

bool Channel::isChannelMember(const std::string name) const {
    std::vector< std::pair< bool, Client* > >::const_iterator it = _clients.begin();

    while (it != _clients.end()) {
        if (name == it->second->getUsername())
            return (true);
        it++;
    }
    return (false);
}
