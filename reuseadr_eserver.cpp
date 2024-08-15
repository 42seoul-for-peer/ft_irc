#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string>

#define TRUE 1
#define FALSE 0

void	error_handling(std::string msg) {
	fputs(msg.c_str(), stderr);
	fputc('\n', stderr);
	exit(1);
}

int main(int argc, char** argv) {
	int serv_sock, clnt_sock;
	char msg[30];
	int option, str_len;
	socklen_t optlen, clnt_adr_sz;
	struct sockaddr_in serv_adr, clnt_adr;
	if (argc != 2) {
		printf("Usage: %s <port> \n", argv[0]);
		exit(1);
	}

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (serv_sock == -1)
		error_handling("socket() error");

	/* time-wait 상태가 되지 않도록 설정하는 옵션으로, 종료 후 바로 해당 포트를 재사용하게 해줌
	optlen = sizeof(option);
	option = TRUE;
	secsocketopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (void*)&option, optlen);
	*/

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));

	if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)))
		error_handling("bind() error");
	if (listen(serv_sock, 5) == -1)
		error_handling("listen() error");
	clnt_adr_sz = sizeof(clnt_adr);
	clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);

	while((str_len = read(clnt_sock, msg, sizeof(msg))) != 0) {
		write(clnt_sock, msg, str_len);
		write(!1, msg, str_len);
	}
	close(clnt_sock);
	close(serv_sock);
	return 0;
}