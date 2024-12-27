#include <stdio.h>          /* printf(), snprintf() */
#include <stdlib.h>         /* strtol(), exit() */
#include <sys/types.h>
#include <sys/socket.h>     /* socket(), setsockopt(), bind(), recvfrom(), sendto() */
#include <errno.h>          /* perror() */
#include <netinet/in.h>     /* IPPROTO_IP, sockaddr_in, htons(), htonl() */
#include <arpa/inet.h>      /* inet_addr() */
#include <unistd.h>         /* fork(), sleep() */
#include <sys/utsname.h>    /* uname() */
#include <string.h>         /* memset() */

#define MAXLEN 2048


int main(int argc, char* argv[]) {
	u_int yes = 1;      /* Used with SO_REUSEADDR. 
                             In Linux both u_int */
                         /* and u_char are valid. */
	int recv_s;     /* Sockets for sending and receiving. */
	struct sockaddr_in mcast_group;
	struct ip_mreq mreq;
	int n;
	unsigned int len;
	unsigned short port;
	struct sockaddr_in from;
	char message[MAXLEN+1];

	if (argc != 4) {
		fprintf(stderr, "Usage: %s mcast_group port int_addr\n", argv[0]);
		exit(1);
	}

	if (inet_addr(argv[1]) == (in_addr_t)-1) {
		fprintf(stderr, "mcast_group (%s) is not a valid ipv4 address\n", argv[1]);
		exit(1);
	}

	if (inet_addr(argv[3]) == (in_addr_t)-1) {
		fprintf(stderr, "int_addr (%s) is not a valid ipv4 address\n", argv[3]);
		exit(1);
	}

	port = strtol(argv[2], NULL, 0);
	if (port < 1 || port > 65536) {
		fprintf(stderr, "port (%s) is not a valid port number\n", argv[2]);
		exit(1);
	}

	memset(&mcast_group, 0, sizeof(mcast_group));
	mcast_group.sin_family = AF_INET;
	mcast_group.sin_port = htons(port);
	mcast_group.sin_addr.s_addr = inet_addr(argv[1]);


	if ((recv_s=socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror ("socket");
		exit(1);
	}

	if (setsockopt(recv_s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
		perror("setsockopt: cant reuse address");
		exit(1);
	}

	if (bind(recv_s, (struct sockaddr*)&mcast_group, sizeof(mcast_group)) < 0) {
		perror ("cant bind to mcast_group");
		exit(1);
	}

	/* Preparatios for using Multicast */ 
	mreq.imr_multiaddr = mcast_group.sin_addr;
	mreq.imr_interface.s_addr = inet_addr(argv[3]);

	/* Tell the kernel we want to join that multicast group. */
	if (setsockopt(recv_s, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
		perror ("no such interface address");
		exit(1);
	}

	while (1) {
		len = sizeof(from);
		if ((n = recvfrom(recv_s, message, MAXLEN, 0,
                                 (struct sockaddr*)&from, &len)) < 0) {
			perror ("recvfrom");
			exit(1);
		}

		write(STDOUT_FILENO, message, n);
	}

	return 0;
}
