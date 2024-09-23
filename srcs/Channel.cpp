#include <iostream>
#include "Channel.hpp"

// OCCF
Channel::~Channel() {
	// std::cout << "Default destructor called for Channel.\n";
}

// MEMBER FUNCTION
Channel::Channel(std::string title, Client* client)
: _mode(MODE_T), _max_clients(-1), _title(title) {
    _clients.push_back(std::pair< bool, Client* >(true, client));
    client->joinChannel(*this);
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

void Channel::setMode(const bool& status, const int& changed_mode) {
    // flag가 + 상태
    if (status == true) {
        _mode |= changed_mode;
    }
    // flag가 - 상태
    else {
        _mode ^= changed_mode;
    }
}

void Channel::setOperator(const bool& status, const std::string& clnt_name)
{
    int clnt_size = _clients.size();
    int idx;
    
    for (idx = 0; idx < clnt_size; idx++) {
        if (_clients[idx].second->getNickname() == clnt_name)
            break ;
    }
    if (idx == clnt_size)
        return ;
    else
        _clients[idx].first = status;
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

const std::vector< std::string >& Channel::getInvitedClients(void) const {
    return (_invited_clients);
}

const std::vector< std::pair< bool, Client* > >& Channel::getClients() const {
    return (_clients);
}

const std::string Channel::printClientsList(void) const {
    std::string clients_list;

    if (_clients.empty())
        return ("");
    int size = _clients.size();
    for (int i = 0; i < size; i++) {
        clients_list += _clients[i].second->getNickname();
        if (i != size - 1)
            clients_list += " ";
    }
    return (clients_list);
}

const int& Channel::getMaxClients(void) const {
    return (_max_clients);
}

const int& Channel::getMode(void) const {
    return (_mode);
}

void Channel::addClient(std::pair< bool, Client* > new_client) {
    new_client.second->joinChannel(*this);
    _clients.push_back(new_client);
}

void Channel::deleteClient(const Client& client) {
    std::vector< std::pair< bool, Client* > >::iterator it = _clients.begin();

    while (it != _clients.end()) {
        if (it->second->getNickname() == client.getNickname())
            break;
        it++;
    }
    if (it != _clients.end())
        _clients.erase(it);
}

void Channel::addInvitedClient(std::string& name) {
    if (std::find(_invited_clients.begin(), _invited_clients.end(), name) == _invited_clients.end())
        _invited_clients.push_back(name);
}

void Channel::rmInvitedClients(std::string& name) {
    if (std::find(_invited_clients.begin(), _invited_clients.end(), name) != _invited_clients.end())
        _invited_clients.erase(std::find(_invited_clients.begin(), _invited_clients.end(), name));
}

bool Channel::isChannelMember(const std::string name) const {
    std::vector< std::pair< bool, Client* > >::const_iterator it = _clients.begin();

    while (it != _clients.end()) {
        if (name == it->second->getNickname())
            return (true);
        it++;
    }
    return (false);
}
