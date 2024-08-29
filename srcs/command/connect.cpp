#include "Command.hpp"

void	Command::connect(Server& serv) {
	if (_args.size() < 1) {
		_rep_no = 461; // ERR_NEEDMOREPARAMS
		return ;
	}
}