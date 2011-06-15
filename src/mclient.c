
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>

#include <cmdargs.h>
#include <log.h>
#include <mcast.h>



#undef ME
#define ME "mclient.c"

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
		write_log(ERRO, ME,"client invalid IP %s", multicast_ip);
		return 0;
	}
	else
		write_log(DBUG, ME,"client set IP %s", multicast_ip);


	memcpy(&iaddr, host->h_addr_list[0], host->h_length);

	// This has already been checked in cmdargs.c
	// No harm in having it twice in case we change stuff
	if( !IN_MULTICAST(ntohl(iaddr.s_addr)) )
	{
		write_log(ERRO, ME,"client non-multicast IP %s", multicast_ip);
		return 0;
	}
	else
		write_log(DBUG, ME,"client set mulitcast IP %s", multicast_ip);

	if((cd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		write_log(ERRO, ME,"client socket creation failed %d", cd);
		return 0;
	}
	else
		write_log(DBUG, ME,"client socket created %d", cd);

	caddr.sin_family = AF_INET;
	caddr.sin_addr.s_addr = htonl(INADDR_ANY);
	caddr.sin_port = htons(0);

	if(bind(cd,(struct sockaddr *)&caddr, sizeof(struct sockaddr_in)) < 0)
	{
		write_log(ERRO, ME,"client socket bind failed");
		return 0;
	}
	else
		write_log(DBUG, ME,"client socket bound");

	loop = 1;
	if(setsockopt(cd, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(unsigned char)))
	{
		write_log(ERRO, ME,"client cannot set multicast loop %d", loop);
		return 0;
	}
	else
		write_log(DBUG, ME,"client set multicast loop %d", loop);


	if(strcmp(interface_ip, DEFAULT_INTERFACE_IP) == 0)
	{
		aaddr.s_addr = INADDR_ANY;
		write_log(DBUG, ME,"using iterface_ip INADDR_ANY");
	}
	else
	{
		aaddr.s_addr = inet_addr(interface_ip);
		write_log(DBUG, ME,"using interface_ip %s", interface_ip);
	}


	if(setsockopt(cd, IPPROTO_IP, IP_MULTICAST_IF, &aaddr, sizeof(struct in_addr)))
	{
		write_log(ERRO, ME,"client cannot set iaddr %s", interface_ip);
		return 0;
	}
	else
		write_log(DBUG, ME,"client set iaddr %s", interface_ip);

	if(setsockopt(cd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl,  sizeof(unsigned char)))
	{
		write_log(ERRO, ME,"client cannot set ttl %d", (int)ttl);
		return 0;
	}
	else
		write_log(DBUG, ME,"client set ttl %d", (int)ttl);

	caddr.sin_family = AF_INET;
	caddr.sin_addr.s_addr = iaddr.s_addr;
	caddr.sin_port = htons(port);

	return 1;
}


int
snd_mcast_msg(const char *msg)
{
	if((sendto(cd, msg, strlen(msg)+1, 0, (struct sockaddr *) &caddr, sizeof(struct sockaddr_in) ))  < 0)
	{
		write_log(ERRO, ME,"client cannot send msg: %s", msg);
		return 0;
	}
	else
		write_log(DBUG, ME,"client sent msg: %s", msg);

	return 1;
}


int
mclient_stop()
{
	if(close(cd))
	{
		write_log(ERRO, ME,"client cannot close socket fd %d", cd);
		return 0;
	}
	else
		write_log(DBUG, ME,"client closed socket fd %d", cd);

	return 1;
}
