
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <log.h>
#include <cmdargs.h>
#include <mcast.h>

int
mserver_start()
{
	struct hostent *host;
	struct ip_mreq imreq;
	struct in_addr iaddr;

	memset(&saddr, 0, sizeof(struct sockaddr_in));
	memset(&iaddr, 0, sizeof(struct in_addr));
	memset(&imreq, 0, sizeof(struct ip_mreq));

	if((host = gethostbyname(site_ip)) == NULL) 
	{
		write_log(CRIT,"server invalid IP\n");
		return 0;
	}

	memcpy(&iaddr, host->h_addr_list[0], host->h_length);

	if( !IN_MULTICAST(ntohl(iaddr.s_addr)) )
	{
		write_log(CRIT,"server non-multicast IP");
		return 0;
	}

	if((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		write_log(CRIT,"server socket creation failed\n");
		return 0;
	}

	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_port = htons(port);  

	if(bind(sd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in)) < 0)
	{
		write_log(CRIT,"server socket bind failed");
		return 0;
	}

	imreq.imr_multiaddr.s_addr = iaddr.s_addr;
	imreq.imr_interface.s_addr = htonl(INADDR_ANY);

	if(setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const void *)&imreq, sizeof(struct ip_mreq)))
	{
		write_log(CRIT,"server cannot join group");
		return 0;
	}

	return 1;
}


char *
rcv_mcast_msg()
{
	char *msg = (char *)malloc(MSG_BUFFER);
	int len = sizeof(struct sockaddr_in);

	if(recvfrom(sd, msg, MSG_BUFFER, 0, (struct sockaddr *)&saddr, &len ) < 0)
		return NULL;
	else
		return msg;
}


int
mserver_stop()
{
	shutdown(sd, 2);
	close(sd);
}
