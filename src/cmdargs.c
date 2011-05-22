
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

#include <cmdargs.h>
#include <log.h>

#include <config.h>


void
sanity_checks()
{
	struct hostent *host;
	struct in_addr iaddr;

        struct ifaddrs *ifa;
        struct sockaddr_in *saddr;

	write_log(INFO, ">>>>> Parameters set");

	if( !(host = gethostbyname(multicast_ip)) )
	{
		write_log(ERRO, "bad multicast IP %s.  Defaulting to %s",
			multicast_ip, DEFAULT_MULTICAST_IP );
		strncpy(multicast_ip, DEFAULT_MULTICAST_IP, MAX_IP_LEN);
	}
	else
	{
		memcpy(&iaddr, host->h_addr_list[0], host->h_length);
		if( !IN_MULTICAST(ntohl(iaddr.s_addr)) )
		{
			write_log(ERRO, "non-multicast IP %s.  Defaulting to %s",
				multicast_ip, DEFAULT_MULTICAST_IP );
			strncpy(multicast_ip, DEFAULT_MULTICAST_IP, MAX_IP_LEN);
		}
		else
			write_log(INFO, "Mulitcast IP = %s", multicast_ip);
	}

	if(port < MIN_PORT || MAX_PORT < port)
	{
		write_log(ERRO, "bad port %d.  Defaulting to %d",
			port, DEFAULT_PORT );
		port = DEFAULT_PORT;
	}
	else
		write_log(INFO, "Port         = %d", port);

	if((server_mode != SERVER_MODE ) && (server_mode != CLIENT_MODE))
	{
		write_log(ERRO, "bad server mode %d.  Defaulting to %d",
			server_mode, DEFAULT_SERVER_MODE);
		server_mode = DEFAULT_SERVER_MODE;
	}
	else
		write_log(INFO, "Server Mode  = %d", server_mode);

	if( !(pwd = getpwnam(user_name)) )
	{
		write_log(ERRO, "no such user name %s.  Defaulting to %s",
			user_name, DEFAULT_USERNAME);
		strncpy(user_name, DEFAULT_USERNAME, UT_NAMESIZE);

		if( !(pwd = getpwnam(user_name)) )
		{
			write_log(ERRO, "no such user name %s.  Defaulting to %s",
				user_name, FALLBACK_USERNAME);
			strncpy(user_name, FALLBACK_USERNAME, UT_NAMESIZE);
			pwd = getpwnam(user_name);
		}
	}
	else
		write_log(INFO, "User name    = %s", user_name);

	if( (timing < MIN_TIMING) || (MAX_TIMING < timing) )
	{
		write_log(ERRO, "bad timing %d.  Defaulting to %d",
			timing, DEFAULT_TIMING);
		timing = DEFAULT_TIMING;
	}
	else
		write_log(INFO, "Timing       = %d", timing);

	if( host = gethostbyname(interface_ip) )
	{
		memcpy(&iaddr, host->h_addr_list[0], host->h_length);
		if( !IN_CLASSA(ntohl(iaddr.s_addr)) && !IN_CLASSB(ntohl(iaddr.s_addr))
				&& !IN_CLASSC(ntohl(iaddr.s_addr)) )
			write_log(ERRO, "non-class A, B or C IP %s.  Defaulting.", interface_ip);
		else
			write_log(INFO, "Interface IP = %s", interface_ip);
	}
	else
		write_log(ERRO, "bad interface IP %s.  Defaulting.", interface_ip);

	if( getifaddrs(&ifa) == 0 )
	{
		while( ifa != NULL )
		{
			saddr = (struct sockaddr_in *) ifa->ifa_addr;
			if(saddr->sin_family == AF_INET && !strcmp(ifa->ifa_name, interface_name))
			{
				strncpy(interface_ip, inet_ntoa(saddr->sin_addr), MAX_IP_LEN);
				write_log(INFO, "Interface %s IP = %s",
					interface_name, interface_ip);
			}
			ifa=ifa->ifa_next;
		}
	}
	else
		write_log(ERRO, "can't get IP for interface %s", interface_name);

	//TODO - check for the existence of the the hw_plugin

	if(log_level < CRIT || DBUG < log_level)
	{
		write_log(ERRO, "bad log level %d.  Defaulting to %d", log_level, DEFAULT_LOG_LEVEL);
		log_level = DEFAULT_LOG_LEVEL;
	}
	else
		write_log(INFO, "Log Level    = %d", log_level);

	write_log(INFO, "<<<<< Parameters set");
}


#define CONF_LINE_BUFFER 1024

void
parse_cfg_file()
{
	// Start with default values and let config file override
	strncpy(multicast_ip, DEFAULT_MULTICAST_IP, MAX_IP_LEN);
	port = DEFAULT_PORT;
	server_mode = DEFAULT_SERVER_MODE;
	strncpy(user_name, DEFAULT_USERNAME, UT_NAMESIZE);
	timing = DEFAULT_TIMING;
	strncpy(interface_ip, DEFAULT_INTERFACE_IP, MAX_IP_LEN);
	strncpy(interface_name, DEFAULT_INTERFACE_NAME, MAX_IF_LEN);
	strncpy(hw_plugin_name, DEFAULT_HW_PLUGIN, MAX_PLUGIN_LEN);
	log_level = DEFAULT_LOG_LEVEL;

	FILE *myfile;
	char conf_line[CONF_LINE_BUFFER];
	char first[CONF_LINE_BUFFER], second[CONF_LINE_BUFFER];

	strncpy(multicast_ip, DEFAULT_MULTICAST_IP, MAX_IP_LEN);
	port          = DEFAULT_PORT;
	log_level     = DEFAULT_LOG_LEVEL;
	server_mode   = DEFAULT_SERVER_MODE ;

	if( !(myfile = fopen(config_file, "r")) )
		write_log(INFO, "No config file found");
	else
	{
		while( fgets(conf_line, CONF_LINE_BUFFER, myfile) )
		{
			sscanf(conf_line,"%s %s", first, second ) ;

			if( !strcmp(first,"MulticastIP") )
				strncpy(multicast_ip, second, MAX_IP_LEN);
			if( !strcmp(first,"Port") )
				port = atoi(second);
			if( !strcmp(first,"Server") )
				server_mode = SERVER_MODE;
			if( !strcmp(first,"User") )
				strncpy(user_name, second, UT_NAMESIZE);
			if( !strcmp(first,"Timing") )
				timing = atoi(second);
			if( !strcmp(first,"SourceIP") )
				strncpy(interface_ip, second, MAX_IP_LEN);
			if( !strcmp(first,"Interface") )
				strncpy(interface_name, second, MAX_IF_LEN);
			if( !strcmp(first,"HWPlugin") )
				strncpy(hw_plugin_name, second, MAX_PLUGIN_LEN);
			if(strcmp(first,"Debug") == 0)
				log_level = atoi(second);
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

	strncpy(config_file, DEFAULT_CONFIG_FILE, MAX_CONFIG_FILE_LEN);

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
				strncpy(config_file, optarg, MAX_CONFIG_FILE_LEN);
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
