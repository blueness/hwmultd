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
#include <config.h>



#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netdb.h>
#include <arpa/inet.h>
#include <ifaddrs.h>

#include <pwd.h>



// Check and optionally set the values of the GLOBAL VALUEs.
// If any key-value pair is not found, or an illegal value is
// found, the corresponding GLOBAL VALUE is set to the default.
void
sanity_checks()
{
	struct hostent *host;
	struct in_addr iaddr;

        struct ifaddrs *ifa;
        struct sockaddr_in *saddr;

	write_log(INFO, __FILE__, ">>>>> Parameters set");

	// Is the multicast IP address a legal ipv4 address?
	if( !(host = gethostbyname(multicast_ip)) )
	{
		write_log(ERRO, __FILE__, "bad multicast IP %s.  Defaulting to %s",
			multicast_ip, DEFAULT_MULTICAST_IP );
		strncpy(multicast_ip, DEFAULT_MULTICAST_IP, MAX_IP_LEN);
	}
	else
	{
		// If it is, is it a legal mutlicast address?
		memcpy(&iaddr, host->h_addr_list[0], host->h_length);
		if( !IN_MULTICAST(ntohl(iaddr.s_addr)) )
		{
			write_log(ERRO, __FILE__, "non-multicast IP %s.  Defaulting to %s",
				multicast_ip, DEFAULT_MULTICAST_IP );
			strncpy(multicast_ip, DEFAULT_MULTICAST_IP, MAX_IP_LEN);
		}
		else
			write_log(INFO, __FILE__, "Mulitcast IP = %s", multicast_ip);
	}

	// Is the port a legal value?
	if(port < MIN_PORT || MAX_PORT < port)
	{
		write_log(ERRO, __FILE__, "bad port %d.  Defaulting to %d",
			port, DEFAULT_PORT );
		port = DEFAULT_PORT;
	}
	else
		write_log(INFO, __FILE__, "Port         = %d", port);

	// Is the operation mode a legal value?
	if((server_mode != SERVER_MODE ) && (server_mode != CLIENT_MODE) && (server_mode != BOTH_MODE))
	{
		write_log(ERRO, __FILE__, "bad server mode %d.  Defaulting to %d",
			server_mode, DEFAULT_SERVER_MODE);
		server_mode = DEFAULT_SERVER_MODE;
	}
	else
		write_log(INFO, __FILE__, "Server Mode  = %d", server_mode);

	// Is the supplied username a legal user on the system?
	if( !(pwd = getpwnam(user_name)) )
	{
		write_log(ERRO, __FILE__, "no such user name %s.  Defaulting to %s",
			user_name, DEFAULT_USERNAME);
		strncpy(user_name, DEFAULT_USERNAME, UT_NAMESIZE);

		// If not, is the DEFAULT_USERNAME a legal user on the system?
		if( !(pwd = getpwnam(user_name)) )
		{
			write_log(ERRO, __FILE__, "no such user name %s.  Defaulting to %s",
				user_name, FALLBACK_USERNAME);

			// If not, fall back on the guaranteed username
			strncpy(user_name, FALLBACK_USERNAME, UT_NAMESIZE);
			pwd = getpwnam(user_name);
		}
	}
	else
		write_log(INFO, __FILE__, "User name    = %s", user_name);

	// Is the timing a legal value?
	if( (timing < MIN_TIMING) || (MAX_TIMING < timing) )
	{
		write_log(ERRO, __FILE__, "bad timing %d.  Defaulting to %d",
			timing, DEFAULT_TIMING);
		timing = DEFAULT_TIMING;
	}
	else
		write_log(INFO, __FILE__, "Timing       = %d", timing);

	// Is the interface IP to multicast out of a legal IP address?
	//TODO - we need to set to the default value on failure
	if( host = gethostbyname(interface_ip) )
	{
		memcpy(&iaddr, host->h_addr_list[0], host->h_length);

		// If so, is it a legal class A, B or C
		if( !IN_CLASSA(ntohl(iaddr.s_addr)) && !IN_CLASSB(ntohl(iaddr.s_addr))
				&& !IN_CLASSC(ntohl(iaddr.s_addr)) )
			write_log(ERRO, __FILE__, "non-class A, B or C IP %s.  Defaulting.", interface_ip);
		else
			write_log(INFO, __FILE__, "Interface IP = %s", interface_ip);
	}
	else
		write_log(ERRO, __FILE__, "bad interface IP %s.  Defaulting.", interface_ip);

	// Let's get the IP address for all the interfaces
	//TODO - we need to set to the default value on failure
	if( getifaddrs(&ifa) == 0 )
	{
		// Let's loop through all the interface
		while( ifa != NULL )
		{
			saddr = (struct sockaddr_in *) ifa->ifa_addr;

			// This interface matches the interface name we want, so use its IP addr
			if(saddr->sin_family == AF_INET && !strcmp(ifa->ifa_name, interface_name))
			{
				strncpy(interface_ip, inet_ntoa(saddr->sin_addr), MAX_IP_LEN);
				write_log(INFO, __FILE__, "Interface %s IP = %s",
					interface_name, interface_ip);
			}
			ifa=ifa->ifa_next;
		}
	}
	else
		//TODO - this should be "can't get IP addresses for all interfaces"
		write_log(ERRO, __FILE__, "can't get IP for interface %s", interface_name);

	//TODO - check for the existence of the the hw_plugin and/or cl_plugin
	//TODO - we need to set to the default value on failure
	write_log(INFO, __FILE__, "HW Plugin    = %s", hw_plugin_name);
	write_log(INFO, __FILE__, "CL Plugin    = %s", cl_plugin_name);

	// Is the log value a legal value?
	if(log_level < CRIT || DBUG < log_level)
	{
		write_log(ERRO, __FILE__, "bad log level %d.  Defaulting to %d", log_level, DEFAULT_LOG_LEVEL);
		log_level = DEFAULT_LOG_LEVEL;
	}
	else
		write_log(INFO, __FILE__, "Log Level    = %d", log_level);

	write_log(INFO, __FILE__, "<<<<< Parameters set");
}


// The maximum length of a line in the configuration file
#define CONF_LINE_BUFFER 4096
char conf_file[MAX_CONF_DIR_LEN+MAX_CONF_FILE_LEN];

void
parse_cfg_file()
{
	FILE *myfile;
	char conf_line[CONF_LINE_BUFFER], conf_line_orig[CONF_LINE_BUFFER];
	char first[CONF_LINE_BUFFER], second[CONF_LINE_BUFFER];
	int i, selection;

	// Start with default values and let config file override
	strncpy(multicast_ip, DEFAULT_MULTICAST_IP, MAX_IP_LEN);
	port = DEFAULT_PORT;
	server_mode = DEFAULT_SERVER_MODE;
	strncpy(user_name, DEFAULT_USERNAME, UT_NAMESIZE);
	timing = DEFAULT_TIMING;
	strncpy(interface_ip, DEFAULT_INTERFACE_IP, MAX_IP_LEN);
	strncpy(interface_name, DEFAULT_INTERFACE_NAME, MAX_IF_LEN);
	strncpy(hw_plugin_name, DEFAULT_HW_PLUGIN, MAX_PLUGIN_LEN);
	strncpy(cl_plugin_name, DEFAULT_CL_PLUGIN, MAX_PLUGIN_LEN);
	log_level = DEFAULT_LOG_LEVEL;

	strncpy(multicast_ip, DEFAULT_MULTICAST_IP, MAX_IP_LEN);
	port          = DEFAULT_PORT;
	log_level     = DEFAULT_LOG_LEVEL;
	server_mode   = DEFAULT_SERVER_MODE ;

	// Open the config file for reading
	write_log(INFO, __FILE__, "Looking for config file %s", conf_file);
	if( !(myfile = fopen(conf_file, "r")) )
		write_log(INFO, __FILE__, "No config file found");
	else
	{
		// Read one line at a time
		while( fgets(conf_line, CONF_LINE_BUFFER, myfile) )
		{
			// Keep a copy of the config line before we start zeroing chars
			strncpy(conf_line_orig, conf_line, CONF_LINE_BUFFER);

			// Don't parse anything past #, so we'll just zero it
			for( i = 0; i < strlen(conf_line); i++ )
				if( conf_line[i] == '#' )
				{
					conf_line[i] = 0;
					break;
				}

			// Read the key-value pairs
			if( sscanf(conf_line, "%s %s", first, second ) != 2 )
			{
				conf_line_orig[strlen(conf_line_orig) - 1] = 0; //Remove newline
				write_log(DBUG, __FILE__, "skipping config line: %s", conf_line_orig);
				continue;
			}

			//If you find a recognized key, set its corresponding value

			if( !strcmp(first,"MulticastIP") )
				strncpy(multicast_ip, second, MAX_IP_LEN);

			if( !strcmp(first,"Port") )
				if( sscanf(second, "%d", &selection) == 1 )
					port = selection;

			if( !strcmp(first,"Server") )
				if(sscanf(second, "%d", &selection) == 1)
					if( selection == SERVER_MODE || selection == CLIENT_MODE || selection == BOTH_MODE )
						server_mode = selection;

			if( !strcmp(first,"User") )
				strncpy(user_name, second, UT_NAMESIZE);

			if( !strcmp(first,"Timing") )
				if(sscanf(second, "%d", &selection) == 1)
					timing = selection;

			if( !strcmp(first,"SourceIP") )
				strncpy(interface_ip, second, MAX_IP_LEN);

			if( !strcmp(first,"Interface") )
				strncpy(interface_name, second, MAX_IF_LEN);

			if( !strcmp(first,"HWPlugin") )
				strncpy(hw_plugin_name, second, MAX_PLUGIN_LEN);

			if( !strcmp(first,"CLPlugin") )
				strncpy(cl_plugin_name, second, MAX_PLUGIN_LEN);

			if(strcmp(first,"Debug") == 0)
				if( sscanf(second, "%d", &selection) == 1 )
					log_level = selection;
		}

		// We're done, so close the file
		fclose(myfile) ;
	}

	// With all the GLOBAL VALUEs populated, let's sanitize them
	sanity_checks();
}


void
print_help()
{
	printf(
	"Package Name : " PACKAGE_STRING "\n"
	"Bug Reports  : " PACKAGE_BUGREPORT "\n"
	"Program Name : " PACKAGE_NAME "\n"
	"Description  : Broadcast hardware information\n\n"
	"Usage        : %s {[-c] configfile | [-v] | [-h]}\n"
	"options      : -c configfile  Use an non-default config file \n"
	"             : -v  Print out version\n"
	"             : -h  Print out this help\n",
	PACKAGE_NAME
	);
}


void
print_version()
{
	fprintf(stderr, "%s\n", PACKAGE_STRING);
}


// Deal with the command line options
void
parse_cmd_args( int argc, char *argv[] )
{
	int oc ;

	strncpy(conf_file, DEFAULT_CONF_DIR, MAX_CONF_DIR_LEN);

	// TODO - This should be renamed to DEFAULT_CONFIG_FILE and be
	// TODO - made configurable at compile time with a -D
	strcat(conf_file, "/hwmultd.conf");

	while( ( oc = getopt( argc, argv, ":hvc:") ) != -1 )
	{
		switch(oc)
		{
			case 'h':
				print_help();
				exit(EXIT_SUCCESS);
			case 'v':
				print_version();
				exit(EXIT_SUCCESS);
			case 'c':
				strncpy(conf_file, optarg, MAX_CONF_DIR_LEN+MAX_CONF_FILE_LEN);
				break;
			case ':':
				fprintf(stderr, "%s: option -%c requires argument\n", argv[0], optopt);
				break;
			case '?':
			default:
				fprintf(stderr, "%s: option -%c is invalid: ignored\n", argv[0], optopt);
		}
	}
}
