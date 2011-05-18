
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include <cmdargs.h>
#include <log.h>
#include <mcast.h>

int cd;
struct sockaddr_in caddr;


int
mclient_start()
{
	struct hostent *host;
   	struct in_addr iaddr, aaddr;
	struct ifreq ifr;

	unsigned char ttl = 1;
	unsigned char loop = 1;

	memset(&caddr, 0, sizeof(struct sockaddr_in));
	memset(&iaddr, 0, sizeof(struct in_addr));
	memset(&aaddr, 0, sizeof(struct in_addr));
	memset(&ifr,   0, sizeof(struct ifreq));

	// This has already been checked in cmdargs.c
	// No harm in having it twice in case we change stuff
	if( !(host = gethostbyname(multicast_ip)) )
	{
		write_log(ERRO,"client invalid IP %s", multicast_ip);
		return 0;
	}
	else
		write_log(DBUG,"client set IP %s", multicast_ip);


	memcpy(&iaddr, host->h_addr_list[0], host->h_length);

	// This has already been checked in cmdargs.c
	// No harm in having it twice in case we change stuff
	if( !IN_MULTICAST(ntohl(iaddr.s_addr)) )
	{
		write_log(ERRO,"client non-multicast IP %s", multicast_ip);
		return 0;
	}
	else
		write_log(DBUG,"client set mulitcast IP %s", multicast_ip);

	if((cd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		write_log(ERRO,"client socket creation failed %d", cd);
		return 0;
	}
	else
		write_log(DBUG,"client socket created %d", cd);

	caddr.sin_family = AF_INET;
	caddr.sin_addr.s_addr = htonl(INADDR_ANY);
	caddr.sin_port = htons(0);

	if(bind(cd,(struct sockaddr *)&caddr, sizeof(struct sockaddr_in)) < 0)
	{
		write_log(ERRO,"client socket bind failed");
		return 0;
	}
	else
		write_log(DBUG,"client socket bound");

	loop = 1;
	if(setsockopt(cd, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(unsigned char)))
	{
		write_log(ERRO,"client cannot set multicast loop %d", loop);
		return 0;
	}
	else
		write_log(DBUG,"client set multicast loop %d", loop);


	if(strcmp(interface_ip, DEFAULT_INTERFACE_IP) == 0)
	{
		aaddr.s_addr = INADDR_ANY;
		write_log(DBUG,"Using iterface_ip INADDR_ANY");
	}
	else
	{
		aaddr.s_addr = inet_addr(interface_ip);
		write_log(DBUG,"Using interface_ip %s", interface_ip);
	}


	if(strcmp(interface_name, DEFAULT_INTERFACE_NAME) != 0)
	{
		snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), interface_name);
		if (setsockopt(cd, SOL_SOCKET, SO_BINDTODEVICE, &ifr, sizeof(struct ifreq)) < 0)
		{
			write_log(ERRO,"failed bind to device %s", interface_name);
			return 0;
		}
		else
			write_log(DBUG,"bound to device %s", interface_name);
	}

	if(setsockopt(cd, IPPROTO_IP, IP_MULTICAST_IF, &aaddr, sizeof(struct in_addr)))
	{
		write_log(ERRO,"client cannot set iaddr %s", interface_ip);
		return 0;
	}
	else
		write_log(DBUG,"client set iaddr %s", interface_ip);

	if(setsockopt(cd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl,  sizeof(unsigned char)))
	{
		write_log(ERRO,"client cannot set ttl %d", (int)ttl);
		return 0;
	}
	else
		write_log(DBUG,"client set ttl %d", (int)ttl);

	caddr.sin_family = PF_INET;
	caddr.sin_addr.s_addr = iaddr.s_addr;
	caddr.sin_port = htons(port);

	return 1;
}


int
snd_mcast_msg(const char *msg)
{
	if((sendto(cd, msg, strlen(msg)+1, 0, (struct sockaddr *) &caddr, sizeof(struct sockaddr_in) ))  < 0)
	{
		write_log(ERRO,"client cannot send msg: %s", msg);
		return 0;
	}
	else
		write_log(DBUG,"client sent msg: %s", msg);

	return 1;
}


int
mclient_stop()
{
	int ret = 1;

	if(close(cd))
	{
		write_log(ERRO,"client cannot close socket fd %d", cd);
		ret = 0;
	}
	else
		write_log(DBUG,"client closed socket fd %d", cd);

	return ret;
}
