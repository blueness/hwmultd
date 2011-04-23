
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
#include <mserver.h>

int
mserver_start()
{
	struct hostent *host;
	struct in_addr maddr;
	struct ip_mreq mreq;

	if((host = gethostbyname(site_ip)) == NULL) 
	{
		write_log(CRIT,"invalid IP\n");
		return 0;
	}

	memcpy( &maddr, host->h_addr_list[0], host->h_length );
	if( !IN_MULTICAST(ntohl(maddr.s_addr)) )
	{
		write_log(CRIT,"non-multicast IP");
		return 0;
	}

	if((sd = socket( AF_INET, SOCK_DGRAM, 0 )) < 0)
	{
		write_log(CRIT,"socket creation failed\n");
		return 0;
	}

	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_port = htons(port);  
	if(bind( sd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0)
	{
		write_log(CRIT,"socket bind failed");
		return 0;
	}

	mreq.imr_multiaddr.s_addr = maddr.s_addr;
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	if(setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *) &mreq, sizeof(mreq)))
	{
		write_log(CRIT,"cannot join group");
		return 0;
	}

	return 1;
}


char *
recv_mcast_msg()
{
	struct sockaddr_in caddr;
	char *msg = (char *)malloc(MSG_BUFFER);

	int len = sizeof(caddr);
	if(recvfrom( sd, msg, MSG_BUFFER, 0, (struct sockaddr *) &caddr, (socklen_t *) &len ) < 0)
		return NUL;
	else
		return msg;

	/*
	if(sendto(sd, msg, strlen(msg)+1, 0, (struct sockaddr *) &saddr, sizeof(saddr)) < 0)
	{
		write_log(ERRO,"msg sendto multicast addr failed");
		return 0;
	}
	*/
}
