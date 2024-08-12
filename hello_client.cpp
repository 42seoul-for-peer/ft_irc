#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string>

void	error_handling(const std::string& msg);

int main(int argc, char** argv) {
	int					sock;
	struct sockaddr_in	serv_addr;
	std::string			msg(30, 0);
	int					str_len;

	if (argc != 3) {
		printf("Usage: %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		error_handling("socket() error");
	
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	if (connect(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error");
	
	str_len = read(sock, (void*) msg.c_str(), sizeof(msg) - 1);
	if (str_len == -1)
		error_handling("read() error");
	
	printf("Msg from server: %s\n", msg.c_str());
	close(sock);

	return (0);
}

void	error_handling(const std::string& msg) {
	fputs(msg.c_str(), stderr);
	fputc('\n', stderr);
	exit(1);
}