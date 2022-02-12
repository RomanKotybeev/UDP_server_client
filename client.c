#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>

#include "utils/confsocket.h"
#include "lengthdata.h"

struct ClientData {
	char msg[max_msg_len];
};

int main(int argc, char **argv)
{
	int sd;
	int port;
	char *ip_serv;
	char *ip_my;
	int sz_send;
	int sz_recv;
	struct ClientData cl;
	socklen_t servaddrlen;
	struct sockaddr_in myaddr;
	struct sockaddr_in servaddr;
	struct timeval tv;
	char buf[max_msg_len];

	if (argc < 5) {
		fprintf(stderr, "Provide agruments: server addr, your address, port, messsage\n");
		return 1;
	}
	ip_serv = argv[1];
	ip_my = argv[2];
	port = atoi(argv[3]);
	strncpy(cl.msg, argv[4], max_msg_len);
	
	sd = confaddr(ip_my, port, &myaddr, 1);
	confaddr(ip_serv, port, &servaddr, 0);

	sz_send = sendto(sd, cl.msg, strlen(cl.msg), 0,
	                 (struct sockaddr *)&servaddr, sizeof(servaddr));
	if(sz_send < 0) {
		perror("Failed to send msg");
		close(sd);
		exit(EXIT_FAILURE);
	}

	/* Wait for response only `tv_sec` seconds */
	tv.tv_sec = 15;
	setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	
	sz_recv = recvfrom(sd, buf, max_msg_len, 0,
	                   (struct sockaddr *)&servaddr, &servaddrlen);
	if (sz_recv < 0) {
		perror("Failed to recieve message from server");
		close(sd);
		exit(EXIT_FAILURE);
	}

	printf("%s\n", buf);

	return 0;
}
