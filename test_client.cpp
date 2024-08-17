#include <unistd.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string>

void error_msg(std::string msg) {
	std::cerr << msg << std::endl;
	exit(1);
}

int main(int argc, char** argv) {

	if (argc != 3)
		return 0;
	
	int port_num = atoi(argv[1]);
	std::string msg = argv[2];

	int clnt_sock;
	struct sockaddr_in c_addr;
	unsigned int len;
	int buf_len, rcv_len;

	clnt_sock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&c_addr, 0, sizeof(c_addr));
	c_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	c_addr.sin_family = AF_INET;
	c_addr.sin_port = htons(port_num);

	if (connect(clnt_sock, (struct sockaddr *) &c_addr, sizeof(c_addr)) < 0) {
		std::cout << "connect error!" << std::endl;
		return 1;
	} else
		std::cout << "connected..." << std::endl;
	
	while (1) {
		char sndBuffer[BUFSIZ], rcvBuffer[BUFSIZ];

		fputs("input: ", stdout);
		fgets(sndBuffer, BUFSIZ, stdin);
		if (!strcmp(sndBuffer, "q\n") || !strcmp(sndBuffer, "Q\n"))
			break;

		buf_len = write(clnt_sock, sndBuffer, strlen(sndBuffer));
		rcv_len = 0;

	while (rcv_len < buf_len) {
		int rcv_cnt = read(clnt_sock, rcvBuffer, sizeof(rcvBuffer));
		if (rcv_cnt < 0)
			error_msg("read error!");
		rcv_len += rcv_cnt;
	}

		rcvBuffer[buf_len] = '\0';
		std::cout << "Server echo...: " << rcvBuffer << std::endl;

	}

	close(clnt_sock);
}