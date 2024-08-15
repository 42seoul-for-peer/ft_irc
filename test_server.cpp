#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char** argv) {

	if (argc != 2)
		return 0;
	
	int port_num = atoi(argv[1]);

	int s_sock, c_sock;
	struct sockaddr_in s_addr, c_addr;
	unsigned int len;
	int n;

	char rcvBuffer[BUFSIZ];

	s_sock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&s_addr, 0, sizeof(s_addr));
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(port_num);

	bind(s_sock, (struct sockaddr *)&s_addr, sizeof(s_addr));

	listen(s_sock, 5);

	while (1) {
		len = sizeof(c_addr);
		c_sock = accept(s_sock, (struct sockaddr *) &c_addr, &len);

		n = read(c_sock, rcvBuffer, sizeof(rcvBuffer));
		rcvBuffer[n] = '\0';
		write(c_sock, rcvBuffer, n);

		close (c_sock);
	}

	close (s_sock);

}