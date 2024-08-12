#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string>

#define BUF_SIZE 1024

void	error_handling(const std::string& msg) {
	fputs(msg.c_str(), stderr);
	fputc('\n', stderr);
	exit(1);
}

int main(int argc, char **argv) {
	int sock;
	char msg[BUF_SIZE];
	int str_len;
	struct sockaddr_in serv_adr;

	if (argc != 3) {
		printf("Usage: %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		error_handling("socket() error");
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_adr.sin_port = htons(atoi(argv[2]));

	if (connect(sock, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) == -1)
		error_handling("connect() error");
	else
		puts("Connected ... ... ... ");
	
	while (1) {
		fputs("Input message(Q to quit): ", stdout);
		fgets(msg, BUF_SIZE, stdin);
		
		if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
			break ;

		write(sock, msg, strlen(msg));
		str_len = read(sock, msg, BUF_SIZE - 1);
		msg[str_len] = 0;
		printf("Message from server: %s", msg);
	}
	close(sock);
	return 0;
}