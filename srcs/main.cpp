#include <iostream>
#include "Server.hpp"

int main(int argc, char *argv[])
{
	try {
		if (argc != 3)
			throw (std::runtime_error("usage: ./irsserv [port] [password]"));
		Server serv(argv[1], argv[2]);
		serv.serverInit();
		serv.serverProcess();
	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
		return (1);
	}
	return (0);
}