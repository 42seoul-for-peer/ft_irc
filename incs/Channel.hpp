#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <vector>
# include "Client.hpp"
# include "Server.hpp"

class Channel {
// OCCF
 public:
	Channel(std::string title);
	~Channel();
 private:
	Channel();
	Channel(const Channel& copy);
	Channel& operator = (const Channel& copy);

// MEMBER VARIABLE
 private:
	std::vector< std::pair<bool, Client&> > _clients;
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
	void setMode(int& bitNum);

	// GETTER
	const std::string& getTopic(void) const;
	const std::string& getPasswd(void) const;
	const int& getMaxClients(void) const;
	const int& getMode(void) const;
	
	// HANDLING CLIENT
	void addClient(Client& client);
	void leaveClient(Client& client);
	void addInvitedClient(std::string& name);
};

#endif