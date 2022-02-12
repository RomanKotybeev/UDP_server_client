#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "confsocket.h"

int createbind(struct sockaddr_in *addr, int addrlen)
{
	int connected;
	int sockfd;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		perror("Failed to create socket");
		exit(EXIT_FAILURE);
	}
	connected = bind(sockfd, (struct sockaddr *)addr, addrlen);
	if (connected < 0) {
		perror("Failed to bind a socket");
		close(sockfd);
		exit(EXIT_FAILURE);
	}
	return sockfd;
}

int confaddr(char *ip_addr, int port,
             struct sockaddr_in *addr, int need_socket)
{
	int ip_ok;

	addr->sin_family = AF_INET;
	addr->sin_port = htons(port);
	ip_ok = inet_aton(ip_addr, &(addr->sin_addr));
	if(!ip_ok) {
		perror(ip_addr);
		exit(EXIT_FAILURE);
	}

	if (need_socket)
		return createbind(addr, sizeof(*addr));

	return 1;
}
