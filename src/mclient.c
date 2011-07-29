/*
	This file is part of hwmultd.
	Copyright (C) 2011  Anthony G. Basile

	hwmultd is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	hwmultd is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "cmdargs.h"
#include "log.h"
#include "mcast.h"



#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>



int cd;
struct sockaddr_in caddr;


int
mclient_start()
{
	struct hostent *host;
   	struct in_addr iaddr, aaddr;

	// ttl = The time to live for the multicast client.
	// Let's not leave our subnet, so ttl = 1
	unsigned char ttl = 1;

	// Do we loop our multicast messages back to ourselves? 1 = yes, 0 = no
	// We probably don't need this because when operating in both server
	// and client mode, we don't start up a mulitcast server (ie listening),
	// we just pass the message in an internal buffer
	unsigned char loop = 1;

	// Zero the allocated structs
	memset(&caddr, 0, sizeof(struct sockaddr_in));
	memset(&iaddr, 0, sizeof(struct in_addr));
	memset(&aaddr, 0, sizeof(struct in_addr));

	// Get the multicast IP address
	// This has already been checked in cmdargs.c
	// No harm in having it twice in case we change stuff
	if( !(host = gethostbyname(multicast_ip)) )
	{
		write_log(ERRO, __FILE__,"client invalid IP %s", multicast_ip);
		return 0;
	}
	else
		write_log(DBUG, __FILE__,"client set IP %s", multicast_ip);


	memcpy(&iaddr, host->h_addr_list[0], host->h_length);

	// Check if the address is in the multicast range
	// This has already been checked in cmdargs.c
	// No harm in having it twice in case we change stuff
	if( !IN_MULTICAST(ntohl(iaddr.s_addr)) )
	{
		write_log(ERRO, __FILE__,"client non-multicast IP %s", multicast_ip);
		return 0;
	}
	else
		write_log(DBUG, __FILE__,"client set mulitcast IP %s", multicast_ip);

	// Open a UDP client socket
	if((cd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		write_log(ERRO, __FILE__,"client socket creation failed %d", cd);
		return 0;
	}
	else
		write_log(DBUG, __FILE__,"client socket created %d", cd);

	caddr.sin_family = AF_INET;
	caddr.sin_addr.s_addr = htonl(INADDR_ANY);
	caddr.sin_port = htons(0);

	// Bind the client socket to
	//   1) the address family AF_INET (ipv4)
	//   2) the IP address INADDR_ANY (0.0.0.0 for now)
	//   3) the port (0 for now)
	if(bind(cd,(struct sockaddr *)&caddr, sizeof(struct sockaddr_in)) < 0)
	{
		write_log(ERRO, __FILE__,"client socket bind failed");
		return 0;
	}
	else
		write_log(DBUG, __FILE__,"client socket bound");

	// Set our ttl.  Do we want our multicast packets to leave our subnet?
	if(setsockopt(cd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl,  sizeof(unsigned char)))
	{
		write_log(ERRO, __FILE__,"client cannot set ttl %d", (int)ttl);
		return 0;
	}
	else
		write_log(DBUG, __FILE__,"client set ttl %d", (int)ttl);

	// Set our loop.  Do we loop multicast messages back to ourselves?
	if(setsockopt(cd, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(unsigned char)))
	{
		write_log(ERRO, __FILE__,"client cannot set multicast loop %d", loop);
		return 0;
	}
	else
		write_log(DBUG, __FILE__,"client set multicast loop %d", loop);

	// If we have more than one network interface, which do we multicast out of?
	// If we have an interface_ip from our configuration file, then use the
	// interface with that ip, else bind to any (the first?) interface available
	if(strcmp(interface_ip, DEFAULT_INTERFACE_IP) == 0)
	{
		aaddr.s_addr = INADDR_ANY;
		write_log(DBUG, __FILE__,"using iterface_ip INADDR_ANY");
	}
	else
	{
		aaddr.s_addr = inet_addr(interface_ip);
		write_log(DBUG, __FILE__,"using interface_ip %s", interface_ip);
	}


	// Now aim the client socket out of the interface we just determined
	if(setsockopt(cd, IPPROTO_IP, IP_MULTICAST_IF, &aaddr, sizeof(struct in_addr)))
	{
		write_log(ERRO, __FILE__,"client cannot set iaddr %s", interface_ip);
		return 0;
	}
	else
		write_log(DBUG, __FILE__,"client set iaddr %s", interface_ip);


	// Now that we have (ab)used caddr to aim the client socket out the desired
	// interface, let set the correct address family, multicast IP and port
	caddr.sin_family = AF_INET;
	caddr.sin_addr.s_addr = iaddr.s_addr;
	caddr.sin_port = htons(port);

	return 1;
}


int
snd_mcast_msg(const char *msg)
{
	// Multicast out our message, which we expect is a null terminated string
	if((sendto(cd, msg, strlen(msg)+1, 0, (struct sockaddr *) &caddr, sizeof(struct sockaddr_in) ))  < 0)
	{
		write_log(ERRO, __FILE__,"client cannot send msg: %s", msg);
		return 0;
	}
	else
		write_log(DBUG, __FILE__,"client sent msg: %s", msg);

	return 1;
}


int
mclient_stop()
{
	// Close the client socket
	if(close(cd))
	{
		write_log(ERRO, __FILE__,"client cannot close socket fd %d", cd);
		return 0;
	}
	else
		write_log(DBUG, __FILE__,"client closed socket fd %d", cd);

	return 1;
}
