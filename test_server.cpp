#include <sys/_types/_socklen_t.h>
#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>

#define EPOLL_SIZE 100

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

	socklen_t adr_sz;
	int buf_len;

	char rcvBuffer[BUFSIZ];

	struct epoll_event* ep_events;
	struct epoll_event event;
	int epfd, event_cnt;

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);

	if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		error_msg("bind error!");
	if (listen(serv_sock, 5) < 0)
		error_msg("listen error!");

	epfd = epoll_create(EPOLL_SIZE);
	ep_events = malloc(sizeof(struct epoll_event) * EPOLL_SIZE);

	event.events = EPOLLIN;
	event.data.fd = serv_sock;
	epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);

	while (1) {
		
		event_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
		
		if (event_cnt < 0) {
			std::cout << "epoll_wait error!" << std::endl;
			break;
		}

		for (int i = 0; i < event_cnt; i++) {
			if (ep_events[i].data.fd == serv_sock) {
				adr_sz = sizeof(clnt_addr);
				clnt_sock = accept(serv_sock, (struct sockaddr *) &clnt_addr, &adr_sz);
				
				event.events = EPOLLIN;
				event.data.fd = clnt_sock;
				epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);

				std::cout << "connected client: " << clnt_sock << std::endl;
			} else {
				buf_len = read(clnt_sock, rcvBuffer, sizeof(rcvBuffer));
				if (buf_len == 0) {
					epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
					close(i);
					std::cout << "closed client: " << i << std::endl;
				} else {
					write(i, rcvBuffer, buf_len);
				}
			}
		}
	}

	close (serv_sock);
	close (epfd);
	return 0;

}