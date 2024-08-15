#include <unistd.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string>

int main(int argc, char** argv) {

	if (argc != 3)
		return 0;
	
	int port_num = atoi(argv[1]);
	std::string msg = argv[2];

	int clnt_sock;
	struct sockaddr_in c_addr;
	unsigned int len;
	int buf_len;

	clnt_sock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&c_addr, 0, sizeof(c_addr));
	c_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	c_addr.sin_family = AF_INET;
	c_addr.sin_port = htons(port_num);

	if (connect(clnt_sock, (struct sockaddr *) &c_addr, sizeof(c_addr)) < 0) {
		std::cout << "connect error!" << std::endl;
		return 1;
	}
	
	char rcvBuffer[BUFSIZ];

	write(clnt_sock, argv[2], sizeof(argv[2]));
	buf_len = read(clnt_sock, rcvBuffer, sizeof(rcvBuffer));

	rcvBuffer[buf_len] = '\0';
	std::cout << "Server echo...: " << rcvBuffer << std::endl;

	close(clnt_sock);
}