#ifndef CONFSOCKET_H_ENTRY
#define CONFSOCKET_H_ENTRY
#include <netinet/in.h>

/* Specify an IP-address and port.
 * Functions creates a socket if `need_socket` provided,
 * in this case, a socket descriptor is returned.
 */
int confaddr(char *ip_addr, int port, struct sockaddr_in *addr, int need_socket);
#endif
