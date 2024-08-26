#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <vector>
# include "Client.hpp"
# include "Server.hpp"

class Channel {
// OCCF
 public:
	// 최초로 join 사용했을 때 이름, 사용자
	Channel(const std::string& name, Client& client);
	~Channel();
 private:
	Channel();
	Channel(const Channel& copy);
	Channel& operator = (const Channel& copy);
// MEMBER VARIABLE
 private:
	// int: operator 여부, client 정보
	std::vector< std::pair<bool, Client&> >	_users;
	std::string				_name;
	std::string				_topic;
	std::string				_password;
	int						_max_user;
		// default -1 (overflow 조심) mode flag 보고 set하기
	int						_mode;
	std::vector< std::string > _invited_user; // 명령어 동작 한 번 확인하고 필요한지 검토

// MEMBER FUNCITON

};

#endif
