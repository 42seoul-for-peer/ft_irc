#include "Command.cpp"
#include <string>

std::string	Command::_genErrMsg(int err_no) {
	if (err_no == ERR_NOSUCHNICK) {
		return ("No such nick");
	}
	else
		return ("some what error");
}