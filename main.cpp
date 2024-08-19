#include <iostream>

#include <sys/socket.h>
#include <sys/event.h>  // to use kqueue
#include <arpa/inet.h>
#include <cstdlib>      // to use atoi
#include <cstring>      // to use memset
#include <vector>

void error_msg(std::string msg) {
	std::cerr << "\033[1;31m" <<msg << "\033[0m" <<std::endl;
	exit(1);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cout << "usage: ./irsserv [port] [password]" << std::endl;
        return (1);
    }
    std::vector<struct kqevent> kqEvents;
    struct kevent       event;
    struct sockaddr_in  srvAddr;
    struct sockaddr_in  clntAddr;
    int                 newEvent;
    int                 eventFd;
    int                 srvSock;
    int                 kq;

    if ((srvSock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        error_msg("socket error");
        
    memset(&srvAddr, 0, sizeof(srvAddr));
    srvAddr.sin_family = AF_INET;
    // INADDR_ANY = 0.0.0.0 = localhost = 127.0.0.1
    srvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int port = std::atoi(argv[1]);
    if (port == 0)
        error_msg("port error");
    srvAddr.sin_port = htons(port);

    if (bind(srvSock, (struct sockaddr*)&srvAddr, sizeof(srvAddr)))
        error_msg("bind error");
    
    if (listen(srvSock, 10) == -1)
		error_msg("listen error");

    // kqueue turn
    kq = kqueue();
    if (kq < 0)
        error_msg("kqueue error");
    EV_SET(&event, srvSock, EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(kq, &event, 1, NULL, 0, NULL) == -1)
        error_msg("kevent error");
    while (1)
    {
        newEvent = kevent(kq, NULL, 0, &event, 1, NULL);
        if (newEvent == -1)
            error_msg("kevent error");
        for (int i = 0; i < newEvent; i++)
        {
            eventFd = kqEvents[i].ident;
            // disconnected
            if (kqEvents[i].flags & EV_EOF)
            {
                std::cout << "Client " << eventFd << "has disconnected" << std::endl;
                close(event_fd);
            }
            // new connection
            else if (event_fd == srvSock)
        }

    }
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

	// struct kevent* kq_events;
	std::vector<struct kevent> kq_events;
	struct kevent event;

	int kq, result, event_cnt;

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);

	if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		error_msg("bind error!");
	if (listen(serv_sock, 5) < 0)
		error_msg("listen error!");

	kq = kqueue(); // event queue 생성
	if (kq < 0)
		error_msg("kqueue error!");
	
	EV_SET(&event, serv_sock, EVFILT_READ, EV_ADD, 0, 0, NULL);
	result = kevent(kq, &event, 1, NULL, 0, NULL);
	if (result < 0)
		error_msg("kevent error!");

	while (1) {
		event_cnt = kevent(kq, NULL, 0, &(kq_events[0]), 1, NULL);
		if (event_cnt < 0) {
			std::cout << "kevent error!" << std::endl;
			break;
		}

		for (int i = 0; i < event_cnt; i++) {
			if (kq_events[i].ident == serv_sock) {
				adr_sz = sizeof(clnt_addr);
				clnt_sock = accept(serv_sock, (struct sockaddr *) &clnt_addr, &adr_sz);
				
				EV_SET(&event, clnt_sock, EVFILT_READ, EV_ADD, 0, 0, NULL);
				result = kevent(kq, &event, 1, NULL, 0, NULL);
				if (result < 0)
					break ;

				std::cout << "connected client: " << clnt_sock << std::endl;
			} else {
				buf_len = read(kq_events[i].ident, rcvBuffer, sizeof(rcvBuffer));
				if (buf_len == 0) {
					EV_SET(&event, kq_events[i].ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
					result = kevent(kq, &event, 1, NULL, 0, NULL);
					close(kq_events[i].ident);
					if (result < 0)
						break ;
				
					std::cout << "closed client: " << kq_events[i].ident << std::endl;
				} else {
					write(kq_events[i].ident, rcvBuffer, buf_len);
				}
			}
		}
	}
	// while (kq_events.size()) {
	// 	kq_events.pop_back();
	// }
	close (serv_sock);
	close (kq);
	return 0;

}