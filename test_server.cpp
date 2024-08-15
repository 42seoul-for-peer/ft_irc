#include <sys/_types/_socklen_t.h>
#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>

void error_msg(std::string msg) {
	std::cerr << msg << std::endl;
	exit(1);
}

int main(int argc, char** argv) {

	if (argc != 3)
		return 0;
	
	int port = atoi(argv[1]);
	std::string password = argv[2];

	int serv_sock, clnt_sock;
	struct sockaddr_in serv_addr, clnt_addr;
	struct timeval timeout;
	fd_set reads, cpy_reads;

	socklen_t adr_sz;
	int buf_len, fd_max, fd_num;

	char rcvBuffer[BUFSIZ];

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);

	if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		error_msg("bind error!");
	if (listen(serv_sock, 5) < 0)
		error_msg("listen error!");

	FD_ZERO(&reads);
	FD_SET(serv_sock, &reads);
	fd_max = serv_sock;

	while (1) {
		cpy_reads = reads;
		timeout.tv_sec = 5;
		timeout.tv_usec = 5000;

		fd_num = select(fd_max + 1, &cpy_reads, 0, 0, &timeout);
		if (fd_num < 0)
			break;
		if (fd_num == 0)
			continue;

		for (int i = 0; i < fd_max + 1; i++) {
			if (FD_ISSET(i, &cpy_reads)) {
				if (i == serv_sock) {
					adr_sz = sizeof(clnt_addr);
					clnt_sock = accept(serv_sock, (struct sockaddr *) &clnt_addr, &adr_sz);
					
					FD_SET(clnt_sock, &reads);
					if (fd_max < clnt_sock)
						fd_max = clnt_sock;
					std::cout << "connected client: " << clnt_sock << std::endl;
				} else {
					buf_len = read(clnt_sock, rcvBuffer, sizeof(rcvBuffer));
					if (buf_len == 0) {
						FD_CLR(i, &reads);
						close(i);
						std::cout << "closed client: " << i << std::endl;
					} else {
						write(i, rcvBuffer, buf_len);
					}
				}
			}
		}
	}

	close (serv_sock);
	return 0;

}