/*
 * MIT License
 *
 * Copyright (c) 2018 Lewis Van Winkle
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECT*ION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)

int main(int argc, char* argv[]) {

    printf("Configuring local address...\n");

    //socket setting
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    
    struct addrinfo *bind_address;
    getaddrinfo(0, argv[1], &hints, &bind_address);

    //create socket
    printf("Creating socket...\n");
    SOCKET socket_listen;
    socket_listen = socket(bind_address->ai_family,
            bind_address->ai_socktype, bind_address->ai_protocol);
    if (!ISVALIDSOCKET(socket_listen)) {
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    //socket option setting
    int flag = 1;
    int result = setsockopt(socket_listen,IPPROTO_TCP,TCP_NODELAY,(char*) &flag , sizeof(int));
    if ( result < 0){
        printf("setsockopt failed");
        return 0;
    }

    //socket binding
    printf("Binding socket to local address...\n");
    if (bind(socket_listen,
                bind_address->ai_addr, bind_address->ai_addrlen)) {
        fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
    freeaddrinfo(bind_address);

    //socket listen
    printf("Listening...\n");
    if (listen(socket_listen, 10) < 0) {
        fprintf(stderr, "listen() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    //select setting
    fd_set master;
    FD_ZERO(&master);
    FD_SET(socket_listen, &master);
    SOCKET max_socket = socket_listen;
    //printf("%d",socket_listen);

    printf("Waiting for connections...\n");

    //communicate with clients
    while(1) {

	//select
        fd_set reads;
        reads = master;
        if (select(max_socket+1, &reads, 0, 0, 0) < 0) {
            fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
            return 1;
        }

        SOCKET i;
        for(i = 1; i <= max_socket; ++i) {
            if (FD_ISSET(i, &reads))
            { //change detection
                if (i == socket_listen)
                { //new connect
                    
                    struct sockaddr_storage client_address;
                    socklen_t client_len = sizeof(client_address);
                    
                    // int b[] = "client 1 connected";
                    // b[7] =  socket_listen;
                    // printf("%s\n",b);

		    //accept connection
                    SOCKET socket_client = accept(socket_listen,
                            (struct sockaddr*) &client_address,
                            &client_len);
                    if (!ISVALIDSOCKET(socket_client))
                    {
                        fprintf(stderr, "accept() failed. (%d)\n",
                                GETSOCKETERRNO());
                        return 1;
                    }
                    else
                    {
                        printf("%d, %d;;\n",socket_listen, socket_client);
                        if (socket_client==5)
                        { //connected with two clients
			                //create random block to synchronize
                            char temp[1001];
                            for (int k = 0; k < 1000; k++)
                                temp[k] = (rand() % 7) + '0';
                            temp[1000] = '\0';
			                //send myTurn value and block to clients
                            char message4[1010];
                            char message5[1010];
                            sprintf(message4, "1%s", temp);
                            sprintf(message5, "0%s", temp);
                            int s4 = send(4, message4, strlen(message4), 0);
                            int s5 = send(5, message5, strlen(message5), 0);
                        }                        
                    }
		            //update fdset
                    FD_SET(socket_client, &master);
                    if (socket_client > max_socket)
                        max_socket = socket_client;
                    char address_buffer[100];
                    getnameinfo((struct sockaddr*)&client_address,
                            client_len,
                            address_buffer, sizeof(address_buffer), 0, 0,
                            NI_NUMERICHOST);
                    //printf("New connection from %s\n", address_buffer);
                }
		        else { //receive message from connected clients
		    //receive command
                    char read[1024];
                    int bytes_received = recv(i, read, 1024, 0);
                    read[bytes_received] = '\0';
                    printf("recved : %.*s\n", bytes_received, read);
                    if (bytes_received < 1) {
                        FD_CLR(i, &master);
                        CLOSESOCKET(i);
                        
                        continue;
                    }

		    //receive from client1
                    if(i==4){
		        //send to client2
                        send(5, read, bytes_received, 0);
                        printf("sended to 5: %s\n",read);
                    }
		    //receive from client2
                    else if(i==5){
		        //send to client1
                        send(4, read, bytes_received, 0);
                        printf("sended to 4: %s\n",read);
                    }
                }
            } //if FD_ISSET
        } //for i to max_socket
    } //while(1)



    printf("Closing listening socket…\n");
    CLOSESOCKET(socket_listen);

    printf("Finished.\n");

    return 0;
}
