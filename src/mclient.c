
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

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
	unsigned char ttl = 1;
	unsigned char loop = 1;

	memset(&caddr, 0, sizeof(struct sockaddr_in));
	memset(&iaddr, 0, sizeof(struct in_addr));
	memset(&aaddr, 0, sizeof(struct in_addr));

	// This has already been checked in cmdargs.c
	// No harm in having it twice in case we change stuff
	if( !(host = gethostbyname(multicast_ip)) )
	{
		write_log(ERRO,"client invalid IP");
		return 0;
	}
	else
		write_log(DBUG,"client set IP");


	memcpy(&iaddr, host->h_addr_list[0], host->h_length);

	// This has already been checked in cmdargs.c
	// No harm in having it twice in case we change stuff
	if( !IN_MULTICAST(ntohl(iaddr.s_addr)) )
	{
		write_log(ERRO,"client non-multicast IP");
		return 0;
	}
	else
		write_log(DBUG,"client set mulitcast IP");

	if((cd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		write_log(ERRO,"client socket creation failed");
		return 0;
	}
	else
		write_log(DBUG,"client socket created");

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
		write_log(ERRO,"client cannot set multicast loop");
		return 0;
	}
	else
		write_log(DBUG,"client set multicast loop");


	if( strcmp(interface_ip, DEFAULT_INTERFACE_IP) == 0 )
	{
		write_log(DBUG,"Using iterface_ip INADDR_ANY");
		aaddr.s_addr = INADDR_ANY;
	}
	else
	{
		write_log(DBUG,"Using interface_ip, %s", interface_ip);
		aaddr.s_addr = inet_addr(interface_ip);
	}

	//struct ifreq ifr;
	//
	//memset(&ifr, 0, sizeof(ifr));
	//snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "eth0");
	//if (setsockopt(s, SOL_SOCKET, SO_BINDTODEVICE, (void *)&ifr, sizeof(ifr)) < 0)
	//{
	//... error handling ...
	//

	if(setsockopt(cd, IPPROTO_IP, IP_MULTICAST_IF, &aaddr, sizeof(struct in_addr)))
	{
		write_log(ERRO,"client cannot set iaddr");
		return 0;
	}
	else
		write_log(DBUG,"client set iaddr");

	if(setsockopt(cd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl,  sizeof(unsigned char)))
	{
		write_log(ERRO,"client cannot set ttl");
		return 0;
	}
	else
		write_log(DBUG,"client set ttl");

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

	if(shutdown(cd, SHUT_RDWR))
	{
		write_log(ERRO,"client cannot shutdown socket");
		ret = 0;
	}
	else
		write_log(DBUG,"client shutdown socket");

	if(close(cd))
	{
		write_log(ERRO,"client cannot close socket fd");
		ret = 0;
	}
	else
		write_log(DBUG,"client closed socket fd");

	return ret;
}
