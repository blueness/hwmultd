


#include "cmdargs.h"
#include "log.h"
#include "mcast.h"



#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>



char *msg;
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

	if( !(msg = (char *)malloc(MSG_BUFFER*sizeof(char))) )
	{
		write_log(ERRO, __FILE__, "server cannot malloc message buffer");
		return 0;
	}
	else
		write_log(DBUG, __FILE__, "server malloc-ed message buffer");

	memset(msg, 0, MSG_BUFFER*sizeof(char));

	if( !(host = gethostbyname(multicast_ip)) ) 
	{
		write_log(ERRO, __FILE__, "server invalid IP %s", multicast_ip);
		return 0;
	}
	else
		write_log(DBUG, __FILE__, "server set IP %s", multicast_ip);

	memcpy(&iaddr, host->h_addr_list[0], host->h_length);

	if( !IN_MULTICAST(ntohl(iaddr.s_addr)) )
	{
		write_log(ERRO, __FILE__, "server non-multicast IP %s", multicast_ip);
		return 0;
	}
	else
		write_log(DBUG, __FILE__, "server set multicast IP %s", multicast_ip);

	if((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		write_log(ERRO, __FILE__, "server socket creation failed %d", sd);
		return 0;
	}
	else
		write_log(DBUG, __FILE__, "server socket created %d", sd);

	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_port = htons(port);  

	if(bind(sd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in)) < 0)
	{
		write_log(ERRO, __FILE__, "server socket bind failed");
		return 0;
	}
	else
		write_log(DBUG, __FILE__, "server socket bound");

	imreq.imr_multiaddr.s_addr = iaddr.s_addr;
	imreq.imr_interface.s_addr = htonl(INADDR_ANY);

	if(setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const void *)&imreq, sizeof(struct ip_mreq)))
	{
		write_log(ERRO, __FILE__, "server cannot join multicast group");
		return 0;
	}
	else
		write_log(DBUG, __FILE__, "server joined multicast group");

	return 1;
}


char *
rcv_mcast_msg()
{
	int len = sizeof(struct sockaddr_in);

	memset(msg, 0, MSG_BUFFER*sizeof(char));

	if(recvfrom(sd, msg, MSG_BUFFER, 0, (struct sockaddr *)&saddr, &len ) < 0)
	{
		write_log(ERRO, __FILE__, "server cannot receive msg");
		return "";
	}
	else
		write_log(DBUG, __FILE__, "server recived msg: %s", msg);

	return msg;
}


int
mserver_stop()
{
	free(msg);

	if(close(sd))
	{
		write_log(ERRO, __FILE__, "server cannot close socket fd %d", sd);
		return 0;
	}
	else
		write_log(DBUG, __FILE__, "server closed socket fd %d", sd);

	return 1;
}
