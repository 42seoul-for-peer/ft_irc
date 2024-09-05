#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# define MODE_K 0b00001
# define MODE_L 0b00010
# define MODE_I 0b00100
# define MODE_T 0b01000
# define MODE_P 0b10000

# include <string>
# include <vector>
# include "Client.hpp"
# include "Server.hpp"

class Client;

class Channel {
// OCCF
 public:
	Channel(std::string title, Client* client);
	~Channel();
 private:
	Channel();
	Channel(const Channel& copy);
	Channel& operator = (const Channel& copy);

// MEMBER VARIABLE
 private:
	std::vector< std::pair< bool, Client* > > _clients;
    std::vector< std::string >              _invited_clients;
	std::string	_title;
	std::string	_topic;
	std::string	_passwd;
    int			_max_clients;
	int			_mode;

// MEMBER FUNCITON
 public:
	// SETTER
	void setTopic(std::string& str);
	void setPasswd(std::string& str);
	void setMaxClients(int& num);
	void setMode(const bool& status, const int& changedMode);

	// GETTER
	const std::string& getTitle(void) const;
	const std::string& getTopic(void) const;
	const std::string& getPasswd(void) const;
	const std::vector< std::string >& getInvitedClients(void) const;
	const std::vector< std::pair< bool, Client* > >& getClients() const;
	const int& getMaxClients(void) const;
	const int& getMode(void) const;
	
	// HANDLING CLIENT
	void addClient(std::pair< bool, Client* > new_client);
	void leaveClient(const Client& client);
	void addInvitedClient(std::string& name);
	void rmInvitedClients(std::string& name);
	bool isChannelMember(const std::string name) const;
};

#endif