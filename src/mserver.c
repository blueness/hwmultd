
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

#define MSG_BUFFER 1000

int sd;
struct sockaddr_in saddr;


int
mserver_start()
{
	struct hostent *host;
	struct ip_mreq imreq;
	struct in_addr iaddr;

	memset(&saddr, 0, sizeof(struct sockaddr_in));
	memset(&iaddr, 0, sizeof(struct in_addr));
	memset(&imreq, 0, sizeof(struct ip_mreq));

	if( !(host = gethostbyname(multicast_ip)) ) 
	{
		write_log(ERRO,"server invalid IP %s", multicast_ip);
		return 0;
	}
	else
		write_log(DBUG,"server set IP %s", multicast_ip);

	memcpy(&iaddr, host->h_addr_list[0], host->h_length);

	if( !IN_MULTICAST(ntohl(iaddr.s_addr)) )
	{
		write_log(ERRO,"server non-multicast IP %s", multicast_ip);
		return 0;
	}
	else
		write_log(DBUG,"server set multicast IP %s", multicast_ip);

	if((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		write_log(ERRO,"server socket creation failed %d", sd);
		return 0;
	}
	else
		write_log(DBUG,"server socket created %d", sd);

	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_port = htons(port);  

	if(bind(sd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in)) < 0)
	{
		write_log(ERRO,"server socket bind failed");
		return 0;
	}
	else
		write_log(DBUG,"server socket bound");

	imreq.imr_multiaddr.s_addr = iaddr.s_addr;
	imreq.imr_interface.s_addr = htonl(INADDR_ANY);

	if(setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const void *)&imreq, sizeof(struct ip_mreq)))
	{
		write_log(ERRO,"server cannot join multicast group");
		return 0;
	}
	else
		write_log(DBUG,"server joined multicast group");

	return 1;
}


char *
rcv_mcast_msg()
{
	char *msg;
	int len = sizeof(struct sockaddr_in);
	
	if(!(msg = (char *)malloc(MSG_BUFFER)))
	{
		write_log(ERRO,"server cannot malloc message buffer");
		return NULL;
	}
	else
		write_log(DBUG,"server malloc-ed message buffer");

	if(recvfrom(sd, msg, MSG_BUFFER, 0, (struct sockaddr *)&saddr, &len ) < 0)
	{
		write_log(ERRO,"server cannot receive msg");
		return NULL;
	}
	else
		write_log(DBUG,"server recived msg: %s", msg);

	return msg;
}


int
mserver_stop()
{
	int ret = 1;

	if(close(sd))
	{
		write_log(ERRO,"server cannot close socket fd %d", sd);
		ret = 0;
	}
	else
		write_log(DBUG,"server closed socket fd %d", sd);

	return ret;
}
