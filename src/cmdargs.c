


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



#undef ME
#define ME "cmdargs.c"



void
sanity_checks()
{
	struct hostent *host;
	struct in_addr iaddr;

        struct ifaddrs *ifa;
        struct sockaddr_in *saddr;

	write_log(INFO, ME, ">>>>> Parameters set");

	if( !(host = gethostbyname(multicast_ip)) )
	{
		write_log(ERRO, ME, "bad multicast IP %s.  Defaulting to %s",
			multicast_ip, DEFAULT_MULTICAST_IP );
		strncpy(multicast_ip, DEFAULT_MULTICAST_IP, MAX_IP_LEN);
	}
	else
	{
		memcpy(&iaddr, host->h_addr_list[0], host->h_length);
		if( !IN_MULTICAST(ntohl(iaddr.s_addr)) )
		{
			write_log(ERRO, ME, "non-multicast IP %s.  Defaulting to %s",
				multicast_ip, DEFAULT_MULTICAST_IP );
			strncpy(multicast_ip, DEFAULT_MULTICAST_IP, MAX_IP_LEN);
		}
		else
			write_log(INFO, ME, "Mulitcast IP = %s", multicast_ip);
	}

	if(port < MIN_PORT || MAX_PORT < port)
	{
		write_log(ERRO, ME, "bad port %d.  Defaulting to %d",
			port, DEFAULT_PORT );
		port = DEFAULT_PORT;
	}
	else
		write_log(INFO, ME, "Port         = %d", port);

	if((server_mode != SERVER_MODE ) && (server_mode != CLIENT_MODE))
	{
		write_log(ERRO, ME, "bad server mode %d.  Defaulting to %d",
			server_mode, DEFAULT_SERVER_MODE);
		server_mode = DEFAULT_SERVER_MODE;
	}
	else
		write_log(INFO, ME, "Server Mode  = %d", server_mode);

	if( !(pwd = getpwnam(user_name)) )
	{
		write_log(ERRO, ME, "no such user name %s.  Defaulting to %s",
			user_name, DEFAULT_USERNAME);
		strncpy(user_name, DEFAULT_USERNAME, UT_NAMESIZE);

		if( !(pwd = getpwnam(user_name)) )
		{
			write_log(ERRO, ME, "no such user name %s.  Defaulting to %s",
				user_name, FALLBACK_USERNAME);
			strncpy(user_name, FALLBACK_USERNAME, UT_NAMESIZE);
			pwd = getpwnam(user_name);
		}
	}
	else
		write_log(INFO, ME, "User name    = %s", user_name);

	if( (timing < MIN_TIMING) || (MAX_TIMING < timing) )
	{
		write_log(ERRO, ME, "bad timing %d.  Defaulting to %d",
			timing, DEFAULT_TIMING);
		timing = DEFAULT_TIMING;
	}
	else
		write_log(INFO, ME, "Timing       = %d", timing);

	if( host = gethostbyname(interface_ip) )
	{
		memcpy(&iaddr, host->h_addr_list[0], host->h_length);
		if( !IN_CLASSA(ntohl(iaddr.s_addr)) && !IN_CLASSB(ntohl(iaddr.s_addr))
				&& !IN_CLASSC(ntohl(iaddr.s_addr)) )
			write_log(ERRO, ME, "non-class A, B or C IP %s.  Defaulting.", interface_ip);
		else
			write_log(INFO, ME, "Interface IP = %s", interface_ip);
	}
	else
		write_log(ERRO, ME, "bad interface IP %s.  Defaulting.", interface_ip);

	if( getifaddrs(&ifa) == 0 )
	{
		while( ifa != NULL )
		{
			saddr = (struct sockaddr_in *) ifa->ifa_addr;
			if(saddr->sin_family == AF_INET && !strcmp(ifa->ifa_name, interface_name))
			{
				strncpy(interface_ip, inet_ntoa(saddr->sin_addr), MAX_IP_LEN);
				write_log(INFO, ME, "Interface %s IP = %s",
					interface_name, interface_ip);
			}
			ifa=ifa->ifa_next;
		}
	}
	else
		write_log(ERRO, ME, "can't get IP for interface %s", interface_name);

	//TODO - check for the existence of the the hw_plugin and/or cl_plugin
	write_log(INFO, ME, "HW Plugin    = %s", hw_plugin_name);
	write_log(INFO, ME, "CL Plugin    = %s", cl_plugin_name);

	if(log_level < CRIT || DBUG < log_level)
	{
		write_log(ERRO, ME, "bad log level %d.  Defaulting to %d", log_level, DEFAULT_LOG_LEVEL);
		log_level = DEFAULT_LOG_LEVEL;
	}
	else
		write_log(INFO, ME, "Log Level    = %d", log_level);

	write_log(INFO, ME, "<<<<< Parameters set");
}


#define CONF_LINE_BUFFER 4096
char conf_file[MAX_CONF_DIR_LEN+MAX_CONF_FILE_LEN];

void
parse_cfg_file()
{
	FILE *myfile;
	char conf_line[CONF_LINE_BUFFER];
	char first[CONF_LINE_BUFFER], second[CONF_LINE_BUFFER];
	int i, selection, tmp_int;

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

	write_log(INFO, ME, "Looking for config file %s", conf_file);
	if( !(myfile = fopen(conf_file, "r")) )
		write_log(INFO, ME, "No config file found");
	else
	{
		while( fgets(conf_line, CONF_LINE_BUFFER, myfile) )
		{
			//Don't parse anything past #
			for( i = 0; i < strlen(conf_line); i++ )
				if( conf_line[i] == '#' )
				{
					conf_line[i] = 0;
					break;
				}

			if( sscanf(conf_line, "%s %s", first, second ) != 2 )
			{
				write_log(ERRO, ME, "malformed config line %s", conf_line);
				continue;
			}

			if( !strcmp(first,"MulticastIP") )
				strncpy(multicast_ip, second, MAX_IP_LEN);

			if( !strcmp(first,"Port") )
			{
				//port = atoi(second);
				if( sscanf(second, "%d", &tmp_int) == 1 )
					port = tmp_int;
			}

			if( !strcmp(first,"Server") )
			{
				//selection = atoi(second);
				if(sscanf(second, "%d", &tmp_int) == 1)
				{
					selection = tmp_int;
					if( selection == 1 )
						server_mode = SERVER_MODE;
					else
						server_mode = !SERVER_MODE;
				}
			}

			if( !strcmp(first,"User") )
				strncpy(user_name, second, UT_NAMESIZE);

			if( !strcmp(first,"Timing") )
			{
				//timing = atoi(second);
				if(sscanf(second, "%d", &tmp_int) == 1)
					timing = tmp_int;
			}

			if( !strcmp(first,"SourceIP") )
				strncpy(interface_ip, second, MAX_IP_LEN);

			if( !strcmp(first,"Interface") )
				strncpy(interface_name, second, MAX_IF_LEN);

			if( !strcmp(first,"HWPlugin") )
				strncpy(hw_plugin_name, second, MAX_PLUGIN_LEN);

			if( !strcmp(first,"CLPlugin") )
				strncpy(cl_plugin_name, second, MAX_PLUGIN_LEN);

			if(strcmp(first,"Debug") == 0)
			{
				//log_level = atoi(second);
				if( sscanf(second, "%d", &tmp_int) == 1 )
					log_level = tmp_int;
			}
		}

		fclose(myfile) ;
	}

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


void
parse_cmd_args( int argc, char *argv[] )
{
	int oc ;

	strncpy(conf_file, DEFAULT_CONF_DIR, MAX_CONF_DIR_LEN);
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
