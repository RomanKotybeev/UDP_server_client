#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "utils/confsocket.h"
#include "lengthdata.h"

/* An object of this type will store clients metadata.
 * metadata[] will be stored in a given format (see callback func)
 */
struct MetaClientData {
	unsigned long n_clients; /* The number of clients */
	int received_msg_len; /* The length of the recieved message */
	char metadata[max_metadata_len];
	int metadata_size;
};

void create_deamon(char *ipaddr, int port)
{
	int pid;

	close(0);
	close(1);
	close(2);
	open("/dev/null", O_RDONLY);
	open("/dev/null", O_WRONLY);
	open("/dev/null", O_WRONLY);
	chdir("/");
	pid = fork();
	if (pid > 0)
		exit(0);
	setsid();
	pid = fork();
	if (pid > 0)
		exit(0);

	openlog("UPD_serverd", 0, LOG_USER);
	syslog(LOG_INFO, "UDP server started on %s:%d", ipaddr, port);
}

void write_metadata_callback(int clientmsg_len, void *clientdata)
{
	int cx;
	struct MetaClientData *mcd = clientdata;

	cx = snprintf(mcd->metadata, max_metadata_len,
	              "# of clients: %lu, # of recieved bytes: %d",
	              mcd->n_clients, clientmsg_len);
	if (cx < 0) {
		syslog(LOG_ERR, "Client message length is more than %d",
		       max_metadata_len);
		return ;
	}
	mcd->n_clients++;
	mcd->metadata_size = sizeof(mcd->metadata);
}

int response_to_msg(int sockfd, struct sockaddr_in *addr,
                    struct MetaClientData *mcd)
{
	int sz = sendto(sockfd, mcd->metadata, mcd->metadata_size, 0,
	                (struct sockaddr *)addr, sizeof(*addr));
	if (sz < 0) {
		syslog(LOG_ERR, "Failed sending to %s:%d",
		       inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));
	}
	return sz;
}

int receive_data(int sockfd, struct sockaddr_in *addr, socklen_t *len,
                 struct MetaClientData *mcd)
{
	int msg_len;
	char buf[max_msg_len];

	msg_len = recvfrom(sockfd, buf, max_msg_len, 0, 
	                   (struct sockaddr *)addr, len);
	if (msg_len < 0) {
		syslog(LOG_ERR, "Failed receiving from %s:%d",
		       inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));
		return msg_len;
	}
	write_metadata_callback(msg_len, mcd);

	return msg_len;
}

int main(int argc, char **argv)
{
	int sd;
	int port;
	char *ip_addr;
	socklen_t len;
	struct sockaddr_in servaddr;
	struct sockaddr_in clientaddr;
	struct MetaClientData mcd;

	if (argc < 3) {
		fprintf(stderr, "Provide an address and port\n");
		return 1;
	}
	ip_addr = argv[1];
	port = atoi(argv[2]);
	sd = confaddr(ip_addr, port, &servaddr, 1);

	create_deamon(ip_addr, port);
	len = sizeof(clientaddr);
	mcd.n_clients = 0;
	for (;;) {
		receive_data(sd, &clientaddr, &len, &mcd);
		response_to_msg(sd, &clientaddr, &mcd);
	}
	closelog();
	return 0;
}
