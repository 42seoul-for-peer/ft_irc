#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char** argv) {

	if (argc != 3)
		return 0;
	
	int port = atoi(argv[1]);
	std::string password = argv[2];

	int serv_sock, clnt_sock;
	struct sockaddr_in serv_addr, clnt_addr;
	unsigned int len;
	int buf_len;

	char rcvBuffer[BUFSIZ];

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);

	if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		std::cout << "bind error!" << std::endl;
		return 1;
	}

	if (listen(serv_sock, 5) < 0) {
		std::cout << "listen error!" << std::endl;
		return 1;
	}

	while (1) {
		len = sizeof(clnt_addr);
		clnt_sock = accept(serv_sock, (struct sockaddr *) &clnt_addr, &len);

		buf_len = read(clnt_sock, rcvBuffer, sizeof(rcvBuffer));
		rcvBuffer[buf_len] = '\0';
		write(clnt_sock, rcvBuffer, buf_len);

		close (clnt_sock);
	}

	close (serv_sock);

}